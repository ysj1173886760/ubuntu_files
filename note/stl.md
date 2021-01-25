::前面如果没有名称，则表示该变量或者函数是全局变量或者公共函数

new：指我们在C++里通常用到的运算符，比如A* a = new A;  对于new来说，有new和::new之分，前者位于std

operator new()：指对new的重载形式，它是一个函数，并不是运算符。对于operator new来说，分为全局重载和类重载，全局重载是`void* ::operator new(size_t size)`，在类中重载形式 `void* A::operator new(size_t size)`。

还要注意的是这里的`operator new()`完成的操作一般只是分配内存，事实上系统默认的全局`::operator new(size_t size)`也只是调用malloc分配内存，并且返回一个void*指针。而构造函数的调用(如果需要)是在new运算符中完成的

[详见](https://www.cnblogs.com/raichen/p/5808766.html)