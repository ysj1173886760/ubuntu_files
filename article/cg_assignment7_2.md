### 现代计算机图形学 作业7 解析（2）

这次我们会涉及到有关提高的内容，即多线程和微表面模型

所以这里也会提到有关C++语言方面的一些东西

我相信大家在做这个作业的时候肯定已经被效率问题折磨了很多次了，因为生成一张图片要很久，所以要花很久才能看到自己的代码写的到底是不是正确的

首先是代码框架的一个问题，论坛中有人提出来，大量的时间被耗费在构造随机数生成器了，所以在开始之前，可以先把随机数生成的函数中，把随机数生成器给改成static的，这样可以让速度快很多

然后我们来说多线程

在写代码的过程中我们可以发现，每个像素的光线与光线之间是没有什么联系的，也就是说他们各自都是独立的，这就启发了我们可以使用多线程来进行path tracing

同时这个代码引入多线程比较简单的一点就是他每个过程是独立的，所以我们不需要一些锁来防止多个线程同时访问数据。所以引入多线程也只是简单的把函数拿出来丢到每个线程中就行

那么为了学习多线程，我们来到cppreference，搜索`thread`，大家可以简单的看一看线程的介绍，然后在构造函数中就可以看到具体创建线程的方法，我们传入函数，然后传入对应的参数即可

这里注意，看一下Notes中有写到，`The arguments to the thread function are moved or copied by value. If a reference argument needs to be passed to the thread function, it has to be wrapped (e.g., with std::ref or std::cref`，也就是说，如果要传入引用参数需要我们用`ref`或者`cref`来包装

然后我们可以自己划分缓冲区，然后对于每个线程传入他的对应区域所对应的参数，我们就可以实行多线程了。

```cpp
std::vector<std::thread> workers;
for (int i = 0; i < numThreads; i++) {
    int yStart = i * lineEveryThread;
    int yEnd = std::min(yStart + lineEveryThread, scene.height);
    std::cout << "Thread id: " << i << " start " << yStart << " end " << yEnd << std::endl;
    workers.push_back(std::thread(RenderThread, std::ref(scene), std::ref(framebuffer), yStart, yEnd, spp));
}
```

我这个是按照行划分，然后这里执行完了我们需要进行等待，因为我们的主线程在构造完这些线程后就会继续执行，但是我们需要等所有线程都渲染完才能进行下一步，所以这里要进行线程的等待

```cpp
for (auto &x : workers)
    x.join()
```

`join`就是等待线程，更加具体的描述可以在cppreference中找到

那么主体的多线程的框架结束以后，我们可以想到，对于更新的进度条，实际上是多个线程在共享的同一个数据，所以大家如果还是想继续看到进度条的话，就需要用一个互斥锁来帮我们保护数据

在cppreference中，thread support library里可以看到mutual exclusion中有`std::mutex`，这个就是互斥锁，简单的介绍就是`lock`和`unlock`，在上锁的区域中更新数据，可以保证其他线程不会干扰

```cpp
mutex.lock();
renderProgress++;
UpdateProgress(renderProgress / (float)scene.height);
mutex.unlock();
```

理解还是比较容易的，这个线程lock了以后，其他线程就必须等待unlock之后才能继续lock

还有一个小细节，就是在thread库中，有一个函数叫`std::thread::hardware_concurrency()`，这个函数可以返回你的硬件所支持的并行数量，我们可以以这个函数作为参考来确定我们的线程数。更多的细节也请参考cppreference

看起来这个文章篇幅也差不多了，我们就到这里，下一节就是微表面模型