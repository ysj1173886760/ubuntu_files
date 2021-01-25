/*
 * @lc app=leetcode.cn id=947 lang=cpp
 *
 * [947] 移除最多的同行或同列石头
 */

// @lc code=start
class Solution {
public:
    vector<int> fa;
    int find(int x) {
        return fa[x] == x ? x : fa[x] = find(fa[x]);
    }
    void unite(int x, int y) {
        x = find(x);
        y = find(y);
        fa[x] = y;
    }
    int removeStones(vector<vector<int>>& stones) {
        int n = stones.size();
        fa.resize(n);
        unordered_map<int, vector<int>> mp;
        for (int i = 0; i < n; i++) fa[i] = i;
        for (int i = 0; i < n; i++) {
            mp[stones[i][0] + 10001].push_back(i);
            mp[stones[i][1]].push_back(i);
        }
        for (auto [_, vec] : mp)
            for (int i = 1; i < vec.size(); i++)
                unite(vec[i - 1], vec[i]);
        int ans = 0;
        for (int i = 0; i < n; i++) ans += fa[i] == i;
        return n - ans;
    }
};
// @lc code=end
