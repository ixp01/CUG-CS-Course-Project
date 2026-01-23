#ifndef BINARYTREE_H
#define BINARYTREE_H
#include<iostream>
using namespace std;
#include<vector>
//使用前向声明
class TWidget;

template <typename T>
struct TreeNode {
    T value;//树中的值
    TreeNode* left;
    TreeNode* right;
    TreeNode* parent;
    TreeNode(T val) : value(val), left(nullptr), right(nullptr), parent(nullptr) {}//初始化树
};

template<typename T>
class BinaryTree
{
public:
    TreeNode<T>* root;
public:
    BinaryTree();
    ~BinaryTree();
    void buildPITree(const vector<T>& preorder, const vector<T>& inorder,TWidget* widget);//前序中序构建二叉树
    TreeNode<T>* buildTreeHelperPreIn(const std::vector<T>& preorder, const std::vector<T>& inorder,
                                                     int preStart, int inStart, int inEnd,TWidget* widget);//前序中序构建二叉树辅助函数

    void buildPPTree(const vector<T>& preorder, const vector<T>& postorder);//前序后序构建二叉树

    void buildIPTree(const vector<T>& inorder, const vector<T>& postorder,TWidget* widget);//中序后序构建二叉树
    TreeNode<T>* buildTreeHelperInPost(const std::vector<T>& inorder, const std::vector<T>& postorder,
                                                      int inStart, int inEnd, int postStart,TWidget* widget);//中序后序构建二叉树辅助函数

    int findIndex(const std::vector<T>& sequence, int start, int end, T value);//查找索引

    TreeNode<T>* findParent(T value); // 查找节点的父节点
    TreeNode<T>* lowestCommonAncestor(T value1, T value2); // 查找两个节点的最小公共祖先
    bool canBuildTreeFromPreIn(const std::vector<T>& preorder, const std::vector<T>& inorder);// 检查前序和中序序列是否可以构建二叉树
    bool canBuildTreeFromInPost(const std::vector<T>& inorder, const std::vector<T>& postorder);// 检查中序和后序序列是否可以构建二叉树
    bool canBuildTreeFromPreInHelper(const std::vector<T>& preorder, const std::vector<T>& inorder,
                                                    int preStart, int inStart, int inEnd);//序列检查帮助者函数
    bool canBuildTreeFromInPostHelper(const std::vector<T>& inorder, const std::vector<T>& postorder,
                                                     int inStart, int inEnd, int postIndex);//序列检查帮助者函数


};

#endif // BINARYTREE_H
