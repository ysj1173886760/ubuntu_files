### 多核架构下的中断处理

来一句题外话，本以为我目前的体系结构相关的知识已经足够充分了，但没想到还是不够细节。在理解xv6的过程中，通过教授的一些assertion，我才找到了自己对于中断理解的一些错误，这里特此记录下来。

问题的起因是这样的，在xv6中，中断处理程序一般都需要用lock来防止race condition，所以我就理解成了lock是用来防止多核同时执行一个中断处理程序时，在操作共享数据结构的时候导致的race

但是xv6的net lab中，教授的solution在中断处理程序中没有用lock。同时在课上，他也说了不会有两个core同时执行这个interrupt handler

这引起了我的疑问，我又回到xv6中尝试输出了每次执行interrupt handler的hart id，发现他是改变的。也就是说，并不存在所有的中断都由core0来处理

那么问题来了，我们是怎么保证中断处理程序不会被其他core同时执行呢

首先说明一下上面的一个问题，在interrupt handler中的lock并不是防止相同的handler的race，而是防止top half和bottom half进行race

因为我们decouple了与设备交互的过程，比如UART，将字符填入缓冲区是top half的操作，然后bottom half负责consume

在这个过程中，我们用了共享的数据结构来实现这个producer-consumer的模型，所以为了防止producer和consumer冲突，我们才用了lock

而这个模型并不是多个producer多个consumer，而是多个producer单个consumer

我们可以有多个top half来尝试与设备进行通信，但是回复的interrupt handler只能由一个core来完成

ok，那么现在就是怎么保证只有一个core处理interrupt的问题了

一般的计算机组成原理的教材中都会说到，设备issue一个中断，然后一个core来处理这个中断

![20220128201835](https://picsheep.oss-cn-beijing.aliyuncs.com/pic/20220128201835.png)

这里来一个recap

![20220128202042](https://picsheep.oss-cn-beijing.aliyuncs.com/pic/20220128202042.png)

这里我们关注的就是PLIC和External interrupt

![20220128202122](https://picsheep.oss-cn-beijing.aliyuncs.com/pic/20220128202122.png)

可以看到，PLIC是IRQ style的aggregator

![20220128202223](https://picsheep.oss-cn-beijing.aliyuncs.com/pic/20220128202223.png)

我们的硬件通过IRQ连接到Gateway中，Gateway用来转发interrupt，并且如果已经有interrupt在等待了，Gateway负责停止接受更多的interrupt

然后由PLIC core负责处理优先级，以及优先级的threshold

然后我们看一下他的workflow

![20220128202544](https://picsheep.oss-cn-beijing.aliyuncs.com/pic/20220128202544.png)

![20220128202550](https://picsheep.oss-cn-beijing.aliyuncs.com/pic/20220128202550.png)

最开始由硬件发送IRQ到Gateway

然后Gateway转发到PLIC core中

然后通知processor

processor进行claim，通过读写特定的register，并开始执行interrupt handler

PLIC core负责进行Claim Response，并清除interrupt signal，防止其他的core进行claim

然后直到processor结束中断，processor会重新写claim/complete register表示完成

![20220128203051](https://picsheep.oss-cn-beijing.aliyuncs.com/pic/20220128203051.png)

![20220128203102](https://picsheep.oss-cn-beijing.aliyuncs.com/pic/20220128203102.png)

可以看到，上面的优先级处理，转发中断请求的部分是由PLIC完成的

而下面的claim部分则是和processor共同完成

PLIC ID register用来识别中断是否claim，这样的话就不会有其他的core同时执行

然后processor通过读写register来标识claim/complete这个中断

从而实现中断的distribute以及thread-safe的处理