#ifndef AVLNODE_H
#define AVLNODE_H

#include <string>

// AVL 树节点定义
struct AVLNode {
    std::string data;  // 存储数据
    AVLNode* left;     // 指向左子树的指针
    AVLNode* right;    // 指向右子树的指针
    int height;        // 该节点的高度

    // 构造函数
    AVLNode(const std::string &d) : data(d), left(nullptr), right(nullptr), height(1) {}
};

#endif // AVLNODE_H
