# Partial Thoughts on WSI

今天看到了一个论文。说的是原始的SI要求检测WW冲突其实是没必要的。因为新的写入就是添加新的版本。两个写入不会影响到对方。那为什么要检测WW冲突呢？

检测WW冲突说的是如果两个事务写了同一个数据项。那么他们的[StartTS, CommitTs]，这个区间需要没有交集。即一个改完之后提交，另一个才能开始改。比如在percolator中提交的时候会检测write列，有没有新的写入。

然而之所以我们希望检测WW冲突实际上是为了防止丢失更新。

丢失更新丢失的其实是我们Read-Modify-Update这个操作的Update。比如两个并发的Add 1。然而这个异常出现的根本原因其实不是WW冲突。而是一个人的更新导致另一个人的假设失效了。其实是出现了rw-conflict。即两个人都对对方有rw-antidependency。从而出现了冲突。

所以WSI的核心思路就是检测rw-conflict。即跟踪一个事务的读集，在提交的时候看他的读集有没有被其他人修改。而对于WW冲突则无需在意。如果我们写入的时机是在最后的提交时刻，那么就不会引起脏读，也就没有必要检测WW冲突。