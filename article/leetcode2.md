今天基本上都是中等题，感觉也要复习复习基础知识

[树的子结构](https://leetcode-cn.com/problems/shu-de-zi-jie-gou-lcof/)

先用一个递归的函数判断当前的树是不是可以构成子结构，如果不能的话，就递归的考虑当前树的左子树和右子树

[反转链表](https://leetcode-cn.com/problems/fan-zhuan-lian-biao-lcof/)

这次尝试用递归写了一下，就是递归的反转当前节点的下一个节点，然后再把当前节点接到尾部即可

[一次编辑](https://leetcode-cn.com/problems/one-away-lcci/)

因为只有一次机会，所以当两个字符串长度相同的时候，判断最多只有一个字符不同

当长度不同的时候，最多相差1，并且只能有一个地方不匹配

[零矩阵](https://leetcode-cn.com/problems/zero-matrix-lcci/)

可以用两个数组记录每一行每一列是否出现了0，之后再扫描一遍就行

一个优化是用第一行和第一列去存储这个信息，然后用额外的两个变量去存储第一行和第一列是否有0

[分割链表](https://leetcode-cn.com/problems/partition-list-lcci/)

用两个dummy head分别存储小于x和大于x的节点，最后连起来就行

有一个要注意的点就是在重新构造链表的时候，要把每个节点的后继清除，否则最后一个节点可能以前不是最后一个节点，但是没有清除后继就出现了环

[堆盘子](https://leetcode-cn.com/problems/stack-of-plates-lcci/)

用链表内套栈模拟即可

[栈排序](https://leetcode-cn.com/problems/sort-of-stacks-lcci/)

这个题意很诡异，其实说的就是想让你实现一个小根堆，但是只让用栈

对于当前栈有序的情况下，我们只要把所有小于value的值pop到另一个栈中，然后插入value再把之前的值push回来就行

[环路检测](https://leetcode-cn.com/problems/linked-list-cycle-lcci/)

用快慢指针找到环，然后根据距离推导一下可以发现一个规律，然后根据规律写代码就行

[节点间通路](https://leetcode-cn.com/problems/route-between-nodes-lcci/)

图的遍历

[特定深度节点列表](https://leetcode-cn.com/problems/list-of-depth-lcci/)

bfs

[合法的二叉搜索树](https://leetcode-cn.com/problems/legal-binary-search-tree-lcci/)

一种方法是记录当前子树的上界和下界，然后递归的判断就行

第二种方法是中序遍历，求中序遍历的时候记录一下前一个节点的值，保证中序遍历递增就行

[后继者](https://leetcode-cn.com/problems/successor-lcci/)

还是中序遍历，找到目标节点后设置一下标记，然后把中序遍历的下一个点记录下来就行

[求和路径](https://leetcode-cn.com/problems/paths-with-sum-lcci/)

dfs + 前缀和，记录路径上的前缀和，以及对应sum的个数，然后可以直接计算目标和的路径数

[迷路的机器人](https://leetcode-cn.com/problems/robot-in-a-grid-lcci/)

记忆化搜索，因为某个点能不能到达终点不随路径改变

记录下每个点到达下一个有效节点的方向，然后最后模拟一下走一下路径就可以

[两数相加](https://leetcode-cn.com/problems/add-two-numbers/)

用链表模拟加法，正常模拟就行

[无重复的最长子串](https://leetcode-cn.com/problems/longest-substring-without-repeating-characters/)

滑动窗口，维护每个字符的数量，当某一个字符的数量大于1的时候就移动左指针，直到合法为止

[寻找两个有序数组的中位数](https://leetcode-cn.com/problems/median-of-two-sorted-arrays/)

这个思路比较新颖

比如我们要找第k大的数字，分别令p1 = nums1[k / 2 - 1]，p2 = nums2[k / 2 - 1]

当p1小于p2的时候，我们可以确定p1中的前k / 2 - 1个数一定不可能成为我们要的第k个数，所以截断他们

同理，p2小与p1的时候，也可以截断p2

两个数相等的时候，那么他们一定就是整个数组中最小的k / 2 - 1 + k / 2 - 1 = k - 2个数，所以不会影响到答案，也可以截断

实现起来比较巧妙，要多去体会

```cpp
int getKthElement(vector<int> &nums1, vector<int> &nums2, int k) {
    int index1 = 0;
    int index2 = 0;
    while (1) {
        if (index1 == nums1.size()) {
            return nums2[index2 + k - 1];
        }
        if (index2 == nums2.size()) {
            return nums1[index1 + k - 1];
        }
        if (k == 1) {
            return min(nums1[index1], nums2[index2]);
        }

        int new_index1 = min(index1 + k / 2 - 1, int(nums1.size() - 1));
        int new_index2 = min(index2 + k / 2 - 1, int(nums2.size() - 1));
        if (nums1[new_index1] <= nums2[new_index2]) {
            k -= new_index1 - index1 + 1;
            index1 = new_index1 + 1;
        } else {
            k -= new_index2 - index2 + 1;
            index2 = new_index2 + 1;
        }
    }
}
```

[电话号码的组合](https://leetcode-cn.com/problems/letter-combinations-of-a-phone-number/)

模拟就行