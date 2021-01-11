# c++ auto

[参考文章](https://blog.csdn.net/ywcpig/article/details/52739314)

[参考文章](https://blog.csdn.net/yiti8689/article/details/108277295)

需要使用本地拷贝时，用auto，需要保证拷贝构造函数等必须存在，即拷贝一份新的变量

使用auto&&时，总会使用引用，但是如果使用的对象是一个临时对象，比如返回值是按值传递的，他就会和局部变量的表现相同，不会改变原对象

使用auto&时保证你总是在使用一个对其他变量的引用，由于不能对临时对象进行引用，所以可以使用auto&&代替

const auto &和auto &&是相似的，但是他不能更改，是只读的

但是auto会忽略掉顶层的const，所以如果需要让auto是const，需要显式的指定，const auto

但是auto&&不会忽略，他不保证对象总是可以改变的

auto&&是右值引用（左右值通用） auto&是左值引用

需要改变时使用auto&

只读时使用const auto &

对拷贝值进行操作用auto
