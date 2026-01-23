#ifndef RBNODE_H
#define RBNODE_H

#include <string>

// 红黑树节点定义
enum Color { RED, BLACK };

struct RBNode {
    std::string data;  // 存储数据
    RBNode* left;      // 指向左子树的指针
    RBNode* right;     // 指向右子树的指针
    RBNode* parent;    // 指向父节点的指针
    Color color;       // 节点的颜色

    RBNode(const std::string &d)
        : data(d), left(nullptr), right(nullptr), parent(nullptr), color(RED) {}
};

#endif // RBNODE_H
