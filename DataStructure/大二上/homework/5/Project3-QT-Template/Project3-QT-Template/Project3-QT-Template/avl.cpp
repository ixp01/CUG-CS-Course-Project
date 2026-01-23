#include "avl.h"
#include <iostream>
#include <algorithm>

// 构造函数：通过 Insert 逐一插入节点，维持 AVL 平衡
AVL::AVL(std::vector<std::string>* v1, std::vector<std::string>* v2) {
    if (v1 != nullptr && v2 != nullptr && v1->size() == v2->size()) {
        for (size_t i = 0; i < v1->size(); ++i) {
            root = Insert(root, (*v1)[i], (*v2)[i]);
        }
    }
}

// 析构函数：释放AVL树的所有节点内存
AVL::~AVL() {
    DeleteTree(root);
}

// 插入节点函数：插入后调整平衡
AVLNode* AVL::Insert(AVLNode* root, const std::string& enData, const std::string& cnData) {
    if (root == nullptr) {
        return new AVLNode(enData, cnData);
    }

    if (enData < root->en) {
        root->left = Insert(root->left, enData, cnData);
    } else if (enData > root->en) {
        root->right = Insert(root->right, enData, cnData);
    } else {
        return root;  // 不允许插入重复的键
    }

    // 检查平衡因子并进行旋转调整
    int balance = GetBalanceFactor(root);

    // 左重：执行右旋或左右旋
    if (balance < -1) {
        if (enData < root->left->en) {
            return RightRotate(root);
        } else {
            return LeftRightRotate(root);
        }
    }

    // 右重：执行左旋或右左旋
    if (balance > 1) {
        if (enData > root->right->en) {
            return LeftRotate(root);
        } else {
            return RightLeftRotate(root);
        }
    }

    return root;
}

// 获取节点的平衡因子
int AVL::GetBalanceFactor(AVLNode* node) {
    if (!node) return 0;
    return Height(node->right) - Height(node->left);
}

// 计算节点的高度
int AVL::Height(AVLNode* node) {
    if (node == nullptr) return 0;
    return std::max(Height(node->left), Height(node->right)) + 1;
}

// 左旋操作
AVLNode* AVL::LeftRotate(AVLNode* node) {
    AVLNode* newRoot = node->right;
    node->right = newRoot->left;
    newRoot->left = node;
    return newRoot;
}

// 右旋操作
AVLNode* AVL::RightRotate(AVLNode* node) {
    AVLNode* newRoot = node->left;
    node->left = newRoot->right;
    newRoot->right = node;
    return newRoot;
}

// 先左旋再右旋操作
AVLNode* AVL::LeftRightRotate(AVLNode* node) {
    node->left = LeftRotate(node->left);
    return RightRotate(node);
}

// 先右旋再左旋操作
AVLNode* AVL::RightLeftRotate(AVLNode* node) {
    node->right = RightRotate(node->right);
    return LeftRotate(node);
}

void AVL::Find(const std::string& s, std::vector<std::string>& vForEnglish,
               std::vector<std::string>& vForChinese, std::vector<std::string>& vforpath) {
    AVLNode* current = root;
    int pathN = 0;

    while (current != nullptr) {
        pathN++;
        vforpath.push_back(current->en);

        // 检查当前节点是否符合条件
        if (current->en.substr(0, s.length()) == s) {
            vForEnglish.push_back(current->en);
            vForChinese.push_back(current->cn);
        }

        // 根据字典序继续查找
        if (s < current->en) {
            current = current->left;
        } else if (s > current->en) {
            current = current->right;
        } else {
            break;  // 找到完全匹配，停止查找
        }
    }

    // 对结果排序（按英文字母顺序），并限制排序的元素数目
    int limit = std::min(10, (int)vForEnglish.size());  // 限制最多10个元素
    std::vector<std::pair<std::string, std::string>> results;

    for (int i = 0; i < limit; ++i) {
        results.emplace_back(vForEnglish[i], vForChinese[i]);
    }

    std::sort(results.begin(), results.end());

    vForEnglish.clear();
    vForChinese.clear();

    for (const auto& pair : results) {
        vForEnglish.push_back(pair.first);
        vForChinese.push_back(pair.second);
    }

    // 输出路径调试信息
    // for (const auto& p : vforpath) {
    //     qDebug() << QString::fromStdString(p);
    // }

    qDebug() << "AVL查询次数：" << pathN;
}

// 删除AVL树节点的函数
void AVL::DeleteTree(AVLNode* node) {
    if (node != nullptr) {
        DeleteTree(node->left);
        DeleteTree(node->right);
        delete node;
    }
}
