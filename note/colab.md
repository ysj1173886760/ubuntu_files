# colab使用教程

在这里稍微整理一下colab的使用教程，以便自己日后复习，或他人观看

现在感觉自己学习的过程像是在不断的刷漆，一遍又一遍，干了再刷，刷了再干，在此过程中逐渐凝实，并最终内化为自己的东西

首先colab是要配合google drive来进行操作的

我们先登入google drive

![20210125090416](https://picsheep.oss-cn-beijing.aliyuncs.com/pic/20210125090416.png)

左上角new -> more -> google colaboratory

有关colab的介绍是什么，有什么强大的功能我就不说了，网上有很多人提到

左上角可以修改notebook的名字，我这里叫env，就是用来配置环境的

我们可以把他看做是一个linux+python的环境

输入linux的命令时，需要在前面加上一个叹号，比如这里我们先看看显卡信息

![20210125090721](https://picsheep.oss-cn-beijing.aliyuncs.com/pic/20210125090721.png)

输入!nvidia-smi并运行就好

在这里我们可以看到我们的cuda版本，在右上角，然后我们可以看一下pytorch的版本等信息，如果没有的话，就需要我们自己去pytorch官网，选择对应的版本，官网就会给我们一段指令，复制过来运行即可

![20210125090931](https://picsheep.oss-cn-beijing.aliyuncs.com/pic/20210125090931.png)

这里我们可以看到我们pytorch的版本 1.7.0 并且是gpu版本

接下来我们用一个例子来讲解具体的数据上传过程

首先我们要先挂载google drive

输入这段代码

```python
from google.colab import drive
drive.mount('/content/drive/')
```

![20210125091158](https://picsheep.oss-cn-beijing.aliyuncs.com/pic/20210125091158.png)

进入到这个链接中获取验证码，输入进来并回车，就成功挂载了google drive

![20210125091311](https://picsheep.oss-cn-beijing.aliyuncs.com/pic/20210125091311.png)

这时候你可以通过左边的文件夹看到自己当前都挂载了什么东西

我们可以使用!ls看到当前目录下的东西

但是如果是切换目录，这里的cd就不是很好用了

```python
import os
os.chdir('/content/drive/My Drive/gan')
!ls
```

用python os模块的chdir来更改目录，比如这里我要去我的gan文件夹

![20210125091518](https://picsheep.oss-cn-beijing.aliyuncs.com/pic/20210125091518.png)

!ls指令就会给我们结果，同理，linux下的很多命令都是这样用的，比如移动mv，解压unzip等

然后我们可以上传我们的数据了

其实你可以直接将你的整个项目压缩，然后上传到google drive上来再解压，然后直接在这里运行既可

如果你的数据是github的，你可以直接使用git clone命令来直接下载到google drive中

你也可以在这里直接写代码，写完就运行，要注意的是他貌似每次都会刷新你的package，所以很多包都需要你重新下载，可能是每次的runtime都是独立并重置的

那么比如说我这里把项目文件都组织好了，然后我们写好了代码

![20210125092045](https://picsheep.oss-cn-beijing.aliyuncs.com/pic/20210125092045.png)

像这样，记住路径一定要选择绝对路径，相对路径会出现问题

我这里只把train放了上来，然后看右上角的连接，据说是要选择托管代码执行程序，但是我目前还没找到区别，所以我们直接点连接

然后把需要下载的用pip下载完

然后我们点左上角的修改

![20210125092452](https://picsheep.oss-cn-beijing.aliyuncs.com/pic/20210125092452.png)

笔记本设置中将硬件加速修改成GPU，这样我们就可以用google的显卡加速了

通过右上角的RAM来观察，点进去可以看到你当前的CPU和GPU使用情况，来判断你是否成功使用了GPU