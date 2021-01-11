# c++新特性

## nullptr

空指针 防止隐式的类型转换 NULL 和 0

## constexpr

常量表达式 constexpr函数可以使用递归

```cpp
constexpr int fibonacci(const int n) {
    return n == 1 || n == 2 ? 1 : fibonacci(n-1)+fibonacci(n-2);
}
```

并且从c++14开始，constexpr函数内可以使用局部变量，循环，分支等简单语句

## if/switch 语句初始化

c++17让我们可以在if或switch时对变量进行初始化

```cpp
//将临时变量放到if语句中
if (初始化临时变量; 逻辑判断表达式) {
    代码块
}
```

## 初始化列表

c++11将初始化列表的概念绑定到了类型上，并称之为`std::initializer_list`，他允许构造函数或其他函数像参数一样使用初始化列表

```cpp
std::vector<int> vec;
MagicFoo(std::initializer_list<int> list) {
    for (std::initializer_list<int>::iterator it = list.begin();it != list.end(); ++it)
    vec.push_back(*it);
}
MagicFoo magicFoo = {1, 2, 3, 4, 5};
```

除了用于构造对象，还可以作为普通函数的形参

## 结构化绑定

c++11让我们可以用tuple构造元组，但是没有简单的方法可以直接拿到并定义元组中的元素，可以使用tie对元组进行拆包，但是需要清楚元组包含多少对象，类型等

c++17让我们可以这样做

```cpp
std::tuple<int, double, std::string> f() {
    return std::make_tuple(1, 2.3, "456");
}
auto [x, y, z] = f();
```

## 类型推导

auto可以让编译器进行类型推导

decltype是为了解决auto关键字只能对变量进行类型推导的缺陷而出现的

用法是`decltype(表达式)`

`is_same<T, U>`可以用来判断T和U这两个类型是否相等

由于auto不能推导函数返回值类型，所以c++11引入了尾返回类型

```cpp
template<typename T, typename U>
auto add(T x, U y) -> decltype(x+y) {
    return x + y;
}
```

从c++14开始可以直接让普通函数具备返回值推导，所以就可以使用auto对函数返回值进行推导

decltype(auto) 主要用于对转发函数或封装的返回类型进行推导，使我们无需显式的指定decltype的参数表达式

## if constexpr

c++17将constexpr关键字引入到if语句中，允许代码中声明常量表达式的判断条件

```cpp
template<typename T>
auto print_type_info(const T& t) {
    if constexpr (std::is_integral<T>::value) {
        return t + 1;
    } else {
        return t + 0.001;
    }
}
```

## 区间for迭代

```cpp
for (auto &x : vec) {
    //statement
}
```

加上引用就是可写的，不加就是只读的

## 模板

传统C++中，只要编译过程中遇到了被完整定义的模板，都会实例化，这就导致了重复实例化导致的编译时间的增加。

C++11引入外部模板，让我们可以通知编译器何时进行模板的实例化

```cpp
template class std::vector<bool>; // 强行实例化
extern template class std::vector<double>; // 不在该当前编译文件中实例化模板
```

类型别名模板，模板是用来产生类型的，传统C++中，typedef可以为类型定义一个新的名称，但是无法为模板定义，因为他不是类型

使用using可以达成功效，同时using也有和传统的typedef相同的效果

```cpp
typedef int (*process)(void *);
using NewProcess = int(*)(void *);
template<typename T>
using TrueDarkMagic = MagicType<std::vector<T>, std::string>;

int main() {
    TrueDarkMagic<bool> you;
}
```

既然我们有默认的函数参数，那么也可以有默认的模板参数

```cpp
template<typename T = int, typename U = int>
auto add(T x, U y) -> decltype(x+y) {
    return x+y;
}
```

这样不需要每次指定add的类型也可以使用

变长参数模板，结合python等语言中的不定长参数，并且类型是可以变化的

```cpp
template<typename... Ts> class Magic;
```

个数为0的模板参数也是被允许的`class Magic<> nothing;`

所以可以手动定义至少一个模板参数来避免这种情况

```cpp
template<typename Required, typename... Ts> class Magic;
```

可以使用`sizeof...(Ts)`来获得参数个数

对于解包参数来说，可以使用递归解包的方法

```cpp
//很巧妙的利用了重载
template<typename T0>
void printf1(T0 value) {
    std::cout << value << std::endl;
}
template<typename T, typename... Ts>
void printf1(T value, Ts... args) {
    std::cout << value << std::endl;
    printf1(args...);
}
int main() {
    printf1(1, 2, "123", 1.1);
    return 0;
}
```

还可以使用变参模板展开的方法，C++17中增加了变参模板展开的支持，我们可以利用if constexpr在编译时展开的特性，对变参模板进行处理

```cpp
template<typename T0, typename... T>
void printf2(T0 t0, T... t) {
    //代码会在编译时完成分支的判断并展开函数
    std::cout << t0 << std::endl;
    if constexpr (sizeof...(t) > 0) printf2(t...);
}
```

