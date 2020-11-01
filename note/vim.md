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

 