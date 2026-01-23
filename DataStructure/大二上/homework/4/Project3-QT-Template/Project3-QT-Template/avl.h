#ifndef AVL_H
#define AVL_H

#include <vector>
#include <string>
#include "AVLNode.h"  // 引入定义了 AVLNode 的头文件

class AVL {
public:
    // 构造函数和析构函数声明
    AVL();
    ~AVL();

    // Find 函数声明
    void Find(const std::string &s,
              std::vector<std::string> &vForEnglish,
              std::vector<std::string> &vForChinese,
              std::vector<std::string> &vForPath);

private:
    AVLNode* root;  // 根节点是 AVLNode 类型的指针

    // 辅助函数（例如：插入、平衡、查找等）
    AVLNode* Insert(AVLNode* node, const std::string &data);
    AVLNode* Balance(AVLNode* node);
    AVLNode* RightRotate(AVLNode* y);
    AVLNode* LeftRotate(AVLNode* x);
    int GetHeight(AVLNode* node);
    int GetBalanceFactor(AVLNode* node);
    AVLNode* FindNode(AVLNode* node, const std::string &s);
};

#endif // AVL_H
