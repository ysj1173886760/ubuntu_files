/*
 * @lc app=leetcode.cn id=803 lang=cpp
 *
 * [803] 打砖块
 */

// @lc code=start
class Solution {
public:
    vector<int> fa;
    vector<int> sz;
    int h, w;
    int find(int x) {
        return x == fa[x] ? x : fa[x] = find(fa[x]);
    }
    void unite(int x, int y) {
        x = find(x);
        y = find(y);
        if (x == y) return;
        fa[x] = y;
        sz[y] += sz[x];
    }
    vector<int> hitBricks(vector<vector<int>>& grid, vector<vector<int>>& hits) {
        h = grid.size();
        w = grid[0].size();
        fa.resize(h * w + 1);
        sz.resize(h * w + 1);
        for (int i = 0; i <= h * w; i++) {
            fa[i] = i;
            sz[i] = 1;
        }
        vector<vector<int>> status = grid;
        for (int i = 0; i < hits.size(); i++) {
            status[hits[i][0]][hits[i][1]] = 0;
        }
        for (int i = 0; i < h; i++) {
            for (int j = 0; j < w; j++) {
                if (status[i][j] == 1) {
                    if (i == 0) {
                        unite(h * w, j);
                    }
                    if (i > 0 && status[i - 1][j] == 1) {
                        unite(i * w + j, (i - 1) * w + j);
                    }
                    if (j > 0 && status[i][j - 1] == 1) {
                        unite(i * w + j, i * w + j - 1);
                    }
                }
            }
        } 
        vector<int> ans(hits.size());
        const vector<pair<int, int>> direction{{0, 1}, {1, 0}, {0, -1}, {-1, 0}};
        for (int i = hits.size() - 1; i >= 0; i--) {
            int r = hits[i][0];
            int c = hits[i][1];
            if (grid[r][c] == 0) continue;
            int prev = sz[find(h * w)];
            if (r == 0) unite(c, h * w);
            for (const auto [dr, dc]: direction) {
                int nr = r + dr;
                int nc = c + dc;
                if (nr >= 0 && nr < h && nc >= 0 && nc < w) {
                    if (status[nr][nc] == 1) {
                        unite(r * w + c, nr * w + nc);
                    }
                }
            }
            int size = sz[find(h * w)];
            ans[i] = max(0, size - prev - 1);
            status[r][c] = 1;
        }
        return ans;
    }
};
// @lc code=end
