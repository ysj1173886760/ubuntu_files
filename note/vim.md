### three basic mode of vim

1. command mode, also is default mode. all the alphanumeric keys are bound to commands. Typing `dd` will delete an entire line.
2. insert mode: type `i` (for insert) to enter the insert mode and the keys will behave what u expect. Hit the `Escape` key to get out.
3. last-line mode: type `:`when you\`re at command mode and vim will switch to last-line mode. u can use `:w` to write a file or `:q` to exit the editor.

### basics of moving in vim

![image-20201030120740780](/home/sheep/.config/Typora/typora-user-images/image-20201030120740780.png)

prefacing a movement command with a number will execute that movement multiple times

### editing vim style

`x`to delete the character that the cursor was on

![image-20201030121624374](/home/sheep/.config/Typora/typora-user-images/image-20201030121624374.png)

several commands combine a text operation and movement key. 

you may combine something together like verb+noun. and the answer of vim is often yes

### searching and replacing

![image-20201030151649713](/home/sheep/.config/Typora/typora-user-images/image-20201030151649713.png)

about how to clear last searching highlighting in vim

![image-20201030152121771](/home/sheep/.config/Typora/typora-user-images/image-20201030152121771.png)

`:noh`really helps a lot

### copying and pasting 

if you\`ve run dd and delete an entire line, you can now hit p and paste it back into the document.

![image-20201031145832137](/home/sheep/.config/Typora/typora-user-images/image-20201031145832137.png)

a usual paste operation might look like this: hit v or V to highlight some text, then hit y to yank(put) it into the buffer, then move the cursor where u want to it, and hit p in command mode. There you go.

### saving and quiting

`:w` to write the file to the existing file.

if u want to write out to a different filename or u don\`t have the file, use `:w filename`

use `:q` to quit vim and `:q!`to quit without being nagged

`ZZ`also can be used to saving and quiting.

 

vim会在原始文件的目录下生成.swp文件，称为交换文件，用于由于意外退出vim时恢复文件内容

使用`set directory=$HOME/.vim/swap//`来将交换文件存于统一目录中，防止污染系统

或者使用`set noswapfile`禁止交换文件

`w`和`e`用于逐个狭义单词的移动 e for end

`W` `E`用于广义单词的移动，即以空格分割的非空字符序列视为单词

`b` `B`用于移动到狭义或广义单词的开头

`{` `}` 用于向后或向前移动段落，其中段落以空行分割

`c`用于删除一部分文字后立即进入插入模式，比如`cw`删除一个单词并进入插入模式 c for change?

### 缓冲区

使用`:ls`查看当前缓冲区列表

含义分别为缓冲区编号 \\\%表示该缓冲区位于当前窗口中 \\\a表示该缓冲区处于活动状态，他已被加载并可见 \\\文件名 \\\当前光标位置

使用`:e <filename>`打开另一个文件

使用`:b <num>`来在缓冲区之间进行切换 其中空格可以省略

缓冲区还可以用文件名来标识，所以可以使用文件名的一部分来切换缓冲区，但是当出现多个匹配的时候会报错

也可以使用`:bn`(bnext)或者`bp`(bprevious)来循环遍历缓冲区

当不需要某个缓冲区的时候，可以将其删除`:bd`

使用umimpaired插件来 使用`[b`和`]b`来遍历缓冲区

`[f`和`]f`来循环遍历同一目录中的文件

`:split <filename>`来将窗口分割成两个，其中一个显示新的文件

`:vsplit <filename>`来按照水平方向分割窗口

可以使用简写`:vs` `:sp`

使用`Ctrl + w 再输入一个方向键h j k l`来进行光标在窗口间的移动

使用`Ctrl + w, q`来关闭当前窗口

使用`Ctrl + w, o`来关闭除当前窗口之外的所有窗口

使用`Ctrl + w 再输入一个大写的方向键H J K L`来移动当前的窗口

有关窗口的大小

使用`Ctrl + w, =`将打开窗口的宽高调为一致

`resize +N/-N`将当前窗口的高度增加/减少N行

`vertical resize +N/-N` 将当前窗口的宽度增加/减少N列

同时可以简写为`res` `vert res`

`Ctrl + w, +/-`用来调整高度

`Ctrl + w, </>`用来调整宽度

也可以直接设置为具体的行数/列数

`resize N`  `vertical resize N`

### 标签页

`tabnew <filename>`表示新建一个标签页，如果没有文件名就是新建一个空的标签页

在标签页中可以通过`:e`来加载文件，也可以用`:b`在不同缓冲区间切换

在不同标签页间跳转，可以使用

- 快捷键gt 或`tabnext`用于切换到下一个标签
- 快捷键gT或`tabprevious`切换到上一个标签

标签页可以使用`:tabclose`来关闭，也可以使用`:q`

`:tabmove N`用于将当前标签页移动到第N个标签页之后

### NERDTree

使用`:Bookmark`来收藏当前光标在NERDTree中选择的目录，在NERDTree中按B可以列出所有的书签

### 文本浏览

t (until) 后接一个字母 用于在当前行内搜索该字符，并将光标置于该字符前 T用于反向搜索

f (find) 后接一个字母 用于在当前行内搜索该字符，并将光标置于该字符上 F用于反向搜索

_用于将光标放到行首 0也可以

### 切换到插入模式

i表示在当前光标处进入插入模式

a表示在当前光标后面

A表示在当前行尾

I表示在行首

o表示在下面新增一行，进入插入模式

O表示在上面新增一行，进入插入模式

### easymotion

\\\表示前导符号 easymotion的指令都要先加上前导符号

`\\w表示逐个单词的移动方式`

`\\f 表示向右查找字符 F表示向左`

同样的还有t 和T

和之前相同，wbe都可以用于单词的跳动

w ge表示向后跳 一个开头 一个末尾

b e表示向前 一个开头 一个末尾

### 寄存器

`y 和 p`就是从寄存器中进行的复制粘贴，但是是无名寄存器 用"表示

寄存器的访问方式是"，后接寄存器的标识符

a~z所标识的寄存器用于手动复制数据

数字编号的寄存器表示最后10次删除操作的历史记录

%表示当前文件名 :储存最后的命令等

使用`reg <name>`来访问寄存器的内容

+寄存器（针对linux）表示粘贴板选择器，即ctrl+c操作

