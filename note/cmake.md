 [参考文章](https://blog.csdn.net/whahu1989/article/details/82078563#t0)

# CMake简介

![image-20201101151634461](/home/sheep/.config/Typora/typora-user-images/image-20201101151634461.png)

# 环境配置

![image-20201101151713430](/home/sheep/.config/Typora/typora-user-images/image-20201101151713430.png)

# 样例

首先编写一段代码，什么都行，比如helloworld

保存为test.cpp

然后在test.cpp相同的目录下编写CMakeLists.txt

![image-20201101152209684](/home/sheep/.config/Typora/typora-user-images/image-20201101152209684.png)

其中第一行表示的是cmake版本的最低要求

第二行表示的是本工程的信息，也就是工程的名字

第三行表示的是最终生成的elf文件的名字叫main 使用的源文件是test.cpp

（elf文件是二进制可执行文件的一种形式）

保存好后在当前目录下输入`cmake .`注意中间有空格

最后会显示![image-20201101153056018](/home/sheep/.config/Typora/typora-user-images/image-20201101153056018.png)

可以通过ls查看当前目录下的文件，可以看到成功生成了makefile，还有一些cmake自动生成的文件

在终端下输入`make`

![image-20201101153302445](/home/sheep/.config/Typora/typora-user-images/image-20201101153302445.png)

会显示make成功，可以在ls下查看到我们生成的elf文件

输入`./main`运行文件![image-20201101153435745](/home/sheep/.config/Typora/typora-user-images/image-20201101153435745.png)

运行成功

​	PS：如果想重新生成main，输入make clean就可以删除这个elf文件

# 同一目录下多个源文件

在之前的目录下添加两个文件func.cpp和func.h

文件结构如下

![image-20201101153931136](/home/sheep/.config/Typora/typora-user-images/image-20201101153931136.png)

其中Makefile是运行cmake之后产生的

func.cpp，func.h和test.cpp的内容如下

![image-20201101154028114](/home/sheep/.config/Typora/typora-user-images/image-20201101154028114.png)

![image-20201101154048265](/home/sheep/.config/Typora/typora-user-images/image-20201101154048265.png)

![image-20201101154125555](/home/sheep/.config/Typora/typora-user-images/image-20201101154125555.png)

修改CMakeLists.txt 

![image-20201101154258041](/home/sheep/.config/Typora/typora-user-images/image-20201101154258041.png)

在add_executable的参数中把func.cpp加入进来

然后重新执行cmake生成Makefile并运行make

![image-20201101154352755](/home/sheep/.config/Typora/typora-user-images/image-20201101154352755.png)运行成功

但是考虑当我们有很多源文件的时候，一个一个把源文件加进去就有点不合适了

所以cmake提供了一个命令可以把指定目录下的所有源文件储存在一个变量中

`aux_source_directory(dir var)` 第一个参数是指定目录 第二个参数是用于存放源文件列表的变量

我们再添加两个文件func1.cpp func1.h

![image-20201101155140355](/home/sheep/.config/Typora/typora-user-images/image-20201101155140355.png)

![image-20201101155200765](/home/sheep/.config/Typora/typora-user-images/image-20201101155200765.png)

在test.cpp中调用对应的函数

修改CMakeLists.txt

![image-20201101155241488](/home/sheep/.config/Typora/typora-user-images/image-20201101155241488.png)

使用aux_source_directory函数将当前目录下的源文件列表存放到SRC_LIST中

注意.表示当前目录

然后再在add_executable中调用这个变量，注意变量的调用方法，使用\${}

再次执行cmake和make，并运行main

![image-20201101155745052](/home/sheep/.config/Typora/typora-user-images/image-20201101155745052.png)

运行成功

但是aux_source_directory()也存在弊端，他会指定目录下所有源文件都加进来，所以有可能加入一些我们不需要的文件，此时我们可以使用set命令去新建变量来存放需要的源文件

![image-20201101160020591](/home/sheep/.config/Typora/typora-user-images/image-20201101160020591.png)

# 不同目录下多个源文件

当前目录结构如下

![image-20201101160237278](/home/sheep/.config/Typora/typora-user-images/image-20201101160237278.png)

我们修改CMakeLists.txt如下

![image-20201101160508715](/home/sheep/.config/Typora/typora-user-images/image-20201101160508715.png)

include_directories 该命令是用来向工程添加多个指定头文件的搜索路径，路径之间用空格分割

因为test.cpp里include了func1.h和func.h 所以需要这个命令来指定头文件所在的位置

注意，如果你和我一样使用的是vim编辑器并且使用了ale代码检查插件的话，你会发现ale会向你报错，表示他找不到这个头文件，同时也会影响到ale帮你找其他地方的错误

在顶层的CMakeLists.txt中添加 `set(CMAKE_EXPORT_COMPILE_COMMANDS 1)`

ale会在工程中自动搜索该文件，上述问题即可解决

# 正规的组织结构

![image-20201101162213744](/home/sheep/.config/Typora/typora-user-images/image-20201101162213744.png)

一般我们会将源文件放到src中 头文件放到include中 elf放到bin中

我们编辑最外层的CMakeLists.txt

![image-20201101162631906](/home/sheep/.config/Typora/typora-user-images/image-20201101162631906.png)

add_subdirectory() 可以向当前工程添加存放源文件的子目录，并可以指定中间二进制和目标二进制的存放位置

所以在执行cmake时，就会进入src目录下取寻找src目录下的CMakeLists.txt

所以我们在src下也建立CMakeLists.txt 

![image-20201101162912799](/home/sheep/.config/Typora/typora-user-images/image-20201101162912799.png)

前面说过，set命令用于定义变量 EXECUTABLE_OUTPUT_PATH表示目标二进制可执行文件的存放位置

PROJECT_SOURCE_DIR表示工程的根目录

所以这里的意思就是我们将elf的位置放到根目录下的bin目录中

我们运行cmake，不过我们先切换到build目录下，然后输入cmake ..

makefile就会在build目录下生成 ..表示的就是上一级目录，相当于我们在根目录下进行cmake .

我们再在build下运行make，bin目录下就会生成elf文件

![image-20201101163347845](/home/sheep/.config/Typora/typora-user-images/image-20201101163347845.png)

执行main 运行成功

为什么在build中运行cmake？ 因为cmake生成的文件就会和源码混到一起，会对程序的目录结构钢造成污染，当我们不想用的时候直接清空build目录即可，非常方便。

还有一种写法

![image-20201101163850693](/home/sheep/.config/Typora/typora-user-images/image-20201101163850693.png)

就是在最外层的CMakeLists中写即可