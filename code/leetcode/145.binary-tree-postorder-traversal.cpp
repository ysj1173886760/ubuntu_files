/*
 * @lc app=leetcode.cn id=145 lang=cpp
 *
 * [145] 二叉树的后序遍历
 */

// @lc code=start
/**
 * Definition for a binary tree node.
 * struct TreeNode {
 *     int val;
 *     TreeNode *left;
 *     TreeNode *right;
 *     TreeNode() : val(0), left(nullptr), right(nullptr) {}
 *     TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
 *     TreeNode(int x, TreeNode *left, TreeNode *right) : val(x), left(left), right(right) {}
 * };
 */
class Solution {
public:
    void add(vector<int> &ans, TreeNode *now) {
        vector<int> tmp;
        while (now != nullptr) {
            tmp.push_back(now->val);
            now = now->right;
        }
        for (int i = tmp.size() - 1; i >= 0; i--) ans.push_back(tmp[i]);
    }
    vector<int> postorderTraversal(TreeNode* root) {
        TreeNode *rt = root;
        vector<int> ans;
        while (root != nullptr) {
            if (root->left == nullptr) {
                root = root->right;
            } else {
                TreeNode *tmp = root->left;
                while (tmp->right != nullptr && tmp->right != root) tmp = tmp->right;
                if (tmp->right == nullptr) {
                    tmp->right = root;
                    root = root->left;
                } else {
                    tmp->right = nullptr;
                    add(ans, root->left);
                    root = root->right;
                }
            }
        }
        add(ans, rt);
        return ans;
    }
};
// @lc code=end
