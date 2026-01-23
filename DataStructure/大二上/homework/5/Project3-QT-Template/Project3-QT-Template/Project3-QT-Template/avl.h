
#ifndef AVL_H
#define AVL_H

#include <vector>
#include <string>
#include <QStack>
#include <QDebug>

// AVL树节点结构体
struct AVLNode {
    std::string en;
    std::string cn;
    AVLNode* left;
    AVLNode* right;

    AVLNode(const std::string& enData, const std::string& cnData)
        : en(enData), cn(cnData), left(nullptr), right(nullptr) {}
};

class AVL {
public:
    AVL(std::vector<std::string>* v1, std::vector<std::string>* v2);
    ~AVL();

    // 在AVL树中查找指定字符串的函数
    void Find(const std::string& s, std::vector<std::string>& vForEnglish,
              std::vector<std::string>& vForChinese, std::vector<std::string>& vforpath);

private:
    AVLNode* root = nullptr;

    // 插入节点函数
    AVLNode* Insert(AVLNode* root, const std::string& enData, const std::string& cnData);

    // 计算节点的高度
    int Height(AVLNode* node);

    // 左旋操作
    AVLNode* LeftRotate(AVLNode* node);

    // 右旋操作
    AVLNode* RightRotate(AVLNode* node);

    // 先左旋再右旋操作
    AVLNode* LeftRightRotate(AVLNode* node);

    // 先右旋再左旋操作
    AVLNode* RightLeftRotate(AVLNode* node);

    // 递归删除AVL树节点的函数
    void DeleteTree(AVLNode* node);

    // 获取节点平衡因子
    int GetBalanceFactor(AVLNode* node);
};

#endif // AVL_H
