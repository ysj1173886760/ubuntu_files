# Fault-tolerate replication scheme in multi-master distributed systems

我们要保证每一个epoch在做CC的时候，每一个节点应用的事务都是相同的

![20220421100659](https://picsheep.oss-cn-beijing.aliyuncs.com/pic/20220421100659.png)

之前是把收到的事务直接传给了Transaction Layer，所以导致在replication的时候我们不能保证每个节点收到的事务是相同的

之前也提到过，对比现有的分布式数据库，我们这里其实是把Replication Layer从Storage Layer这一层提上来了。

比如Spanner，TIDB这样的

![20220421101756](https://picsheep.oss-cn-beijing.aliyuncs.com/pic/20220421101756.png)

对于数据的保证是在KV层面做的，也就是用共识协议

对于事务的保证是在上层做的，用的是2PC（Percolator）

之前说过我们其实是把这两个合并到一起了，事务的协调和数据的复制都放到了Replication Layer中

那为了保证Replication Layer传输数据的正确性，我们也可以像上面的架构一样添加共识协议

![20220421102726](https://picsheep.oss-cn-beijing.aliyuncs.com/pic/20220421102726.png)

三个节点的情况，上层会把写集发给每个节点的Leader，每个节点只有一个Leader，就负责把他的写集同步给其他节点

下层的Transaction Layer会从上面的1个Leader以及n - 1个Follower中读取数据

当成功从Follower中读取到epoch x的数据的时候，我们就可以认为这个数据已经被持久化到系统中了。

对于Epoch x来说，当他读到了n个节点中的epoch x的数据的时候，他就可以安全的把这个数据提交

换句话说，对于一个Epoch中的所有写集，我们都可以保证他已经被复制到了大多数节点中，这时候可以安全的提交。因为这个写集不会丢失

正确性是可以保证的，但是性能？

对于共识协议来说，以Raft举例，让Follower成功提交一个日志所需要的最小的时间就是1.5个RTT

![20220421103854](https://picsheep.oss-cn-beijing.aliyuncs.com/pic/20220421103854.png)

对于正常的KV storage来说这就是极限了，但是我们的系统是多主，每个节点都是自己Raft Group的leader，每个人都会相互交换信息，有没有可能根据这个特性去优化呢？

首先，send log这个地方是无法避免的，我们只可能让一个epoch在send log的时候去piggyback前面epoch的确认信息，但是不会对我们当前epoch的操作造成影响，当然了Raft本身就有这个优化。

![20220421105212](https://picsheep.oss-cn-beijing.aliyuncs.com/pic/20220421105212.png)

然后再看，对于普通的KV storage来说，他们的通信过程

![20220421105544](https://picsheep.oss-cn-beijing.aliyuncs.com/pic/20220421105544.png)

我们的系统则有Follower之间的通信。当然了，对于正常的分布式KV来说，他们Follower对应的物理节点肯定也会有通信，但是他们不会对这个做假设（不确定，可能他们就有这方面的优化）。然而我们有这个假设，就是一个节点就是自己发送写集的leader

然后再看，Raft中的决策点在leader上，leader收集了大多数的ack，确定这个log可以提交，然后再告诉Follower去提交他

这里想的一个优化点就是提前决策，Follower可以不只是发给Leader确定信息，而是把这个信息发送给所有人

那么一个Follower就可以收到所有人的确认信息，当他发现确认信息超过大多数的时候，我们就可以提前把这个log提交给Transaction Layer

这就可能带来一个问题，Follower的提前决策，会不会出现brain split的问题？

对应的就是两种情况

1. Leader认为这个log可以提交，但是follower认为不行，这时候日志不会提交，而是等待Leader的提交信息传过来，也就是退化到普通的Raft流程
2. Leader认为这个log不能提交，但是follower认为可以，这种情况下是Ack丢包或者非对称性的网络分区导致的，对于丢包来说，可以在后面的确认信息中重新达成一致-即Leader和Follower都认为Log可以提交，而对于非对称性的网络分区的情况下，Leader最终会下位，即不再有决策权

总结一下，就是每个follower不再只是向leader确认自己的信息，而是向所有人确认，这样其他的follower就可以提前知道决策信息，从而提前提交。

有一个有意思的问题是为什么目前的实现里(etcd, TiKV)没有这么做呢？因为他们的目的其实都是为了可线性化，在Follower上实现可线性化的时候，必须要从leader中获取当前时刻的CommitIndex，然后当Follower推进到CommitIndex的时候再提交，才能保证可线性化读。而我们的系统实际上不需要可线性化读，因为他存储的只是写集，他只要按照epoch一个一个往下读就行，而不需要读最新的。

所以这个做法其实是针对这个系统的一个对共识协议的优化。即我们只需要Raft中的持久化的能力，而不需要线性化

还有一个好处就是可以快速检查宕机，如果一个节点有了两个Leader，说明有一个Leader对应的节点失效了，同时如果他当选了Leader，证明他包含了最新的日志，他也就有能力把剩下的日志同步下去，而且他知道这个节点是在那个epoch宕掉的，他就可以把这个信息广播给所有人，或者写入到集群管理服务中。从而安全的让节点下线

并且通过共识协议可以帮我们持久化写集，这样在Crash Recovery的时候就可以直接通过共识协议去同步写集，从而恢复节点的状态。这里有个缺点就是一个节点的状态是由所有的Raft Group共同决定的，所以在实现snapshot的时候可能会有一定的困难。

还有就是会一定程度的提高外层的复杂度，因为共识协议要求我们有重试的操作。并且模块之间的交互以及实现的细节还要仔细考虑。