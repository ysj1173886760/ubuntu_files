#### git commit -am

git commit -am 等价于git commit -a -m

对于一个新的文件来说，他是未跟踪状态的，你无法commit未跟踪的文件，即untracked

对于已经跟踪的文件，如果进行了修改，就变成了未暂存的状态，commit时如果加上了-a命令，则会自动暂存文件，并提交修改后的版本

而如果不加-a直接commit的话，提交的就是修改前的版本

至于-m 就是加上提交的注释罢了



### git基础操作

首先明确，git不等于github，git是版本管理工具，而github是git在云端上的体现

github是托管的代码仓库

git commit 提交 即创建新的节点 其父节点为当前节点

git checkout 切换分支，切换到当前要操作的分支上

git checkout -b <branch-name> 新建分支并切换到该分支

git branch <branch-name> 新建分支

git merge 合并两个分支，即将该分支合并到当前分支上

git rebase 将当前分支移动到该分支下面 （创造更加线性的提交历史）同时也要记得把另一个分支merge到当前节点上，即直接移动



### git远程操作

git clone 是在本地创建一个远程仓库的拷贝，比如从github.com