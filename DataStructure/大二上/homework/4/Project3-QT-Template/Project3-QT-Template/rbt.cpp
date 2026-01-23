#include "RBT.h"
#include <iostream>
#include <vector>
#include <string>

RBT::RBT() {
}


void RBT::DeleteTree(RBTNode* node) {
    if (node != TNULL) {
        DeleteTree(node->left);
        DeleteTree(node->right);
        delete node;
    }
}

void RBT::Find(const std::string &s,
               std::vector<std::string> &vForEnglish,
               std::vector<std::string> &vForChinese,
               std::vector<std::string> &vForPath) {
    RBTNode* node = FindNode(root, s);

    if (node != TNULL) {
        // 假设每个节点的 data 对应英文部分，
        // 并且有一些方法获取中文和路径信息（这里只是模拟）

        vForEnglish.push_back(node->data);  // 英文信息
        vForChinese.push_back("中文_" + node->data);  // 中文信息
        vForPath.push_back("/path/to/" + node->data);  // 假设路径为某些内容

        std::cout << "Found " << s << " in Red-Black Tree!" << std::endl;
    } else {
        std::cout << "Item " << s << " not found in Red-Black Tree!" << std::endl;
    }
}

RBTNode* RBT::FindNode(RBTNode* node, const std::string &s) {
    if (node == TNULL || node->data == s) {
        return node;
    }

    if (s < node->data) {
        return FindNode(node->left, s);
    } else {
        return FindNode(node->right, s);
    }
}

void RBT::Insert(const std::string &data) {
    RBTNode* node = new RBTNode(data);
    node->parent = nullptr;
    node->left = TNULL;
    node->right = TNULL;
    node->color = RED;

    RBTNode* y = nullptr;
    RBTNode* x = root;

    // 查找插入位置
    while (x != TNULL) {
        y = x;
        if (node->data < x->data) {
            x = x->left;
        } else {
            x = x->right;
        }
    }

    node->parent = y;

    if (y == nullptr) {
        root = node;  // 如果树为空，插入节点为根节点
    } else if (node->data < y->data) {
        y->left = node;
    } else {
        y->right = node;
    }

    // 插入后需要调整红黑树的平衡
    BalanceInsert(node);
}

void RBT::BalanceInsert(RBTNode* k) {
    FixInsert(k);
}

void RBT::LeftRotate(RBTNode* x) {
    RBTNode* y = x->right;
    x->right = y->left;
    if (y->left != TNULL) {
        y->left->parent = x;
    }
    y->parent = x->parent;
    if (x->parent == nullptr) {
        root = y;
    } else if (x == x->parent->left) {
        x->parent->left = y;
    } else {
        x->parent->right = y;
    }
    y->left = x;
    x->parent = y;
}

void RBT::RightRotate(RBTNode* x) {
    RBTNode* y = x->left;
    x->left = y->right;
    if (y->right != TNULL) {
        y->right->parent = x;
    }
    y->parent = x->parent;
    if (x->parent == nullptr) {
        root = y;
    } else if (x == x->parent->right) {
        x->parent->right = y;
    } else {
        x->parent->left = y;
    }
    y->right = x;
    x->parent = y;
}

void RBT::FixInsert(RBTNode* k) {
    RBTNode* u;
    while (k->parent->color == RED) {
        if (k->parent == k->parent->parent->right) {
            u = k->parent->parent->left;
            if (u->color == RED) {
                // Case 1: Uncle is red
                u->color = BLACK;
                k->parent->color = BLACK;
                k->parent->parent->color = RED;
                k = k->parent->parent;
            } else {
                // Case 2 and 3: Uncle is black
                if (k == k->parent->left) {
                    k = k->parent;
                    RightRotate(k);
                }
                k->parent->color = BLACK;
                k->parent->parent->color = RED;
                LeftRotate(k->parent->parent);
            }
        } else {
            u = k->parent->parent->right;
            if (u->color == RED) {
                u->color = BLACK;
                k->parent->color = BLACK;
                k->parent->parent->color = RED;
                k = k->parent->parent;
            } else {
                if (k == k->parent->right) {
                    k = k->parent;
                    LeftRotate(k);
                }
                k->parent->color = BLACK;
                k->parent->parent->color = RED;
                RightRotate(k->parent->parent);
            }
        }
        if (k == root) {
            break;
        }
    }
    root->color = BLACK;
}
