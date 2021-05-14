这里用来存一些百科上的个人认为比较好的解释

概念对于理解一个东西来说是很重要的，因为他会高度概括这个东西所适用的场景，因此理解了概念我们才能将这个东西直觉化，也就可以将这个东西熟练运动到别的地方了。

最后有的地方会加上我自己对这个东西的理解的一些注释

###### Handle

In computer programming, a handle is an abstract reference to a resource that is used when application software references blocks of memory or objects that are managed by another system like a database or an operating system.

A resource handle can be an opaque identifier, in which case it is often an integer number (often an array index in an array or "table" that is used to manage that type of resource), or it can be a pointer that allows access to further information. Common resource handles include file descriptors, network sockets, database connections, process identifiers (PIDs), and job IDs. PIDs and job IDs are explicitly visible integers; while file descriptors and sockets (which are often implemented as a form of file descriptor) are represented as integers, they are typically considered opaque. In traditional implementations, file descriptors are indices into a (per-process) file descriptor table, thence a (system-wide) file table.

While a pointer contains the address of the item to which it refers, a handle is an abstraction of a reference which is managed externally; its opacity allows the referent to be relocated in memory by the system without invalidating the handle, which is impossible with pointers. The extra layer of indirection also increases the control that the managing system has over the operations performed on the referent. Typically the handle is an index or a pointer into a global array of tombstones.

###### exec

exec函数族的作用是根据指定的文件名找到可执行文件，并用它来取代调用进程的内容，换句话说，就是在调用进程内部执行一个可执行文件。这里的可执行文件既可以是二进制文件，也可以是任何Linux下可执行的脚本文件。

与一般情况不同，exec函数族的函数执行成功后不会返回，因为调用进程的实体，包括代码段，数据段和堆栈等都已经被新的内容取代，只留下进程ID等一些表面上的信息仍保持原样，颇有些神似"三十六计"中的"金蝉脱壳"。看上去还是旧的躯壳，却已经注入了新的灵魂。只有调用失败了，它们才会返回一个-1，从原程序的调用点接着往下执行。

我们应该明白了，Linux下是如何执行新程序的，每当有进程认为自己不能为系统和用户做出任何贡献了，他就可以发挥最后一点余热，调用任何一个exec，让自己以新的面貌重生；或者，更普遍的情况是，如果一个进程想执行另一个程序，它就可以fork出一个新进程，然后调用任何一个exec，这样看起来就好像通过执行应用程序而产生了一个新进程一样。

事实上第二种情况被应用得如此普遍，以至于Linux专门为其作了优化，我们已经知道，fork会将调用进程的所有内容原封不动的拷贝到新产生的子进程中去，这些拷贝的动作很消耗时间，而如果fork完之后我们马上就调用exec，这些辛辛苦苦拷贝来的东西又会被立刻抹掉，这看起来非常不划算，于是人们设计了一种"写时拷贝（copy-on-write）"技术，使得fork结束后并不立刻复制父进程的内容，而是到了真正实用的时候才复制，这样如果下一条语句是exec，它就不会白白作无用功了，也就提高了效率。

注：写时复制就是在fork以后，我们并没有直接的复制地址空间，而是相当于将两个指针指向了同一个地址空间，然后在子进程的地址空间改变的时候，我们才将改变的那部分进行复制并修改，CSAPP中对此有详细的图文解释，推荐去看