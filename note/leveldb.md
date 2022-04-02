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

# 日志

![20220401200850](https://picsheep.oss-cn-beijing.aliyuncs.com/pic/20220401200850.png)

leveldb中，有两个memory db，对应了两份日志文件。当一个memory db被冻结的时候，与之对应的log也会变成frozen log

当后台的minor compaction进程将其转换成一个sstable文件持久化以后，对应的frozen log就会被删除

## 日志结构

![20220401201052](https://picsheep.oss-cn-beijing.aliyuncs.com/pic/20220401201052.png)

日志按照block划分，每个block大小为32kb

一个日志记录包含一个或多个chunk，每个chunk包含一个7字节大小的header，前4字节是checksum，2字节是数据长度，最后一个字节是chunk类型

chunk有四种类型，full，first，middle，last。一条日志记录只包含一个chunk，则chunk为full。如果包含多个chunk，那么第一个是first，最后一个是last，中间则是middle。

因为一个block大小为32kb，所以当日志过大的时候，我们就会拆分数据，并把chunk划分成first，middle，last的格式

## 日志内容

日志为写入的batch编码后的信息

![20220401201707](https://picsheep.oss-cn-beijing.aliyuncs.com/pic/20220401201707.png)

header中包含了sequence number，以及本次日志中包含的put/del操作的个数

后面则是所有batch编码后的内容

## 日志写

![20220401201923](https://picsheep.oss-cn-beijing.aliyuncs.com/pic/20220401201923.png)

写入的时候不断判断buffer的大小，如果超过32kb就作为一个完整的chunk写入文件。写完数据注意要计算对应的header

## 日志读

![20220401204616](https://picsheep.oss-cn-beijing.aliyuncs.com/pic/20220401204616.png)

一次读一个block，每次读一个chunk，并校验数据，直到读到最后一个chunk，返回整个日志记录

# 内存数据库

memory db用来维护有序的key-value对，底层是用跳表实现

## 跳表

跳表利用了概率均衡的技术，从而加快并且简化操作。

![20220402140415](https://picsheep.oss-cn-beijing.aliyuncs.com/pic/20220402140415.png)

![20220402140439](https://picsheep.oss-cn-beijing.aliyuncs.com/pic/20220402140439.png)

从链表起一步一步引出跳表的结构

在a中，所有元素按序排列，存储在一个链表中

在b中，每隔两个节点，新增一个额外的指针，指向距离为2的下一个节点

在c中，在b的基础上，每隔四个节点新增一个指针

不断类推我们就可以得到查询效率为logn的结构，但是这样的结构使得每次插入和删除都会异常的复杂

一个拥有k个指针的结点称为一个k层节点，按照上面的逻辑，50%的节点为1层节点，25%的节点为2层节点....。如果我们保证每层分布的节点如上面的概率所述，那么我们仍然可以达到相同的查询效率。e图就是一个示例。

### 结构

![20220402141141](https://picsheep.oss-cn-beijing.aliyuncs.com/pic/20220402141141.png)

跳表是按照层来构造的。底层是一个普通的有序链表。每个更高层都充当下面链表的快速通道，这里在层i中的元素按某个固定的概率p出现在层i+1中。平均起来，每个元素都在1/(1-p)的概率在这一层中出现，而最高层的元素在O(log1/p n)个节点中出现

### 查找

![20220402141610](https://picsheep.oss-cn-beijing.aliyuncs.com/pic/20220402141610.png)

图中，我们要查找一个值为17的链表节点，过程为：
* 首先根据条表的高度选择最高层的头节点
* 如果该节点的值小于查找节点的值，就用该层的下一个节点继续比较
* 如果值相同则直接返回
* 如果该节点的值大于要查找的值，且层高不为0,降低层高并且从前一个节点开始重新查找。如果层高为0则返回当前节点

这里其实原文的说法怪怪的，因为他的视角来说所谓的当前节点就是已经跳过去的节点

### 插入

插入是借助于查找实现的

在查找的过程中，不断记录每一层的前任节点

![20220402142730](https://picsheep.oss-cn-beijing.aliyuncs.com/pic/20220402142730.png)

然后为新插入的节点随机选择层高。然后在合适的位置插入新的节点。并且根据之前记录的前任节点的信息，以链表插入的方式插入到每一层的链接中

### 删除

删除则较为简单，依赖查找过程找到该节点在跳表中的位置后，直接链表删除就可以

### 迭代

#### 向后遍历

* 若迭代器刚被创建，则根据用户指定的查找范围[Start, limit)在链表中找到*第一个*符合条件的跳表节点
* 若迭代器处于中部，则取出上一次访问的跳表节点的后继节点，作为本次访问的跳表节点
* 利用跳表节点信息获得数据

#### 向前遍历

* 若迭代器刚被创建，则根据用户指定的查找范围[Start, limit)在跳表中找到*最后一个*符合条件的跳表节点
* 若迭代器处于中部，则利用上一次访问的节点的key值，查找比该key值更小的跳表节点
* 利用跳表节点信息，获取数据

## 数据组织

![20220402144607](https://picsheep.oss-cn-beijing.aliyuncs.com/pic/20220402144607.png)

以go为例

其中kvData存储每一条数据项的kv数据，nodeData用来存储每个跳表节点的链接信息

nodeData中，每个跳表节点占用一段连续的空间，每一个字节分别用来存储特定的信息

* 第一个字节用来存储本节点kv数据在kvData中对应的偏移量
* 第二个字节用来存储本结点key值长度
* 第三个字节存储本节点value值长度
* 第四个字节用来存储本节点层高
* 第五个字节开始，用来存储每一层对应的下一个节点的索引值

（有点奇怪的是如果这样存储节点，我们怎么回收内存呢？）

答案是不回收，我们只添加不删除。删除操作其实是添加一个空的节点