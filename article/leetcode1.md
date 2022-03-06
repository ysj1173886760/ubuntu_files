记录一下刷题过程中遇到的一些值得思考的点，以便之后复习用。

同时回顾一下每天遇到的问题，来重新整理自己的思路。

[题目1](https://leetcode-cn.com/problems/find-good-days-to-rob-the-bank/)

求数组中的波谷，但是波谷要满足一定的长度

分别求出来每一个位置上，最长的非递增序列和非递减序列的长度。最后遍历一遍就可以

[题目2](https://leetcode-cn.com/problems/gou-jian-cheng-ji-shu-zu-lcof/)

对于每一个位置，求出数组中所有数除去当前数的乘积

这个最简单的方法就是求出前缀积和后缀积，然后单独计算就行

一个优化是可以只用一个变量来保存前缀积或后缀积，然后给对应的位置乘上贡献即可

[题目3](https://leetcode-cn.com/problems/nge-tou-zi-de-dian-shu-lcof/)

投n个骰子，问每种点数出现的概率是多少

设dp[i, j]为当前投了i个骰子，点数为j的概率

转移就是dp[i, j] = sum(dp[i - 1, j - k]) * (1 / 6) where 1 <= k <= 6

即当前骰子的点数为k，总点数为j，那么就从前面骰子中的j - k转移来就行

[题目4](https://leetcode-cn.com/problems/ba-zi-fu-chuan-zhuan-huan-cheng-zheng-shu-lcof/)

字符串转化成整数，这个比较容易，模拟就行

[题目5](https://leetcode-cn.com/problems/shu-ju-liu-zhong-de-zhong-wei-shu-lcof/)

求数据流中的中位数

这个有点像以前的一道题叫黑匣子

可以转化为一个一般化的问题就是求第k大

但是不一样的是数据是由数据流给出来的，所以不能离散化后用权值bit去求kth

当然了可以维护一个平衡树

更简单的方法是维护两个堆，一个堆的大小就是k，存最大的k个元素

当新的元素加入的时候，如果他比最大的k个元素中最小的那个还大，那么他就可以把最小的那个挤到另一个堆中

这样我们就可以用两个堆把数据话分开

[题目6](https://leetcode-cn.com/problems/shu-zu-zhong-de-ni-xu-dui-lcof/)

求逆序对

离散化之后用树状数组

在这贴一下代码吧，感觉之后会比较常用

```cpp
class Solution {
public:
    vector<int> bit;
    int lowbit(int x) {
        return x & -x;
    }
    void add(int pos, int x) {
        for (int i = pos; i < bit.size(); i += lowbit(i)) {
            bit[i] += x;
        }
    }
    int sum(int pos) {
        int ans = 0;
        for (int i = pos; i; i -= lowbit(i)) {
            ans += bit[i];
        }
        return ans;
    }
    int reversePairs(vector<int>& nums) {
        int n = nums.size();

        vector<int> discrete = nums;
        sort(discrete.begin(), discrete.end());
        discrete.erase(unique(discrete.begin(), discrete.end()), discrete.end());

        for (auto &x : nums) {
            // make range to [1, xx]
            x = lower_bound(discrete.begin(), discrete.end(), x) - discrete.begin() + 1;
        }

        bit = vector<int> (discrete.size() + 1);
        int ans = 0;

        for (int i = n - 1; i >= 0; i--) {
            ans += sum(nums[i] - 1);
            add(nums[i], 1);
        }

        return ans;
    }
};
```

[题目7](https://leetcode-cn.com/problems/hua-dong-chuang-kou-de-zui-da-zhi-lcof/)

滑动窗口最大值

最简单的做法就是用map或者set这样的平衡树，可加可减可以求最值

还可以用单调队列来获得更好的复杂度，因为滑动窗口本身就是一个固定长度的队列，我们维护一下队列中的单调性

当一个新的元素加入进来的时候，那么在队列中所有比他小的元素都不可能成为最大值，因为他们必定会被先pop出去，我们就直接pop掉这些元素就行

所以维护的是一个单调递减的队列，队首永远是最大值

[题目8](https://leetcode-cn.com/problems/zheng-ze-biao-da-shi-pi-pei-lcof/)

正则表达式匹配

由于有*，所以每次匹配的字符个数是不确定的，要用dp来解决

设dp[i, j]为当前s匹配到i，p匹配到j的时候是否可以匹配成功

如果当前p[j]是*的话，就要检查p[j - 1]是不是可以匹配s[i]

首先可以确定的情况是*代表0个字符，那么dp[i, j]就可以从dp[i, j - 2]继承过来，表示去掉这两个字符

如果p[j - 1]可以匹配s[i]，我们就也可以尝试让他匹配s[i - 1]，但是这样递归下去对于每一个*都要枚举所有的字符

换一个思路就是当当前字符是*的时候，我们尝试匹配p[j - 1]和s[i]，对于可能的p[j - 1]和s[i - 1]，我们用过dp[i - 1, j]来转移

相当于是看作没有消耗p就匹配了s[i]，这样在dp[i - 1, j]的时候，他就可以继续从i - 2继承，或者放弃继续匹配，从dp[i - 1, j - 2]来转移了

总结下来就是当当前字符为*的时候 dp[i, j] |= dp[i, j - 2] | (dp[i - 1, j] when p[j - 1] match s[i])

否则的话，直接按字符匹配即可 dp[i, j] |= dp[i - 1, j - 1] when s[i] match p[j]

[题目9](https://leetcode-cn.com/problems/replace-non-coprime-numbers-in-array/)

题目中给出了提示，以任意顺序替换相邻的非互质数都可以得到相同的结果

我们就可以贪心的从最左边开始进行替换，通过样例可以发现，可能出现中间一个数将左右两边的数联系起来的情况，所以当我们成功替换以后，还要去检查当前数是否可以和上一个数继续进行替换

用栈模拟一下就可以

[题目10](https://leetcode-cn.com/problems/bst-sequences-lcci/)

这个题有点诡异，其实就是求出所有的序列，满足子节点要在父节点后面出现

因为要求所有的状态，所以要用dfs，但是算法本身要求用bfs，所以要把两个结合起来

在dfs的过程中用双端队列来维护bfs的队列即可

[题目11](https://leetcode-cn.com/problems/eight-queens-lcci/)

八皇后，就是输出一下每一种状态

[题目12](https://leetcode-cn.com/problems/pile-box-lcci/)

线性的dp，先排序然后做dp就行

[题目13](https://leetcode-cn.com/problems/number-of-2s-in-range-lcci/)

求1到n中出现2的次数

这个其实是一个通式，考虑每一个位上的贡献

比如百位上的2,其实就是每循环1000个数就会有100个2,分别是200,201,202...，注意只考虑百位

对于千位上的2,则是每10000个数就会有1000个2。所以通式就是每exp * 10个数会有exp个2

然后对于没有循环满的情况，单独计算即可。更具体一点，就是先把余出来的这部分求出来，然后减去基数就行

比如1234,首先求出余出来的部分就是234,然后减去200就是答案

[题目14](https://leetcode-cn.com/problems/missing-two-lcci/)

有两种做法，一种就是任意值域的情况下，首先用xor求出这两个独立的数，然后用其中的一个二进制位来把整个数组划分为两部分，然后两部分分别xor就是答案

[题目15](https://leetcode-cn.com/problems/max-submatrix-lcci/)

以前的一个经典题

首先用2重循环求出列级别的前缀和，这样就得到了所有的矩阵的不同的行的表达

然后再求最大子序列就行

主要是思路比较棒

[题目16](https://leetcode-cn.com/problems/word-rectangle-lcci/)

用dfs来选单词，然后用trie来优化检查单词是否合法

在dfs的过程中维护一下每一列都在trie的哪个节点，就可以O(1)的检查新的字符串是不是合法的

[题目17](https://leetcode-cn.com/problems/sparse-similarity-lcci/)

倒排索引，对每一个单词维护包含他的文档ID就行

然后可以通过哈希表直接求交集

[题目18](https://leetcode-cn.com/problems/english-int-lcci/)

用英语表示整数

其实就是将数字划分成billion，million，thousand这几个级别

然后每个级别单独得出他对应的英文表示，再和大的后缀拼接起来就行

[题目19](https://leetcode-cn.com/problems/lfu-cache/)

LFU

两种思路，一种是用平衡树，键值是<freq, timestamp>，这样会优先找最小频率的值，如果相同的freq就找更小的timestamp就行

第二种思路就是对原本LRU的拓展

对于每一个freq都维护一个list，然后再维护key到list中的iterator的映射

通过key找到iterator，然后把它从当前list删除，并添加到更高频率的list中即可