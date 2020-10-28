### 使用命令帮助

简要说明命令的作用 `whatis command`

正则匹配 `whatis -w 'loca*'`  *表示后面为任意值 详见正则表达式

更加详细的说明文档 `info command`

`info` 可以直接进入帮助界面 使用方法类似VIM

查询命令command的说明文档 `man command`

![image-20201019162839221](/home/sheep/.config/Typora/typora-user-images/image-20201019162839221.png)



![image-20201019163015465](/home/sheep/.config/Typora/typora-user-images/image-20201019163015465.png)

`man -k keyword` 查询关键字 根据命令中的部分关键字来查询命令

![image-20201019163432248](/home/sheep/.config/Typora/typora-user-images/image-20201019163432248.png)

查看程序的binary文件所在路径 `which command`

查看程序的搜索路径 `whereis command`



### 文件及目录管理

- 创建：`mkdir`
- 删除：`rm`
- 删除非空目录：`rm -rf file`
- 删除日志：`rm *log`  等价于 `$find ./ -name "*log" -exec rm {};`
- 移动：`mv`
- 复制：`cp`  复制目录： `cp -r`



![image-20201019164200434](/home/sheep/.config/Typora/typora-user-images/image-20201019164200434.png)

###### 目录切换

![image-20201019164331679](/home/sheep/.config/Typora/typora-user-images/image-20201019164331679.png)

###### 列出目录项

![image-20201019165139280](/home/sheep/.config/Typora/typora-user-images/image-20201019165139280.png)

###### 查找目录及文件（？？？）

![image-20201019165449884](/home/sheep/.config/Typora/typora-user-images/image-20201019165449884.png)

###### 查看文件内容

![image-20201019165754624](/home/sheep/.config/Typora/typora-user-images/image-20201019165754624.png)



###### 感觉没什么用的东西

![image-20201019170028287](/home/sheep/.config/Typora/typora-user-images/image-20201019170028287.png)

#### 管道和重定向

![image-20201019170559070](/home/sheep/.config/Typora/typora-user-images/image-20201019170559070.png)

#### 终端下的快捷输入和删除

![image-20201019170805603](/home/sheep/.config/Typora/typora-user-images/image-20201019170805603.png)

###### grep文本

![image-20201019172806654](/home/sheep/.config/Typora/typora-user-images/image-20201019172806654.png)

![image-20201019173039337](/home/sheep/.config/Typora/typora-user-images/image-20201019173039337.png)

