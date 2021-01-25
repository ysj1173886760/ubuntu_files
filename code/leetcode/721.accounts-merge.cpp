/*
 * @lc app=leetcode.cn id=721 lang=cpp
 *
 * [721] 账户合并
 */

// @lc code=start
class Solution {
public:
    int n;
    vector<int> fa;
    int find(int x) {
        return x == fa[x] ? x : fa[x] = find(fa[x]);
    }
    void unite(int x, int y) {
        x = find(x);
        y = find(y);
        fa[x] = y;
    }
    vector<vector<string>> accountsMerge(vector<vector<string>>& accounts) {
        map<string, int> mp;
        unordered_map<int, int> accounts_id;
        unordered_map<int, int> ans_id;
        vector<vector<string>> ans;
        n = 0;
        int cnt_accounts = 0;
        for (const auto &x : accounts) {
            for (int i = 1; i < x.size(); i++) {
                if (!mp.count(x[i])) {
                    mp[x[i]] = n++;
                    accounts_id[mp[x[i]]] = cnt_accounts;
                }
            }
            cnt_accounts++;
        }
        fa.resize(n);
        for (int i = 0; i < n; i++) fa[i] = i;
        for (const auto &x : accounts) {
            for (int i = 2; i < x.size(); i++) {
                unite(mp[x[i - 1]], mp[x[i]]);
            }
        }
        int cnt = 0;
        for (const auto &[key, value] : mp) {
            int id = accounts_id[find(mp[key])];
            if (!ans_id.count(id)) {
                ans_id[id] = cnt++;
                ans.push_back({});
                ans[ans_id[id]].push_back(accounts[id][0]);
            }
            ans[ans_id[id]].push_back(key);
        }
        return ans;
    }
};
// @lc code=end
