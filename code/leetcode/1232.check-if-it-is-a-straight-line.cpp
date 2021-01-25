/*
 * @lc app=leetcode.cn id=1232 lang=cpp
 *
 * [1232] 缀点成线
 */

// @lc code=start
class Solution {
public:
    bool checkStraightLine(vector<vector<int>>& coordinates) {
        int n = coordinates.size();
        for (int i = 1; i < n; i++) {
            coordinates[i][0] -= coordinates[0][0];
            coordinates[i][1] -= coordinates[0][1];
        }
        int A = coordinates[1][1];
        int B = -coordinates[1][0];
        for (int i = 2; i < n; i++) {
            int x = coordinates[i][0];
            int y = coordinates[i][1];
            if (A * x + B * y != 0) return false;
        }
        return true;
    }
};
// @lc code=end
