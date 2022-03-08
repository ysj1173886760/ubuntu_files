[蜡烛之间的盘子](https://leetcode-cn.com/problems/plates-between-candles/)

首先预处理出每个位置左边第一个蜡烛的位置，以及右边第一个蜡烛的位置，然后用前缀和算就行

[括号生成](https://leetcode-cn.com/problems/generate-parentheses/)

直接dfs，过程中用栈判断是否合法

[合并K个升序链表](https://leetcode-cn.com/problems/merge-k-sorted-lists/)

用优先队列帮我们从K个表头中找最小的那个，然后append到答案链表中

[最长有效括号](https://leetcode-cn.com/problems/longest-valid-parentheses/)

因为不合法的情况有两种。

一种是右括号多了，当右括号多的时候，之后的合法括号序列一定在这个右括号后面

左括号多的情况下，则是没有对应的右括号去栈中与他更新答案，即合法的序列在此之前就出现了。

我们每次成功构造出合法序列的时候一定是右括号成功匹配左括号的时候，所以只要在成功匹配的地方尝试更新答案即可

对于多余的左括号，就会放在栈中，没有影响。

对于多出来的右括号，则会把现有的栈pop空，我们把这个右括号的位置记下来，那么下次成功匹配以后，我们就可以找到最左边的不合法的位置，从而更新答案

```cpp
class Solution {
public:
    int longestValidParentheses(string s) {
        stack<int> st;
        int ans = 0;
        st.push(-1);
        for (int i = 0; i < s.size(); i++) {
            if (s[i] == '(') {
                st.push(i);
            } else {
                st.pop();
                if (st.empty()) {
                    st.push(i);
                } else {
                    ans = max(ans, i - st.top());
                }
            }
        }
        return ans;
    }
};
```

[跳跃游戏](https://leetcode-cn.com/problems/jump-game/)

这个可以直接记录我们可能跳到的最远的地方，然后不断更新就行

我当时做的时候没考虑到这个，而是用了一个差分来进行区间加，当这个位置的值大于1的时候，就可以继续跳跃

[跳跃游戏2](https://leetcode-cn.com/problems/jump-game-ii/)

就是让多记录一下跳的最小的次数

考虑的核心在于，我们即便是跳过了可以跳到的更远的地方，也不需要考虑回去重跳的情况

即如果当前这次跳跃最远可以到i，那么所有小于i的地方都可以成为下一次起跳的起点，我们不需要考虑具体是在哪里起跳，只要考虑最远能到哪里就行

[合并区间](https://leetcode-cn.com/problems/merge-intervals/)

按照左端点排序后模拟合并就行

[序列化与反序列化](https://leetcode-cn.com/problems/serialize-and-deserialize-binary-tree/)

这个貌似模拟就可以，与直接的前序不同的是，遇到null我们要给他添加到字符串中

[零钱兑换](https://leetcode-cn.com/problems/coin-change/)

完全背包的满包情况，初始化都为无效值就行

[通配符匹配](https://leetcode-cn.com/problems/wildcard-matching/)

这个和昨天的那个很相似，但是这里的*不需要再考虑前一个字符串，所以更简单一些

情况都是一样的，对于*，选择不匹配或者匹配但是不消耗*。其他情况正常匹配即可

[最大矩形](https://leetcode-cn.com/problems/maximal-rectangle/)

悬线法，思路就是对于每一个点，都向上发射一条射线，然后左右移动，来得到最大的面积

具体的实现就是预处理出每个点的左边有多少个1，右边有多少个1。每个点最开始向上射线的长度是1，如果上面也是1的话，就可以继承上面射线的属性，同时要更新left和right。因为下面可能挡住上面，所以取min

[地下城游戏](https://leetcode-cn.com/problems/dungeon-game/)

正向的考虑会有后效性，因为最开始的选择会受到后面路径的影响

所以反着考虑，当前点到终点所需要的最小的血量

其实我感觉我直接想是不太好想的，所以如果数据量允许的情况下，我们可以二分答案，二分起始的血量，然后做dp，看看最后能不能救到公主。至于如何保证血量都不为负数，可以在dp到负数的时候设一个极小值来无效话这个值

[分割数组的最大值](https://leetcode-cn.com/problems/split-array-largest-sum/)

二分答案，然后贪心的check

[树节点的第K个祖先](https://leetcode-cn.com/problems/kth-ancestor-of-a-tree-node/)

树上倍增，预处理出每个节点向上走2^k步所能到达的节点

[T秒后青蛙的位置](https://leetcode-cn.com/problems/frog-position-after-t-seconds/)

模拟一下就行，因为他不会回到原来的节点，所以当时间不对应的时候就是0，但是要特判叶子节点

[二叉树展开为链表](https://leetcode-cn.com/problems/flatten-binary-tree-to-linked-list/)

模拟。dfs返回子树转化成链表以后的最后一个节点，然后分情况处理就行

[最大交换](https://leetcode-cn.com/problems/maximum-swap/)

记录每一个数字在数组中最右边索引的位置

然后从第一个数字开始从9到0枚举，找到第一个比他大且索引在他的右边，与他交换就行

[K个一组反转链表](https://leetcode-cn.com/problems/reverse-nodes-in-k-group/)

分别划分每组链表，然后都做一遍翻转即可