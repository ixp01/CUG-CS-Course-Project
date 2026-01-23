#include "binarytree.h"
#include"twidget.h"
template class BinaryTree<int>;//模板显式实例化
/*
使用的模板类后，函数实现需与类同源。若将函数实现直接置于相应的cpp文件中，将出现编译错误。
可以通过使用export关键字和模板显式实例化解决这一问题。
因为Qt不支持export关键字，所以在本代码中使用的是模板显式实例化。
*/
template<typename T>
BinaryTree<T>::BinaryTree():root(nullptr){}

template<typename T>
BinaryTree<T>::~BinaryTree()
{//实现析构函数

}

//前序中序构建二叉树
template<typename T>
void BinaryTree<T>::buildPITree(const vector<T> &preorder, const vector<T> &inorder,TWidget* widget)
{
    if (preorder.empty() || inorder.empty() || preorder.size() != inorder.size()) return;
    root = buildTreeHelperPreIn(preorder, inorder, 0, 0, inorder.size() - 1,widget);
    if (root) root->parent = nullptr; // 根节点的parent设置为nullptr
}
//前序中序构建二叉树辅助函数
template<typename T>
TreeNode<T> *BinaryTree<T>::buildTreeHelperPreIn(const std::vector<T> &preorder, const std::vector<T> &inorder, int preStart, int inStart, int inEnd,TWidget* widget)
{
    if (preStart >= preorder.size() || inStart > inEnd) return nullptr;

    T rootValue = preorder[preStart];
    TreeNode<T>* root = new TreeNode<T>(rootValue);
    root->parent = nullptr; // 初始化时假设没有父节点，稍后将被设置

    int inIndex = findIndex(inorder, inStart, inEnd, rootValue);
    int leftTreeSize = inIndex - inStart;

    if (leftTreeSize > 0) {
        root->left = buildTreeHelperPreIn(preorder, inorder, preStart + 1, inStart, inIndex - 1,widget);
        if (root->left) root->left->parent = root; // 设置左子节点的parent指针
    }

    if (inIndex + 1 <= inEnd) {
        root->right = buildTreeHelperPreIn(preorder, inorder, preStart + leftTreeSize + 1, inIndex + 1, inEnd,widget);
        if (root->right) root->right->parent = root; // 设置右子节点的parent指针
    }

    if (widget) widget->update(); // 在每次递归返回前调用update

    return root;
}


//前序后序构建二叉树
template<typename T>
void BinaryTree<T>::buildPPTree(const vector<T> &preorder, const vector<T> &postorder)
{

}


//中序后序构建二叉树
template<typename T>
void BinaryTree<T>::buildIPTree(const vector<T> &inorder, const vector<T> &postorder,TWidget* widget)
{
    if (inorder.empty() || postorder.empty() || inorder.size() != postorder.size()) return;
    root = buildTreeHelperInPost(inorder, postorder, 0, 0, postorder.size() - 1,widget);
    if (root) root->parent = nullptr; // 根节点的parent设置为nullptr
}
//中序后序构建二叉树辅助函数
template<typename T>
TreeNode<T> *BinaryTree<T>::buildTreeHelperInPost(const std::vector<T> &inorder, const std::vector<T> &postorder, int inStart, int inEnd, int postStart,TWidget* widget)
{
    if (inStart > inEnd) return nullptr;

    T rootValue = postorder[postStart];
    TreeNode<T>* root = new TreeNode<T>(rootValue);
    root->parent = nullptr; // 初始化时假设没有父节点，稍后将被设置

    int inIndex = findIndex(inorder, inStart, inEnd, rootValue);

    if (inIndex > inStart) { // 有左子树
        root->left = buildTreeHelperInPost(inorder, postorder, inStart, inIndex - 1, postStart,widget);
        if (root->left) root->left->parent = root; // 设置左子节点的parent指针
    }

    if (inIndex < inEnd) { // 有右子树
        root->right = buildTreeHelperInPost(inorder, postorder, inIndex + 1, inEnd, postStart + (inEnd - inIndex),widget);
        if (root->right) root->right->parent = root; // 设置右子节点的parent指针
    }

    if (widget) widget->update(); // 在每次递归返回前调用update

    return root;
}


//查找索引
template<typename T>
int BinaryTree<T>::findIndex(const std::vector<T> &sequence, int start, int end, T value)
{
    for (int i = start; i <= end; ++i) {
        if (sequence[i] == value) return i;
    }
    return -1; // 应该总是找到值，如果找不到则可能输入序列有误
}
// 查找节点的父节点
template<typename T>
TreeNode<T> *BinaryTree<T>::findParent(T value)
{

}

// 查找两个节点的最小公共祖先
template<typename T>
TreeNode<T> *BinaryTree<T>::lowestCommonAncestor(T value1, T value2)
{

}

// 检查前序和中序序列是否可以构建二叉树
template<typename T>
bool BinaryTree<T>::canBuildTreeFromPreIn(const std::vector<T> &preorder, const std::vector<T> &inorder)
{
    if (preorder.empty() || inorder.empty() || preorder.size() != inorder.size()) return false;
    return canBuildTreeFromPreInHelper(preorder, inorder, 0, 0, inorder.size());
}
// 检查中序和后序序列是否可以构建二叉树
template<typename T>
bool BinaryTree<T>::canBuildTreeFromInPost(const std::vector<T> &inorder, const std::vector<T> &postorder)
{
    if (inorder.empty() || postorder.empty() || inorder.size() != postorder.size()) return false;
    return canBuildTreeFromInPostHelper(inorder, postorder, 0, inorder.size() - 1, postorder.size() - 1);
}
//前序中序检查帮助者
template<typename T>
bool BinaryTree<T>::canBuildTreeFromPreInHelper(const std::vector<T> &preorder, const std::vector<T> &inorder, int preStart, int inStart, int inEnd)
{
    if (preStart >= preorder.size() || inStart > inEnd) return false;
    T root = preorder[preStart];
    int rootIndex = -1;
    for (int i = inStart; i <= inEnd; ++i) {
        if (inorder[i] == root) {
            rootIndex = i;
            break;
        }
    }
    if (rootIndex == -1) return false; // Root not found in inorder

    int leftTreeSize = rootIndex - inStart;
    if (leftTreeSize != (preStart + 1 >= preStart + leftTreeSize + 1 ?
        preorder.size() : preStart + leftTreeSize + 1) - preStart) return false;

    if (!canBuildTreeFromPreInHelper(preorder, inorder, preStart + 1, inStart, rootIndex - 1) ||
        !canBuildTreeFromPreInHelper(preorder, inorder, preStart + leftTreeSize + 1, rootIndex + 1, inEnd))
        return false;
    return true;
}
//中序后序检查帮助者
template<typename T>
bool BinaryTree<T>::canBuildTreeFromInPostHelper(const std::vector<T> &inorder, const std::vector<T> &postorder, int inStart, int inEnd, int postIndex)
{
    if (inStart > inEnd || postIndex < 0) return false;
    T root = postorder[postIndex];
    int rootIndex = -1;
    for (int i = inStart; i <= inEnd; ++i) {
        if (inorder[i] == root) {
            rootIndex = i;
            break;
        }
    }
    if (rootIndex == -1) return false; // Root not found in inorder

    int leftTreeSize = rootIndex - inStart;
    if (!canBuildTreeFromInPostHelper(inorder, postorder, inStart, rootIndex - 1, postIndex - leftTreeSize - 1) ||
        !canBuildTreeFromInPostHelper(inorder, postorder, rootIndex + 1, inEnd, postIndex - 1))
        return false;
    return true;
}




