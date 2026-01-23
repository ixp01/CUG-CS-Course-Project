#include "AVL.h"
#include "AVLNode.h"
#include <iostream>
#include <vector>
#include <string>

// AVL 类的构造函数和析构函数实现
AVL::AVL() {}


// 在 AVL 树中插入一个元素
AVLNode* AVL::Insert(AVLNode* node, const std::string &data) {
    if (node == nullptr) {
        return new AVLNode(data);  // 如果节点为空，创建新节点
    }

    // 插入逻辑
    if (data < node->data) {
        node->left = Insert(node->left, data);
    } else if (data > node->data) {
        node->right = Insert(node->right, data);
    } else {
        return node;  // 重复的元素不插入
    }

    // 更新当前节点的高度
    node->height = 1 + std::max(GetHeight(node->left), GetHeight(node->right));

    // 平衡树
    return Balance(node);
}

// 右旋
AVLNode* AVL::RightRotate(AVLNode* y) {
    AVLNode* x = y->left;
    AVLNode* T2 = x->right;

    // 执行旋转
    x->right = y;
    y->left = T2;

    // 更新高度
    y->height = std::max(GetHeight(y->left), GetHeight(y->right)) + 1;
    x->height = std::max(GetHeight(x->left), GetHeight(x->right)) + 1;

    return x;  // 新的根节点
}

// 左旋
AVLNode* AVL::LeftRotate(AVLNode* x) {
    AVLNode* y = x->right;
    AVLNode* T2 = y->left;

    // 执行旋转
    y->left = x;
    x->right = T2;

    // 更新高度
    x->height = std::max(GetHeight(x->left), GetHeight(x->right)) + 1;
    y->height = std::max(GetHeight(y->left), GetHeight(y->right)) + 1;

    return y;  // 新的根节点
}

// 获取节点高度
int AVL::GetHeight(AVLNode* node) {
    return node ? node->height : 0;
}

// 获取平衡因子
int AVL::GetBalanceFactor(AVLNode* node) {
    return node ? GetHeight(node->left) - GetHeight(node->right) : 0;
}

// 平衡树
AVLNode* AVL::Balance(AVLNode* node) {
    int balance = GetBalanceFactor(node);

    // 左子树高
    if (balance > 1) {
        if (GetBalanceFactor(node->left) < 0) {
            node->left = LeftRotate(node->left);
        }
        return RightRotate(node);
    }

    // 右子树高
    if (balance < -1) {
        if (GetBalanceFactor(node->right) > 0) {
            node->right = RightRotate(node->right);
        }
        return LeftRotate(node);
    }

    return node;  // 已经平衡
}

// 查找节点（假设 AVL 树中的每个节点包含一个字符串）
AVLNode* AVL::FindNode(AVLNode* node, const std::string &s) {
    if (node == nullptr || node->data == s) {
        return node;
    }

    if (s < node->data) {
        return FindNode(node->left, s);
    } else {
        return FindNode(node->right, s);
    }
}

// Find 函数实现
void AVL::Find(const std::string &s,
               std::vector<std::string> &vForEnglish,
               std::vector<std::string> &vForChinese,
               std::vector<std::string> &vForPath) {
    // 通过递归方式查找节点
    AVLNode* node = FindNode(root, s);

    if (node) {
        // 假设每个节点的 `data` 对应英文部分，
        // 并且有一些方法获取中文和路径信息（这里只是模拟）

        // 示例：根据节点内容填充英文、中文和路径信息
        vForEnglish.push_back(node->data);  // 英文信息
        vForChinese.push_back("中文_" + node->data);  // 中文信息
        vForPath.push_back("/path/to/" + node->data);  // 假设路径为某些内容

        std::cout << "Found " << s << " in AVL tree!" << std::endl;
    } else {
        std::cout << "Item " << s << " not found in AVL tree!" << std::endl;
    }
}
