#### git commit -am

git commit -am 等价于git commit -a -m

对于一个新的文件来说，他是未跟踪状态的，你无法commit未跟踪的文件，即untracked

对于已经跟踪的文件，如果进行了修改，就变成了未暂存的状态，commit时如果加上了-a命令，则会自动暂存文件，并提交修改后的版本

而如果不加-a直接commit的话，提交的就是修改前的版本

至于-m 就是加上提交的注释罢了