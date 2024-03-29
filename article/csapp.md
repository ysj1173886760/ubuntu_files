### CSAPP总结

昨天，5月9号终于是赶着CMU的日程把csapp的东西结束了

其实书已经读完很久了，但是最后一个proxylab一直拖着没做

最后的8个lab都已经上传到我的github了

现在是最后的总结性的一篇文章，看着目录写一写，并不是说总结csapp中的知识点，而是谈谈读完书后的体会

我会针对每一章，以及每一个lab都说一下完成后的感受，以及学习时的要点，所以如果没读过这本书的同学也可以进行参阅

第一章是计算机系统漫游，就是一般性的给我们一个overview，并没有涉及到很多的细节，所以看的时候不必太在意技术上的细节，我们只需要根据这一章有一个大概的轮廓，对计算机的轮廓就好

这时候可以去做一下lab0，也就是最开始的一个实验，目的是让我们有一个带入的过程，顺便熟悉和复习C语言知识，相当简单的一个lab，就是实现一个队列，快速过掉就可以

然后我们进入到了第一部分，程序结构和执行

第二章就是信息的表示和处理

这一章主要的点就是掌握好数据在内存中的表示，主要就是补码`(two's complement)`和浮点数在内存中是怎么储存的，最开始的时候，读者可能会被卡到这里一段时间，因为这块的内容不是那么有趣，但是必要的课后题还是要做一做的，读的时候不必太过于纠结细节，快速读完然后做lab的时候再回忆细节即可

读完第二章就可以做lab1，是用位运算来实现一些操作，在这里可以检验我们在第二张学习的成果，尤其是后面几个题，对浮点数的操作，如果有不懂的就翻回去多看看，我做完这个lab的体会就是对数据的理解比干读书要深刻的多，所以做这个lab是理解这一章的关键

然后就是第三章，也是篇幅较长的一章，但是内容相对较为有趣，这一章主要讲汇编，x86_64的汇编，同时也不只是专注于语法，还有很多细节的地方值得我们去考虑，前面几节就是语法，专心读完做课后题就可以有一定的理解，从第7节开始会涉及到一些程序的结构的东西，这一块要多看并且看清楚，同时可以根据自己的理解尝试去生成一些汇编，并阅读这些代码，用于检验自己的理解，后面还有讲到缓冲区溢出攻击等内容，也是需要读者对此有一个全局的视野，这样才能在做后面lab的时候较为轻松

读完这一章后就可以做两个lab，分别是bomb lab和attack lab，bomb lab主要考验我们读汇编的能力，会给出较为全面的程序，并且需要我们利用`objdump`和`gdb`来进行调试，这里多去网上搜搜资料，看看这些工具的用法，做bomb lab还是没什么问题的。对于attack lab来说，确实是有一定难度，并且需要我们对汇编较为熟悉同时还能编写一定的汇编代码，也考察到了我们对程序运行时栈结构的理解，以及后面的return-oriented attack，需要我们用gadget来进行攻击。完成这两个实验可以说是对汇编有一定程度的掌握了，同时也对程序运行时的结构有了一个初步的理解

然后是第四章处理器体系结构，这一章是偏向硬件一些，我个人来说读这章是比较快的，个人觉得没必要纠结指令的细节，所以对于HDL方面的细节不必过于深究，跟着这一章的东西过一遍，理解了CPU的流水线设计，指令执行的几个步骤，以及一些常识性的东西，比如分支预测，竞争冒险，然后做一做课后习题就好

第五章，优化程序性能，说实话目前来说我们编写程序并不是很需要这方面的工作，优化交给编译器就好，我们要先写好程序，再一步一步对其优化，但是这种思想我们还是要知道的，比如我们针对流水线的优化，使用循环展开以及多个变量进行优化，对我们写的程序有一个性能上的理解就好，也就是对在复杂度之外的性能要有一定的估计

第六章是存储器层次结构，这一章相对来说较为重要，因为涉及到后面的cache lab，以及后面几章的学习，所以要理解透彻后再进行下一步。这一章要清楚存储器的局部性，以及现在我们对这个性质的依赖，理解缓存这个概念是非常重要的。后面几个小节是将高速缓存的原理以及编写缓存友好的代码，这里看完就可以去做cache lab了，正好对应第六章的3，4两个小结，根据自己对书中内容的理解写一个小的cache，以及优化代码使cache命中尽可能的多

