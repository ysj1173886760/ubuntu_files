## 条款01：视C++为一个语言联邦

C++主要的次语言总共只有四个(sublanguage)

C：C++仍然是以C为基础，区块，语句，预处理器，内置数据类型，数组，指针等

Object-Oriented C++：这部分就是C with Class，类，封装，继承，多态，虚函数等

Template C++：这是C++的泛型编程，template metaprogramming (TMP 模板元编程)

STL：template程序库，由容器，迭代器，算法以及函数对象所组成

每个次语言都有自己的规约，C++高效编程守则视状况而变化，取决于你使用C++的那一部分

## 条款02：尽量以const，enum，inline替换 #define

可以改成宁可以编译器替换预处理器

因为define不视为语言的一部分，define出来的记号名称不会进入记号表（symbol table），这就可能导致debug时候的问题

当使用const替换define时，有两种特殊情况需要讨论

第一种是常量指针，我们有必要将指针（而不只是指针所指之物）声明为const 比如在头文件内定义一个常量字符串

`const char* const str = "hello world"`

string对象通常比字符数组合适 所以上面的代码往往定义成这样更好些

`const std::string str("hello world")`

第二种是class专属常量，为了将常量的作用域限制于class内，我们需要把它变成class的一个成员，而为了确保此常量至多只有一份实体，我们需要让他成为一个static成员

`static const int Num = 5`

包含在头文件中的这段代码是一个声明式而非定义式，通常C++要求我们对所使用的东西提供一个定义式，但是如果他是class专属常量又是static且为整数类型（注意不是int型，而是int,char,bool等）则需要特殊处理，只要不取他们的地址，就可以直接使用它们而不需要提供定义式，如果我们坚持使用他们的地址，或者编译器（错误的）坚持要看到一个定义式，我们就必须提供定义式如下

`const int GamePlayer::Num;`

这个要放到实现文件而非头文件中，由于声明时已经获得了初值，所以在这里可以不再设置初值

旧式编译器或许不支持上述语法，他们不允许static成员在其声明式上获得初值，此外所谓的in-class初值设定，也只允许对整数常量进行，如果编译器不支持，那么我们可以将初值放到定义式：

```C++
static const double Factor;
const double Estimate::Factor = 1.35;
```

还有一个例外就是如果你在class编译期间需要一个class常量值，比如用在数组声明式中（编译器坚持必须在编译期间知道数组的大小）。如果你的编译器（错误的）不允许static整数型class常量完成in class的初值设定，你可以改用所谓的‘the enum hack’补偿做法。其理论基础是：一个属于枚举类型的数值可权充int被使用

```c++
enum {Num = 5};
int scores[Num];
```

enum hack行为比较类似define而不是const，enum类型不允许取地址，所以如果你不想让别人获得一个pointer或reference指向你的某个整数常量 enum可以帮助你实现（反外挂手段）

使用inline代替宏，考虑如下代码

```c++
#define CALL_WITH_MAX(a, b) f((a) > (b) ? (a) : (b)) 	//以a和b中较大值调用f
int a = 5, b = 0;
CALL_WITH_MAX(++a, b);					//a被加一次
CALL_WITH_MAX(++a, b + 10);			//a被加两次
```

使用inline可以获得宏带来的效率以及一般函数的所有可预料行为和类型安全性

对于单纯常量，最好以const或enum替换#define

对于形似函数的宏，最好改用inline函数替换#define

## 条款03：尽可能使用const

如果关键字const出现在星号左边，表示被指物是常量，如果出现在星号右边，表示指针自身是常量

如果被指物是常量，有的人会把const写在类型之前，有个人会把他写在类型之后，意义相同

```c++
void f1 (const Widget *pw);
void f2 (Widget const *pw);
```

都代表这参数是一个指向常对象的指针

STL迭代器的一个例子

```c++
std::vector<int> vec;
const std::vector<int>::iterator iter = vec.begin();		//相当于T* const
*iter = 10;																	//改变iter所指没问题
++iter;																		//iter不能改变
std::vector<int>::const_iteraotr cIter = vec.begin();	//相当于const T*
*cIter = 10;															//被指物是const 不能改变
++cIter;																	//可以改变
```

令函数返回一个常量值，往往可以降低因客户错误而造成的意外

考虑如下声明式`const Rational operator (const Rational &lhs, const Rational &rhs)`

为什么要返回常对象？否则用户可能实现这样的暴行 (a * b) = c;

将const实施与成员函数的目的，是为了确认该成员函数可作用于const对象身上，这一类成员函数之所以重要，第一，它们使class接口比较容易被理解——得知那个函数可以改动对象内容而那个函数不行，第二，它们使操作const对象成为可能，这对高效编写代码是个关键（改善c++程序效率的根本办法是以pass by reference to const的方式传递对象，而这项的前提是我们由const成员函数来处理取得的const对象）

C++的一个重要特性：两个成员函数如果只是常量性不同，是可以被重载的

成员函数如果是const意味什么？

两个流行概念，bitwise constness和logical constness

对于bitwise constness，只要不改变对象内的任意一个bit，就可以说是const，但是一个更改了指针所指物的成员函数虽然不能算是const，但是如果只有指针属于该对象，那么称此函数为bitwise const不会引发编译错误

对于logical constness，一个函数可以修改它所处理的对象内的某些bits，但只有在客户端侦测不出的情况下才得以如此（即逻辑常函数，用户不会涉及到即可，为了高效性我们还是可以部分改变的）

在logical constness的实现中，如果要在const成员函数来修改值怎么办？ 使用C++的一个与const相关的摆动场：mutable（可变的）mutable可以释放掉non-static成员变量的bitwise constness约束

假设我们在实现过程中需要执行边界检查，日记访问等操作

```c++
class TextBlock {
public:
	const char& operator[] (std::size_t position) const {
		... //边界检查
		... //数据访问等
		return text[position];
	}
	char& operator[] (std::size_t position)  {
		... //边界检查
		... //数据访问等
		return text[position];
	}
}
```

如果每个函数我们都要实现类似这样的代码，肯定是很麻烦的

所以我们应该实现operator[]的机能一次并使用它两次，也就是说，我们应该让一个调用另一个，这将促使我们将常量性转除

上面的例子中，const函数做了non-const函数所做的一切，唯一的不同是返回类型加了一个const修饰

我们令non-const operator调用其const兄弟是一个避免代码重复的安全做法

``` c++
const char &operator(std::size_t position) const {
    return text[position];
}
char &operator(std::size_t position) {
    return const_cast<char&>(static_cast<const TextBlock&>(*this)[position]);
}
```

解释一下上面的代码，首先将*this将其原始类型TextBlock& 转换成了const TextBlock&，为其添加上const，然后在接下来调用的时候使用的就是const版本的函数，再从其返回值中移除const

有关static_cast 和 const_cast 之后会提到

还有一点，令const版本调用non-const版本是我们不应该做的事，const成员函数承诺绝不改变其对象的逻辑状态（logical state），non-const成员函数没有这样的承诺，如果你在const内部调用了non-const成员函数，就违背了承诺，这样就承担了在其过程中改变对象的风险。

将某些东西声明为const可帮助编译器侦测出错误用法。const可被施加于任何作用域内的任何对象，函数参数，函数返回类型，成员函数本体。

编译器强制实施bitwise constness，但你编写程序的时应该使用“概念上的常量性”（conceptual constness）

当const和non-const成员函数有着实质等价的实现时，令non-const版本调用const版本可避免代码重复