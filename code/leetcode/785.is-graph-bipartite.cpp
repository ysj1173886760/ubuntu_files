/*
 * @lc app=leetcode.cn id=785 lang=cpp
 *
 * [785] 判断二分图
 */

// @lc code=start
class Solution {
public:
    bool dfs(int cur, vector<int> &col, vector<vector<int>>& graph) {
        for (const auto &x : graph[cur]) {
            if (col[x] != -1) {
                if (col[x] == col[cur]) return false;
            } else {
                col[x] = 1 - col[cur];
                if (!dfs(x, col, graph)) return false;
            }
        }
        return true;
    }
    bool isBipartite(vector<vector<int>>& graph) {
        int n = graph.size();
        vector<int> col(n, -1);
        for (int i = 0; i < n; i++) {
            if (col[i] != -1) continue;
            col[i] = 0;
            if (!dfs(i, col, graph)) return false;
        }
        return true;
    }
};
// @lc code=end