初始化列表展开的形式，恕我暂时还不能理解

```cpp
template<typename T, typename... Ts>
auto printf1(T value, Ts... args) {
    std::cout << value << std::endl;
    (void) std::initializer_list<T> {([&args] {
        std::cout << args << std::endl;
    }(), value)...};
}
//在{}中的参数作为初始化列表，(args)...被展开为(args1), (args2), ...
//利用逗号表达式，按顺序执行并返回最后一个的结果
//所以这里是通过逗号表达式先调用了lambda函数，再把value赋给初始化列表
```

对于变参模板展开的方式可以看看这个[文章](https://cloud.tencent.com/developer/article/1347059)

C++17将变长参数的特性带给了表达式

```cpp
template<typename ... T>
auto sum(T ... t) {
    return (t + ...);
}
int main() {
    std::cout << sum(1, 2, 3, 4, 5, 6, 7, 8, 9, 10) << std::endl;
}
```

非类型模板参数，我们可以使用字面量作为模板的参数进行传递，并且C++17允许我们使用auto来让编译器辅助进行类型推导

```cpp
template <typename T, int BufSize>
class buffer_t {
public:
    T& alloc();
    void free(T& item);
private:
    T data[BufSize];
}
buffer_t<int, 100> buf; // 100 作为模板参数

template <auto value> void foo() {
    std::cout << value << std::endl;
    return;
}
//编译器将会推导传入的类型
```

## 面向对象

C++11引入了委托构造的概念，可以在同一个类中一个构造函数调用另一个构造函数，和JAVA的类似，比如有参构造函数取调用无参构造函数

继承构造，使用using进行继承构造,即在子类直接将父类的构造函数继承过来，有待深究

```cpp
class Subclass :public Base {
public:
    using Base::Base;   //继承构造
};
```

考虑这样两种情况，子类意外的重写了父类的虚函数，或当父类的虚函数删除后，子类的函数变成了普通的函数，为了防止这种情况，C++11引入了override和final这两个关键字

重写虚函数时，引入override关键字是显式的告诉编译器进行重写，编译器将检查是否存在这样的虚函数，否则将无法通过编译

这里要注意override（重写）和overload（重载）的区别

```cpp
struct Base {
    virtual void foo(int);
};
struct SubClass: Base {
    virtual void foo(int) override; //合法
    virtual void foo(float) override; //非法，因为没有找到这个虚函数
};
```

final关键字是防止类被继续继承以及防止虚函数被继续重写

```cpp
struct SubClass1 final: Base {};    //子类不能再继承SubClass1类
virtual void foo() final;   //子类不能再重写虚函数
```

显式的声明和禁用默认函数，在effective c++中，有提到如果不希望类被拷贝，就要把拷贝构造函数和赋值运算符声明为private。

并且，编译器产生的默认构造函数和用户定义的构造函数无法同时存在，若用户定义了任何构造函数，编译器将不再生成默认构造函数。

```cpp
class Magic {
public:
    Magic() = default;  // 显式声明使用编译器生成的默认构造函数
    Magic& operator=(const Magic&) = delete; // 显式拒绝编译器生成构造函数
    Magic(int magic_number);
}
```

传统C++中，枚举类型会被视为整数，这会让两种完全不同的枚举类型可以进行直接的比较，**甚至同一个命名空间中的不同枚举类型的枚举值名字不能相同**

```cpp
enum class new_enum :unsigned int {
    value1,
    value2,
    value3 = 100,
    value4 = 100
};
```

C++11引入了枚举类，他不会被隐式的转换为整数，如果相同的枚举值之间指定的值相同，是可以进行比较的，枚举类型后面使用了冒号加关键字来指定枚举值的类型，默认为int

可以重载 `<<` 来输出枚举值

```cpp
//有待研究.jpg
template<typename T>
std::ostream& operator<<(typename std::enable_if<std::is_enum<T>::value, std::ostream>::type& stream, const T& e) {
    return stream << static_cast<typename std::underlying_type<T>::type>(e);
}
```

[std::enable_if和SFINAE](https://www.jianshu.com/p/a961c35910d2)

## lambda表达式

```cpp
[捕获列表](参数列表) mutable(可选) 异常属性 -> 返回类型 {
    // 函数体
}
```

lambda表达式内部函数体默认情况下是不能够使用函数体外部的变量的，这时捕获列表可以起到传递外部数据的作用

1. 值捕获 值捕获的前提是变量可以拷贝，被捕获的变量在lambda表达式被创建时拷贝，而不是调用时再拷贝

2. 引用捕获 与引用传参类似，值会发生变化

3. 隐式捕获 手动写捕获列表较为复杂，可以在捕获列表中写一个 `&` 或者 `=` 来向编译器声明采用引用捕获还是值捕获

4. 表达式捕获 上面的捕获都是已经在外层作用域声明的变量，因为这些捕获方式均为左值捕获，C++14允许捕获的成员用任意的表达式进行初始化，这就允许了右值捕获

泛型lambda：auto关键字不能写在参数表里是因为会与模板产生冲突，C++14开始，lambda函数的参数可以使用auto来产生意义上的泛型

```cpp
auto add = [](auto x, auto y) {
    return x + y;
};
add(1, 2);
add(1.1, 2.2);
```

## 函数对象包装器

Lambda 表达式的本质是一个和函数对象类型相似的类类型（称为闭包类型）的对象（称为闭包对象），当 Lambda 表达式的捕获列表为空时，闭包对象还能够转换为函数指针值进行传递

调用时，可以将lambda表达式通过参数传递进行调用，或者是直接调用lambda表达式，在C++11中统一了这些概念，将能被调用的对象的类型，统一成为可调用类型

C++11 `std::function`是一种函数封装，它的实例可以对任何可以调用的目标实体进行储存，复制和调用操作，是函数的容器

```cpp
int foo(int para) {
    return para;
}

int main() {
    // std::function 包装了一个返回值为 int, 参数为 int 的函数
    std::function<int(int)> func = foo;
    int important = 10;
        std::function<int(int)> func2 = [&](int value) -> int {
        return 1+value+important;
    };
    std::cout << func(10) << std::endl;
    std::cout << func2(10) << std::endl;
    return 0;
}
```

`std::bind`用于绑定函数调用的参数的，因为我们可能不能一次性获得函数的所有参数，通过这个函数，我们可以将部分参数提前绑定到函数身上成为一个新的对象

```cpp
int foo(int a, int b, int c) {
    ;
}
int main() {
    auto bindFoo = std::bind(foo, std::placeholders::_1, 1, 2);
    //可能我们不太熟悉他的返回值的类型，使用auto帮助我们
    bindFoo(1);
    return 0;
}
```

左值 (lvalue, left value)，顾名思义就是赋值符号左边的值。准确来说，左值是表达式（不一定是赋值表达式）后依然存在的持久对象。

右值 (rvalue, right value)，右边的值，是指表达式结束后就不再存在的临时对象。

纯右值 (prvalue, pure rvalue)，纯粹的右值，要么是纯粹的字面量，例如 10, true；要么是求值结果相当于字面量或匿名临时对象，例如 1+2。非引用返回的临时变量、运算表达式产生的临时变量、原始字面量、Lambda 表达式都属于纯右值

需要注意字符串字面量在类中是右值，在普通函数中是左值

```cpp
class Foo {
    const char*&& right = "this is a rvalue"; // 此处字符串字面量为右值
public:
    void bar() {
        right = "still rvalue"; // 此处字符串字面量为右值
    }
};
int main() {
    const char* const &left = "this is an lvalue"; // 此处字符串字面量为左值
    // 可能是由于字符串字面量储存在常量区中
}
```

将亡值 (xvalue, expiring value)，是 C++11 为了引入右值引用而提出的概念（因此在传统 C++中，纯右值和右值是同一个概念），也就是即将被销毁、却能够被移动的值。

```cpp
std::vector<int> foo() {
    std::vector<int> temp = {1, 2, 3, 4};
    return temp;
}
std::vector<int> v = foo();
```

在这样的代码中，就传统的理解而言，函数 foo 的返回值 temp 在内部创建然后被赋值给 v，然而 v获得这个对象时，会将整个 temp 拷贝一份，然后把 temp 销毁，如果这个 temp 非常大，这将造成大量额外的开销（这也就是传统 C++ 一直被诟病的问题）。在最后一行中，v 是左值、foo() 返回的值就是右值（也是纯右值）。但是，v 可以被别的变量捕获到，而 foo() 产生的那个返回值作为一个临时值，一旦被 v 复制后，将立即被销毁，无法获取、也不能修改。而将亡值就定义了这样一种行为：临时的值能够被识别、同时又能够被移动

在 C++11 之后，编译器为我们做了一些工作，此处的左值 temp 会被进行此隐式右值转换，等价于static_cast<std::vector<int> &&>(temp)，进而此处的 v 会将 foo 局部返回的值进行移动。也就是后面我们将会提到的移动语义。

结合这篇文章[文章](https://www.jianshu.com/p/d19fc8447eaa)理解一下右值引用

看完上面的文章后再看这个问题

第一个问题，为什么不允许非常量引用绑定到非左值？这是因为这种做法存在逻辑错误：

```cpp
void increase(int & v) {
    v++;
}
void foo() {
    double s = 1;
    increase(s);
}
```

由于 int& 不能引用 double 类型的参数，因此必须产生一个临时值来保存 s 的值，从而当increase() 修改这个临时值时，从而调用完成后 s 本身并没有被修改。

第二个问题，为什么常量引用允许绑定到非左值？原因很简单，因为 Fortran 需要。(这个我也不是太清楚)

