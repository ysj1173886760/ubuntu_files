# In Search of An Effecient Multi-Master Concurrency Control Algorithm

针对GeoGauss的一些小的idea/insight，不局限于并发控制

有些地方可能大家已经提过了

## Cascading Abort

![20220426171717](https://picsheep.oss-cn-beijing.aliyuncs.com/pic/20220426171717.png)

最后只有B提交

一个insight，按序应用写集，但是检查提交的时候逆序检查

``` Rust
for write_set in txn_list {
    Apply(write_set)
}

for (write_set, timestamp) in txn_list.rev() {
    let mut state = Commit;
    for entry in write_set {
        if timestamp != table[entry.id].timestamp {
            state = Abort;
            break;
        }
    }
    if state == Commit {
        continue;
    }
    // rollback the row header
    for entry in write_set {
        if table[entry.id].timestamp == timestamp {
            Rollback(entry);
        }
    }
}

// write-back
```

这样的话，我们发现TxnB被Abort的时候，他就可以把A上面的row header回滚，从而保证TxnA的提交

需要我们额外保存before-timestamp，并且更细节的情况还需要仔细的考虑

## Serializability

实现可串行化的并发控制算法要求我们跟踪读集，比如PostgreSQL使用了SIREAD，Microsoft's Hekaton。要求我们在提交事务之前进行验证，保证读没有被修改（比如重新执行一次读操作）

多个主节点之间发送读集的开销是相当大的

受到这篇论文[Fast Serializable Multi-Version Concurrency Control
for Main-Memory Database Systems]的启发，我们可以不去追踪读集，而是追踪谓词，具体的名字叫Precision Locking

论文中的一个示例

![20220426173731](https://picsheep.oss-cn-beijing.aliyuncs.com/pic/20220426173731.png)

由于我们每个节点本来就有了所有的写集，我们只需要验证从事务开始到提交这段时间中的写集，和事务的读集有无冲突

具体一点，当一个事务T提交的时候，他可以找到在$T_{start-timestamp}$到$T_{commit-timestamp}$之间的成功提交事务的写集。然后把事务T中的每个读谓词都应用到这些写集中，当有写集满足了我们的谓词，说明他更改了我们的读，我们应该abort

利用precision lock的一个好处就是，他可以提供更细粒度的检查。比如在Postgres中的SIREAD会锁住整个行。但是如果修改的是这个行，但不是我们读的那个列的话，其实修改是没有影响的。所以我们这里实际上跟踪了谓词以及读取的属性，从而获得更细粒度的检索，降低Abort rate

```Rust
fn Commit(txn: Transaction) {
    let write_set = FetchWriteSet(txn.beginEpoch, currentEpoch);
    for (predicate, attributes) in txn.read_lists {
        for entry in write_set {
            if entry.Attributes().Intersect(attributes) &&
               Apply(predicate, entry) {
                txn.SetAbort();
                break;
            }
        }

        if txn.IsAbort() {
            break;
        }
    }

    return !txn.IsAbort();
}
```

好处就是这个验证是可开关的，并且谓词的传输开销相对于写集来说要小很多。如果实现正确的话，我们基本上可以不引入额外的开销。

challenge:
1. 在单节点数据库中，我们可以很容易定义一个事务的开始时间和结束时间。但是在多主架构下，我们需要确定可能影响事务读的那些写集。一个比较粗粒度的方法就是记录每个事务读的是哪个Epoch的快照，记为$E_{start}$，然后验证的时候就验证从$E_{start}$到这个事务自己的Epoch中间的那些写集。更细粒度的方法则是要考虑当前Epoch自己的位置，也就是要结合之前的OCC来修改
2. 要正确的实现要对目前的系统有比较深入的理解。追踪谓词，以及验证的实现都不是很容易

## Hybrid Management

从Neutronstar那里借鉴来的思路

传递数据/传递SQL的tradeoff

传递SQL的优势就占用带宽小，但是需要我们在远端重新解析

而传数据（写集/读集/谓词）的优势就是我们不需要重新解析，但是传输开销大

Communication/Computation tradeoff?

通过利用查询优化器给出的预估结果（或者先在本地执行），来决定是传输写集还是传输SQL

比如数据量比较大，我们可以选择传SQL，数据量小，可以传写集，从而避免重复解析的开销

```Rust
// example
fn process(query: SQL) {
    let ast = Parse(query);
    let plan = Planner(ast);
    if Estimate(TransmissionOverhead(plan), ExecutionOverhead(plan)) {
        Communicator::Transmit(query);
    } else {
        let write_set = Execute(plan);
        Communicator::Transmit(write_set);
    }
}

fn receiver(data: RemoteData) {
    let write_set;
    if data.IsSQL() {
        let ast = Parse(data);
        let plan = Planner(ast);
        write_set = Execute(plan);
    } else {
        write_set = Deserialize(data);
    }

    TransactionManager::Append(write_set);
}
```

challenge：
1. 细节需要仔细考虑，对于混合的方法来说，我们需要像写集一样提前定序，给出他要读的快照，以及他所属的Epoch。否则不同远端可能导致解析结果不同。而且SQL的重新解析以及执行可能会拖慢这个Epoch的快照的生成时间，一个解决办法就是把它推到未来的Epoch中
2. 不清楚这个方法能不能有提高，因为这里的计算和通信不像图计算中那样明显，我们更多还是数据密集型
3. 不容易实现，实现起来贯穿整个DBMS，需要保证我们的代码不会导致bottleneck

## Analytical Processing

这里是我个人的猜想，可能不太正确

多主架构的一个主要优势就是我们的读是不需要与其他节点通信的（对比分片的架构，读可能跨越多个KV server，并且不容易处理复杂查询）

我们可以测试HTAP的workload，可能表现不错？

对于HTAP的情况来说，有一个很关键的点就是对于长时间运行的查询操作，他会读历史某一个版本的快照，从而导致垃圾回收的阻塞

![20220426181942](https://picsheep.oss-cn-beijing.aliyuncs.com/pic/20220426181942.png)

![20220426182241](https://picsheep.oss-cn-beijing.aliyuncs.com/pic/20220426182241.png)

利用我们是基于Epoch更新的性质，我们可以使用基于Epoch的细粒度的GC，从而让版本链保持在一个比较短的长度上

![20220426182840](https://picsheep.oss-cn-beijing.aliyuncs.com/pic/20220426182840.png)

假设目前有一个查询在读Epoch7的快照，那么前面就会出现很多无用的版本

![20220426183107](https://picsheep.oss-cn-beijing.aliyuncs.com/pic/20220426183107.png)

我们的优势就是写操作的更新是Epoch粒度的，那我们就可以在Epoch结束的时候遍历本次Epoch的写集，并尝试压缩版本链

这个思想在单机数据库中已经有了，但是没有细粒度的Epoch-based的方法。我们这里只是把它拓展到多机（因为每个机器的GC都是不同的），并且适用到了我们的Epoch-based的方法中

## Abstraction Layer

这个就是一个设想了，我们可以把这些方法抽象成一个层，或者类似中间件一样。这样单机的数据库就可以应用我们的方法（多主的coordination free，replication等）来拓展到分布式的情况