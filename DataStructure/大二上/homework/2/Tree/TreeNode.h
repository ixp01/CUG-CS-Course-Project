#ifndef TREENODE_H
#define TREENODE_H

template <typename T>
struct TreeNode {
    T value;//树中的值
    TreeNode* left;
    TreeNode* right;
    TreeNode* parent;
    TreeNode(T val) : value(val), left(nullptr), right(nullptr), parent(nullptr) {}//初始化树
};

#endif // TREENODE_H
