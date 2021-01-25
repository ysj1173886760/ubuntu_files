/*
 * @lc app=leetcode.cn id=1018 lang=cpp
 *
 * [1018] 可被 5 整除的二进制前缀
 */

// @lc code=start
class Solution {
public:
    vector<bool> prefixesDivBy5(vector<int>& A) {
        vector<bool> ans;
        int now = 0;
        for (const auto &x : A) {
            now = now * 2 + x;
            now %= 5;
            ans.push_back(now == 0);
        }
        return ans;
    }
};
// @lc code=end
