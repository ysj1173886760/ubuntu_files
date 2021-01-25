![image-20210111090941966](/home/sheep/.config/Typora/typora-user-images/image-20210111090941966.png)

![image-20210111091210409](/home/sheep/.config/Typora/typora-user-images/image-20210111091210409.png)

![image-20210112094826797](/home/sheep/.config/Typora/typora-user-images/image-20210112094826797.png)

maxl是最长前后缀

next[i]是i失配后要去的位置，比如在第五个位置失配了，那么下次比较的就是next[5]即第三个位置

nextval是对next的优化，如果next和maxl相同的话，那么如果当前位置失配了，回到next哪里也照样是失配，所以再取一次next，相当于多帮你跳了几步

比如nextval[5]是0，因为如果在第五个位置失配，说明当前的字符肯定不是a，那么回到了next[5] = 3的位置比较的同样是a，肯定是不会成功的，同理，如果继续跳的话回到的是next[3] = 1，但是由于1那个位置还是a，所以肯定也不会成功，那么说明当前这个字符不会成功的，变成0就好。

所以nextval保证你的下一次比较不会是徒劳的，但是不满足这种条件的情况下，也就是maxl不等于next的时候，就跳回next比较就好

（为什么可以多跳很多步而不是一步？因为相同时取得是nextval而不是next）