然后是第二部分，在系统上运行程序

来到第七章链接，这一章最开始我读的时候看了有好多遍，查阅了一定的资料才明白具体的过程，因为前后联系较为紧密，所以建议看的时候要对整个章节有一定的把握，虽然篇幅内容不多，但是有些部分确实可以实打实的解决我们平常的问题，正如在书中说的那样，如果无法理解链接的原理，那么编写程序的时候一定会对这方面的错误头痛不已，正因为这章的复杂性，也有了我之前的对第七章链接的一个总结。

第八章是异常控制流，到这里我们涉及到C语言编程的部分就开始变多了，这一章较为重要，主要是进程的控制，以及信号，前面还有对操作系统异常的一些解释，概念上的东西较多，但是绝对值得仔细阅读，读完之后也会对linux操作系统有一个更深入的理解。

读完这章后就是一个shell lab，叫我们完成一个shell，实现了基本的内置命令，主要是考察我们对进程的控制，以及对信号的掌握，多去翻阅书中的讲解，可以更好的完成这个lab

第九章也是重中之重，虚拟内存，这里综合了前面讲的一些东西，可以与前面的链接，存储器等结合起来理解，帮助我们形成一个完整的知识体系。无论是前面对于虚拟内存工作原理的讲解，内存映射，还是后面的动态内存分配，都是值得我们仔细推敲的一点，当然了，我们还需要具体的去学习操作系统，以得到更加深入的理解

读完这章可以做malloc lab，叫我们实现一个自己的malloc，这里大家可以先实现书中讲到的最简单的版本，然后再去尝试后面的显式的空闲链表，以及分离的空闲链表等做法

可以发现我在这一部分说的东西很少，原因是这三张都是满满的知识，不需要我来进行特定的总结，更希望大家仔细阅读这三章，以为将来的操作系统的学习打下坚实的基础。

最后一部分是程序间的交互和通信

第十章就是系统级IO，这里其实讲的并不是很细，我们也需要在将来的操作系统的学习过程中进行更加深入的研究，这里只是一个抛砖引玉，当然我们还可以去阅读APUE对这方面的讲解

第十一章是网络编程，主要是socket套接字编程，只是给我们了一个对于网络的简单的概述，这里也是我们要去自己深入学习的，比如去看《计算机网络：自顶向下》，这里同样是一个抛砖引玉

最后一章是并发编程，给出了几个模型，基于线程，基于进程，或者基于IO多路复用。主要讲了多线程编程的各种方面，比如信号量，死锁等，但是还是需要我们在学习操作系统的过程中进行一个更深入的理解。

读完后就可以做最后一个proxylab，相对来说，我们只需要按照第十一章的web服务器，就可以编写一个简单的proxy，然后再用十二章学到的，比如预线程化来优化我们的proxy，最后再引入读者-写者模型，可以帮助我们完成最后的cache部分

对我个人来说，读完这本书后最大的体会不是我学会了多少东西，这本书更像是为我打开计算机世界的大门，读完后直接的感受就是，自己有好多东西要学，很多书想看，读完一本大厚书，反倒感觉自己知道的更少了，因为视野变宽了，很感谢这本书为我带来的思想上的提升，以及视野的提升。自己现在大二下也快过了一半了，才有幸读完这本书，不禁感慨，曾经以为自己已经是走在前列的人，但是现在看来不过是坐井观天罢了，但是为时不晚。阅读过程中自己也总结出来不少方法论，对于自己将来的学习路途，我有着充分的信心。

还是很感谢书的两位作者，能为我们带来如此具有启发性的书籍，可以说是把我从井里直接拽了出来。如果可以的话，我也希望自己将来能够分享自己对于知识的理解，为这个行业，为所有计算机的爱好者，贡献出自己的力量

最后，如果看到这里的同学，如果你也是计算机爱好者，我由衷的推荐这本书，值得你静下心来慢慢阅读。