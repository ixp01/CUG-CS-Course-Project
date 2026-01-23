#ifndef RBT_H
#define RBT_H

#include <vector>
#include <string>
#include<RBTNode.h>

// 红黑树节点定义

class RBT {
public:
    RBT();                // 构造函数
    ~RBT();               // 析构函数

    void Find(const std::string &s,
              std::vector<std::string> &vForEnglish,
              std::vector<std::string> &vForChinese,
              std::vector<std::string> &vForPath);  // 查找节点

    void Insert(const std::string &data);  // 插入数据

private:
    RBTNode* root;   // 根节点
    RBTNode* TNULL;  // 哨兵节点

    // 辅助函数：旋转、平衡、查找等
    void BalanceInsert(RBTNode* k);
    void LeftRotate(RBTNode* x);
    void RightRotate(RBTNode* x);
    void FixInsert(RBTNode* k);
    RBTNode* FindNode(RBTNode* node, const std::string &s);
    void DeleteTree(RBTNode* node);  // 清理树资源
};

#endif // RBT_H
