/*
 * @lc app=leetcode.cn id=1584 lang=cpp
 *
 * [1584] 连接所有点的最小费用
 */

// @lc code=start
class Solution {
public:
    int minCostConnectPoints(vector<vector<int>>& points) {
        int ans = 0;
        priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
        int n = points.size();
        vector<bool> vis(n, false);
        auto dist = [](const vector<int> &x, const vector<int> &y) {
            return abs(x[0] - y[0]) + abs(x[1] - y[1]);
        };
        vector<int> dis(n, INT32_MAX);
        dis[0] = 0;
        pq.push({dis[0], 0});
        while (!pq.empty()) {
            auto[val, x] = pq.top();
            pq.pop();
            if (vis[x]) continue;
            vis[x] = true;
            ans += dis[x];
            for (int i = 0; i < n; i++) {
                if (vis[i]) continue;
                if (dis[i] > dist(points[x], points[i])) {
                    dis[i] = dist(points[x], points[i]);
                    pq.push({dis[i], i});
                }
            }
        }
        return ans;
    }
};
// @lc code=end
