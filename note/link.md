### 链接

这篇文章是在我看完CSAPP第七章后对自己的理解做的一个总结， 也算是对书中第七章的内容做一个总结吧

适合已经看完第七章但是还没有把链接搞得很清楚的人，或者可以把这篇文章看做是对程序链接的一个概括，希望我的总结可以为你们带来启发

由于本人有点懒，所以并没有打算专门的画图来讲解，如果有同学没有看懂欢迎找我来讨论

这里我们用最简单的例子来进入今天的主题，假设你要两个c文件，`main.c` 和 `sum.c`，其中main里调用sum中的一个函数，我们使用这样的方式来编译这个程序

`linux> gcc -o prog main.c sum.c`

这样你就会得到一个名为prog的可执行文件，那么在这个过程中究竟发生了什么呢。

首先使用C处理器`cpp`(别和c++弄混，这里是c preprocess)，将main.c翻译成了一个ASCII码的中间文件main.i

然后运行c编译器`ccl`，将main.i翻译成一个汇编语言文件main.s

然后再运行汇编器`as`，将main.s汇编成`可重定位目标文件(re-locatable object file)`main.o

以同样的方法，我们可以得到sum.o, 最后，我们运行链接器程序`ld`，将main.o和sum.o，以及一些系统目标文件组合起来，得到了一个可执行目标文件prog

我们在调用的时候，使用一个叫做加载器的函数，他会将可执行文件中的代码和数据复制到内存，以进行程序的执行

有关上面提到的cpp, ccl, as, 他们最终的结果就是帮助我们生成可重定位目标文件，这里我们的核心是链接，如果有感兴趣的读者想了解上面的内容可以看编译原理或者汇编

回到我们的正题，现在我们已经获得了可重定位目标文件，里面都是字节块，储存着程序的代码或者程序的数据，都是机器的指令，但是目前来说，他还不是一个可以执行的文件

为什么？首先考虑同一个模块中的调用，（同一个模块指的是同一个文件，比如都是main.o），局部变量储存在栈中，这里我们不需要考虑。而全局变量储存在内存中，如果你想要引用这些全局变量，就需要获得他们的地址，因为字节码中不会让你用变量名称来引用全局变量。但是随之而来的问题是，编译器，汇编器，他们并不会帮你指定变量到底储存在内存中的那个位置，这就需要用到链接器。然后我们再考虑跨模块的调用，比如在main里调用sum中的函数，或者引用其他地方的全局变量，实际上就是对符号的引用，在这个过程中，我们是无法在当前模块中找到对这些符号引用所对应的符号定义的，所以这就是链接器的另一个任务，帮助每一个符号引用找到对应的符号定义。

链接器的主要任务就是符号解析和重定位，再结合我们刚才得到的文件--可重定位目标文件，我们就知道他名字的缘由了。这种文件虽然有着程序代码和数据，但是对于自己具体分配到内存的哪里还不知道，所以这里就需要链接器来帮助他重定位，找到自己的位置。

有关目标文件的节的概念这里就不具体介绍，可以去看书里的讲解

下面来到链接器的部分，首先是符号解析，即对每一个符号引用找到对应的符号定义，这里我们不止可以在可重定位目标文件中找，还可以在静态库中进行查找。对于静态库的概念，可以简单的理解成常驻的可重定位目标文件，当我们引用到静态库中的模块时，链接器会复制里面的模块给我们并把这些模块和我们的文件一起链接。

对于静态库解析引用，是有一个算法的，也相对较为简单，这里不再赘述，CSAPP中文版477页可以看到。

符号解析的算法简单来说就是，遇到一个未定义的符号，我们把他记录下来，之后遇到该符号的定义的时候就把符号引用和符号定义关联起来就可以。

符号解析完毕后，我们就需要进行重定位了，就像之前说的，汇编器在生成目标模块时，他是不知道程序的代码和数据到底存放到哪里的，所以遇到对最终位置未知的引用，他会生成一个重定位条目，这会告诉链接器在链接过程中如何修改这个引用，同样涉及到一定的细节上的算法。

链接器首先将运行时内存地址赋给目标文件，这样所有的函数以及全局变量就有了自己的运行时地址，然后对这些符号的引用进行修改。即修改代码中对这些符号的引用，让他们指向正确的运行时地址。

最终我们就得到了由静态链接生成的可执行文件，这时，我们可以运行这个可执行文件，加载器就会复制代码和数据到内存中，然后跳转到程序的入口点来运行程序，并不是main函数，而是`_start`以及系统启动函数`__libc_start_main`，具体初始化的细节也不在这里赘述。

上面说完了静态库，还有动态库要说，准确来说，是动态链接共享库，在系统中以`.so`来命名，即库是叫做共享库，我们是动态链接这个库

我们在加载并运行时进行共享库的链接，这就涉及到一个问题，我们如何确定库中代码的地址呢？运行的时候我们已经不能再去修改可执行文件的代码段了，也就是对于那些函数和变量的引用的修改已经结束了，况且，我们不能保证每次动态库代码的地址是固定的，也就无法保证每次运行程序，引用的地址都是固定的。

共享库使用`位置无关代码PIC(Position-Independent Code)`来实现我们的目标。

那么怎么实现这项技术呢？首先我们说PIC数据引用，我们基于一个有意思的性质：无论我们在内存中何处加载一个目标模块，数据段和代码段之间的距离总是固定的。所以代码中的任何指令和数据的距离是一个运行时常量，与他们的绝对内存地址无关。

基于此项事实，我们在数据段（存放数据的地方）定义了一个表`全局偏移量表(Global Offset Table`，在这里储存着我们引用的全局数据目标，GOT为每一个这样的条目生成了一个重定位记录，在加载时，动态链接器会重定位GOT中的每个条目，使得他包含正确的地址。

即我们通过引用GOT中的数据间接引用到了我们希望引用的全局变量，因为我们可以确定代码和GOT的距离，同时GOT中的内容会被动态链接器重定位，所以满足了我们在不修改代码的前提下引用到了正确的内存地址。

然后我们要说PIC函数调用，系统使用延迟绑定的技术，以及`PLT(Procedure Linkage Table)过程链接表`和GOT的相互配合来实现

具体的算法去书中看更清楚一些，这里只讲一下简单的思想，GOT中储存的地址不仅仅可以是数据地址，还可以是函数地址等。我们在调用函数的时候，会进入PLT表中对应的条目，如果我们是第一次调用，那么此时GOT中对应的条目的地址还不是真实地址（也有用处），PLT表会帮助我们调用动态链接器并对该函数进行动态链接，并将函数的地址存放到GOT中对应条目的位置。我们之后调用的时候，就可以直接通过PLT来到GOT中找到函数地址的位置，并调用该函数。

到此，链接的部分就结束了，库打桩这里也就不提了，本文只是给出链接的简单的一个概述，还是推荐大家找相关文献读一读，如有错误请不吝赐教。