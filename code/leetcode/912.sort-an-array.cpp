/*
 * @lc app=leetcode.cn id=912 lang=cpp
 *
 * [912] 排序数组
 */

// @lc code=start
class Solution {
public:
    vector<int> sortArray(vector<int>& nums) {
        int n = nums.size(), exp = 1;
        int maxx = 0;
        vector<int> buffer(n);
        for (const auto &x : nums) maxx = max(maxx, abs(x));
        while (exp <= maxx) {
            vector<int> cnt(19);
            for (const auto &x : nums) cnt[(x / exp) % 10 + 9]++;
            for (int i = 1; i < 19; i++) cnt[i] += cnt[i - 1];
            for (int i = n - 1; i >= 0; i--) {
                int digit = (nums[i] / exp) % 10 + 9;
                buffer[cnt[digit] - 1] = nums[i];
                cnt[digit]--;
            }
            copy(buffer.begin(), buffer.end(), nums.begin());
            exp *= 10;
        }
        return nums;
    }
};
// @lc code=end
