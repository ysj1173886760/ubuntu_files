# Consistency level in DBMS

CAP中的C和ACID中的C都代表了consistency，但是他们的含义是完全不同的。

比如ACID中，一致性是指用户定义的语义。指的是我们不会违反引用约束，外键约束，以及一些用户定义的约束。

而在CAP中的C指的是让并发执行的分布式系统表现的像是一个单线程的中心化的系统。在某一时刻的读必须只有一种可能的情况，即他们反映了最近写入的数据

在ACID中，consistency level通常是不会使用的。因为ACID中的C基本上都是application developer的责任——定义正确的事务

所以在说consistency level的时候，我们其实是在说CAP中的C。注意最强的一致性级别中，即strict consistency指的是系统保证所有的读都会反映之前的写。（CAP中的其实指的是atomic consistency，其实是比strict consistency弱一些的，但是在实践中仍然被认为是perfect的）

## Overview of well-known consistency levels

Consistency level的想法源自于共享内存的多核处理器。目的是去推断出在并发执行的线程中，一个线程的读能否感知到另一个线程的写。所以最开始的工作大多数都关注在读写单个数据项上，而非在事务中的一组读写。

![20220402085553](https://picsheep.oss-cn-beijing.aliyuncs.com/pic/20220402085553.png)

在sequential consistency中，所有的写操作是有一个全局的顺序的。所有的线程必须看到的是相同的顺序。在上面的例子中，如果有一个线程发现X被修改成了5,然后又发现Y被改成了10,那么所有的线程都必须保证看到的修改中X发生在Y前面

![20220402085920](https://picsheep.oss-cn-beijing.aliyuncs.com/pic/20220402085920.png)

在这个例子中，P3先观察到了X=5,然后是Y=10,然而P4则是先观察到了Y=10，然后才是X=5。他们之间的顺序产生了冲突，所以这是不满足顺序一致性的。

并且在顺序一致性中，他不会对写的顺序有任何的要求。只要保证全局一致的顺序就行

![20220402090357](https://picsheep.oss-cn-beijing.aliyuncs.com/pic/20220402090357.png)

只要P3和P4同意写X是在写Y之后就可以，不用关心他们具体是怎么被排序的

与顺序一致性不同的是，strict consistency会按照现实时间去排序写操作。而在分布式系统中，我们不可能有一个准确的全局时间来帮我们达到这一点，所以strict consistency可以看作只是理论上的东西。

![20220402090647](https://picsheep.oss-cn-beijing.aliyuncs.com/pic/20220402090647.png)

在分布式系统中，由于我们的读写可以在任意的地方发起，我们所能达到的最高级别的一致性是lineraizability（atomic consistency）。他和strict consistency非常类似，因为他们都要求在顺序一致性的情况下让写操作匹配时间。但是不同的是lineraizability允许操作被提交到系统是有一定时间的，当系统回应说成功的时候，才能视写操作完成。在分布式系统中，我们的复制等操作就在这段时间发生。所以linearizability不会要求那些读写操作的时间有重叠的情况下的顺序。

![20220402091334](https://picsheep.oss-cn-beijing.aliyuncs.com/pic/20220402091334.png)

这里就是一个例子

Causal consistency是一种比较常用的一致性级别。在sequential consistency中，所有的写操作要有一个全局的顺序，即便是这些写操作是完全无关的。而Causal consistency则不要求对于不相关的写的顺序。

![20220402093508](https://picsheep.oss-cn-beijing.aliyuncs.com/pic/20220402093508.png)

什么叫相关写呢，在上面的图中，P2读到了P1的写，然后又写了Y=10,这时候X=5的写和Y=10的写就出现了相关性。那么所有后续的读，如果读到了Y=10,就一定可以读到X=5

这里需要好好想一下。对于sequential consistency来说，我们要有一个全局的order，这里注意是我们的order不能违反单线程执行的情况。比如我们在一个线程中有两个写，那么这两个写应该是按序被感知到的。因为这个线程内部就蕴含了这块的语义，即同一个线程内部的写是有序的。

对于因果一致性来说，我们读一个值A，然后写B，这样构成了这两个值的因果，所以我们需要保证这两个值有一个偏序关系，本质上是因为读操作读到了A的写入，那么就构成了A小于B的偏序关系。

进一步想为什么lamport clock可以解决这个问题？因为当我们为每一个值维护一个计数器的时候，当我们读到了A，也就获得了A上的计数器，后续的写B操作的计数器就一定会比A大，那么在排序中，就可以保证写B这个操作在写A这个操作之后。从而保证了因果一致性

顺序一致性是一定包含因果一致性的，因为顺序一致性不是完全乱排序，在一个线程内部的执行中他们是有序的。比如读A之后写B，那么在顺序一致性中就已经可以确定写A是在写B之前的了，因为关键要注意到一个线程内部本身就是有序的。

在分布式角度来说，顺序一致性就是保证了我们观测到副本的顺序是统一的，比如1号副本上观测到了先A再B，那么2号副本上也应该观测到同样的行为。

而因果一致性不保证两次观测是顺序是相同的，除非他们存在因果依赖