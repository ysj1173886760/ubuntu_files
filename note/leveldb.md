# 整体架构

![20220331203738](https://picsheep.oss-cn-beijing.aliyuncs.com/pic/20220331203738.png)

貌似和bigtable完全相同

level由这几个组建组成

## memtable

是一个主存中的跳表。按序存储。当容量到达阈值的时候就转换成一个不可修改的memtable

## immutable memtable

不可修改的memtable。当他被创建的时候，leveldb的后台压缩进程会利用其中的内容创建一个sstable。持久化到磁盘中

## log

WAL，每次写操作要先顺序写日志。从而保证我们可以从异常状态下恢复。

写操作的原子性也由日志来保证，当日志成功写入的时候，写操作成功。当写日志异常的时候，后续的恢复会丢掉这个日志。写操作被视为失败

## sstable

leveldb的数据主要通过sstable来存储。leveldb后台会定期进行compaction，随着compaction的进行，sstable文件在逻辑上被分为若干层。

内存数据dump出来的文件为level 0层。compaction出来的文件为level i层

sstable文件不可修改

## manifest

leveldb记录了每一层中所有文件的元数据，包括文件大小，最大key值，最小key值。通过维护这些元数据我们可以加快查找，以及控制compaction进行。

这些元信息被存储在level的版本信息中

leveldb每次compaction完成都会创建一个新的version。而manifest文件记录了每次version的变化。

![20220331205007](https://picsheep.oss-cn-beijing.aliyuncs.com/pic/20220331205007.png)

每个manifest记录了每次版本变更（1）添加了那些sst文件，（2）删除了那些sst文件，（3）当前compaction下标，（4）日志文件编号，（5）操作seqNumber等信息。

我们通过不断apply这些记录最后得到level上一次运行结束的版本信息

## current

current记录了当前manifest文件名

# 读写操作

![20220331205424](https://picsheep.oss-cn-beijing.aliyuncs.com/pic/20220331205424.png)

leveldb的写入有两部分，先写日志。然后在把写操作应用到内存中

leveldb提供两种写操作，put和delete

此外leveldb提供了一个批处理工具Batch。我们可以通过Batch来完成批量的原子操作

## Batch

![20220331205715](https://picsheep.oss-cn-beijing.aliyuncs.com/pic/20220331205715.png)

无论是正常操作还是批量操作底层都会创建一个batch作为数据库操作的最小单元

batch中每个数据都按照上面的格式编码。type表示是更新还是删除

数据从batch写入到内存中的时候，需要一个key值的转换，这种格式成为internalKey

![20220331205929](https://picsheep.oss-cn-beijing.aliyuncs.com/pic/20220331205929.png)

尾部追加了sequence number，每次操作都会递增这个数。在leveldb中的操作采用的是append的方式，所以对于一个key会有多个版本的记录，使用最大的sequence number就是最新的数据

每一个sequence number就代表了数据库的一个版本

## 合并写

leveldb中对于并发写入的时候，同一时刻只允许一个写入操作将内容写入到主存以及日志中。为了增加日志的写入性能，leveldb将一些小写入合并成一个大写入。

对于第一个获取到写锁的写操作
* 第一个写入操作获取到写入锁
* 当前写操作的数据量未超过合并上线，并且有其他写操作等待的情况下，就将其他写操作的内容合并到自身
* 若本次写操作的数据量超过上线，或者没有其他写操作等待了，就将所有内容统一写入到日志文件，并写入到内存中
* 通知每一个被合并的写操作的最终写入结果，释放或移交写锁

其他写操作
* 等待写锁或者被合并
* 若被合并判断是否成功，成功则等待最终写入结果，否则则接过写锁并写入
* 未被合并则等待写锁或者等待被合并

![20220331210724](https://picsheep.oss-cn-beijing.aliyuncs.com/pic/20220331210724.png)

## 读操作

用户可以通过Get直接读数据，或者创建一个snapshot然后基于该snapshot用Get读数据

本质上第一种方法是基于当前状态的快照的读，所以两种都是基于快照进行读

leveldb中，通过一个整数来代表数据库的状态。

上面提到的每一个sequence号都可以作为leveldb的一个状态快照

![20220331211103](https://picsheep.oss-cn-beijing.aliyuncs.com/pic/20220331211103.png)

通过快照来保证数据库进行并发的读写。

获取到一个快照后，leveldb会为本次查询的key构建一个internalKey，其中seq字段使用的就是快照的seq，从而过滤掉所有seq大于快照号的数据项

![20220331211331](https://picsheep.oss-cn-beijing.aliyuncs.com/pic/20220331211331.png)

leveldb的读取分为三步

1. 在memory db中查找指定的key，如果成功则结束查找
2. 在冻结的memory db中查找指定的key，如果成功则结束查找
3. 按从低到高的顺序在level i层中查找key，若找到则结束。否则返回NotFound

注意在leveldb中第0层，可能出现key重合的情况，所以要优先找文件号大的sstable。而在非0层中，所有文件之间的key不重合，所以可以借助sstable的元数据来快速定位。所以每一层只要读一次。