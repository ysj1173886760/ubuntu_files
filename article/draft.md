Step1： SQL Server与客户端交互，读请求可以直接去下层KV读取快照，写请求则写入到Buffer中。当发起提交的时候则发送整个Txn给一个Replication Node

![20220512090901](https://picsheep.oss-cn-beijing.aliyuncs.com/pic/20220512090901.png)


Step2： 每个Replication Node转发不属于他自己分片的数据。这里是一个同步点，每个服务器必须获取了该Epoch下所有的数据后才能开始merge rowheader

![20220512091427](https://picsheep.oss-cn-beijing.aliyuncs.com/pic/20220512091427.png)

Step3： 每个Replication Node收到其他节点的信息后，开始merge row header。当一个shard内的txn可以提交的时候，表示这个txn在这个shard的数据是可以提交的。并将这个信息进行区域内的广播。这时候每个节点都可以知道自己负责的txn是不是在其他shard上都可以提交。
另一种思路就是以shard的写集为粒度。对于一个shard的txn x的写集来说，只有其他所有shard对应的txn x的写集都可以提交成功的时候，我们就可以把写集发送给远端

![20220512091813](https://picsheep.oss-cn-beijing.aliyuncs.com/pic/20220512091813.png)

Step4： 将可以提交的事务发送给远端。这里可以发送txn，也可以发送每个Shard对应的写集。
对于发送txn来说，我们需要执行类似Step2的转发操作。
对于发送写集来说，由于我们不希望暴露分片信息给其他Region，所以转发也是必要的

![20220512092646](https://picsheep.oss-cn-beijing.aliyuncs.com/pic/20220512092646.png)

Step5： Region内需要同步，他们要确保收满了其他所有Region的数据后才能开始merge rowheader

![20220512093330](https://picsheep.oss-cn-beijing.aliyuncs.com/pic/20220512093330.png)

Step6： 和Step3相同，每个分片处理冲突，并把信息广播到区域内的其他节点上。这时候每个节点就可以知道那些数据可以提交

![20220512093607](https://picsheep.oss-cn-beijing.aliyuncs.com/pic/20220512093607.png)

Step7： 写回数据

![20220512094238](https://picsheep.oss-cn-beijing.aliyuncs.com/pic/20220512094238.png)

其实可以发现，我们每一次的同步都是因为把一个节点拆分成多个节点导致的。每次收到数据需要确认收全，这是一次同步。每次Merge row header结束后需要把信息广播给其他节点。这是第二次同步。而我们有两次MultiMaster OCC的过程（一次本地，一次全局），所以一共是4次同步

当然我们可以省略掉第一次本地的OCC（但是需要保证同一个事务不同Shard的数据是在一个Epoch下的）