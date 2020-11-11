## 第一章

可将操作系统定义为一种软件，这种软件称之为内核(kernel)

内核的接口被称为系统调用(system call)

广义上，操作系统包括了内核和一些其他软件

![image-20201108095453497](/home/sheep/.config/Typora/typora-user-images/image-20201108095453497.png)

例如Linux就是GNU操作系统的内核

![image-20201108095833351](/home/sheep/.config/Typora/typora-user-images/image-20201108095833351.png)

![image-20201108100132298](/home/sheep/.config/Typora/typora-user-images/image-20201108100132298.png)

目录的起点称为根(root) 名字是一个字符`/`

![image-20201108100502476](/home/sheep/.config/Typora/typora-user-images/image-20201108100502476.png)

所以cd ..并不是指返回父目录，而是进入到当前目录下一个对父目录的引用？

文件结束字符通常为CTRL+D

标准I/O函数提供一种对不用缓冲I/O函数的带缓冲的接口

![image-20201108103239366](/home/sheep/.config/Typora/typora-user-images/image-20201108103239366.png)

![image-20201108103321573](/home/sheep/.config/Typora/typora-user-images/image-20201108103321573.png)

信号是通知进程已发生某种情况的一种技术

Delete 或 Ctrl+C 是中断键 Ctrl+/是退出键，可以产生让当前进程终端的信号

UNIX系统使用三个进程时间值：时钟时间，用户CPU时间，系统CPU时间

时钟时间是进程运行的时间总量

用户CPU时间是用户指令所用的时间

系统CPU时间是为该进程执行内核程序所经历的时间

用户CPU时间和系统CPU时间之和成为CPU时间

![image-20201108165615701](/home/sheep/.config/Typora/typora-user-images/image-20201108165615701.png)

系统调用通常提供一种最小接口，而库函数通常提供比较复杂的功能

## 第二章

