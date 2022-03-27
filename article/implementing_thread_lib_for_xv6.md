# 为xv6添加线程库支持的设计

为xv6添加线程库支持，我们有两个点需要考虑。

1. 生成新的线程
2. 线程之间的同步

生成新的线程思路和linux中的lwp是一样的

具体可以看这封[邮件](https://lkml.iu.edu/hypermail/linux/kernel/9608/0191.html)

生成新的线程的思路和fork基本类似，只不过我们这里会把除了栈以外的地址空间共享。

这时候有个问题就是多个线程共享同一个地址空间的时候，我们只能释放一次，而不能让每个线程退出的时候都释放。

所以我们需要一个引用计数的结构。当两个进程共享地址空间的时候，他们就会共享这个引用计数器。只有最后一个离开的进程才需要释放整个地址空间

由于没有实现信号，所以可以暂时不考虑一个进程退出会使其他的线程都退出的情况。

对于第二点，线程之间的同步。我先说一个比较直观的思路

比如实现一个spinlock，我们就直接在用户态去malloc一个自旋锁，然后用原子指令去自旋就行

注意这里是不能关中断的。因为这里是用户态。

但是对于条件变量来说，我们需要实现的一个操作是原子的放锁加上睡眠。

睡眠肯定是要让内核来管理的，但是我们还需要放锁。这就需要我们把用户态的锁引入到内核中。让内核来帮我们放锁

这样就增加了耦合性。

所以为了简化实现。我们可以让用户通过原子操作自己实现自旋锁。

我们通过内核提供条件变量和互斥锁。这样用户可以基于这两个实现其他类型的锁。

我们通过提供四个系统调用，分别是

`mutex_lock(int mutex_handler)`

`mutex_unlock(int mutex_handler)`

`cv_wait(int cv_handler, int mutex_handler)`

`cv_signal(int cv_handler)`

我们在内核内部初始化并通过free list来管理空闲的mutex和cv

mutex就是我们在内核中使用的sleeplock

而cv则是一个队列，在wait的时候就可以把对应的进程的pid加入到这个cv的等待队列中。并且释放对应的mutex，然后yield cpu

signal的时候就可以唤醒等待队列中的进程。这时候这些进程会从上面yield cpu的地方回来，然后重新尝试获得mutex，并回到用户态

基本思路就是这些，但是还有些细节需要额外考虑，比如如果永远没有人去signal一个进程。或者mutex没人释放。

这里需要我们额外的在进程中维护正在使用的mutex和cv。在进程退出的时候，如果有人在cv上等待，或者在锁上等待，就释放锁。再回收资源。

更加细节的问题目前还没有考虑到。