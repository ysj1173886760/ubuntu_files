/*
 * @lc app=leetcode.cn id=797 lang=cpp
 *
 * [797] 所有可能的路径
 */

// @lc code=start
class Solution {
public:
    int n;
    vector<vector<int>> ans;
    void dfs(int cur, vector<vector<int>>& graph, vector<int> now) {
        now.push_back(cur);
        if (cur == n - 1) {
            ans.push_back(now);
            return;
        }
        for (const auto &x : graph[cur]) {
            dfs(x, graph, now);
        }
    }
    vector<vector<int>> allPathsSourceTarget(vector<vector<int>>& graph) {
        n = graph.size();
        dfs(0, graph, vector<int>{});
        return ans;
    }
};
// @lc code=end
