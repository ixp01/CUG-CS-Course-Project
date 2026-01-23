#ifndef RBT_H
#define RBT_H

#include <string>
#include <vector>
#include <iostream>

struct RBTNode {
    std::string word;  // 存储单词
    std::string translation;  // 存储翻译
    RBTNode* parent;  // 父节点
    RBTNode* left;  // 左子树
    RBTNode* right;  // 右子树
    bool isRed;  // 红黑树的颜色（红色为true，黑色为false）

    RBTNode(const std::string& w = "", const std::string& t = "", RBTNode* p = nullptr,
            RBTNode* l = nullptr, RBTNode* r = nullptr, bool color = true)
        : word(w), translation(t), parent(p), left(l), right(r), isRed(color) {}
};

class RBT {
public:
    RBT();  // 默认构造函数
    RBT(std::vector<std::string>* v1, std::vector<std::string>* v2);  // 使用单词和翻译初始化树

    // 插入单词及其翻译
    void insert(const std::string& word, const std::string& translation);

    // 查找单词并返回对应的翻译和路径
    void Find(std::string s,
              std::vector<std::string>* vForEnglish,
              std::vector<std::string>* vForChinese,
              std::vector<std::string>* vforpath);

private:
    RBTNode* root;  // 根节点
    RBTNode* TNULL;  // 哨兵节点（树的末端，所有子节点为空）
    int comparisonCount;  // 用于记录比较次数

    // 红黑树修复函数，维护红黑树的性质
    void insertFix(RBTNode* k);

    // 左旋操作
    void leftRotate(RBTNode* x);

    // 右旋操作
    void rightRotate(RBTNode* x);

    // 查找并记录路径，返回是否找到单词
    bool Find(RBTNode* node, const std::string& s,
              std::vector<std::string>* vForEnglish,
              std::vector<std::string>* vForChinese,
              std::vector<std::string>* vforpath);
};

#endif // RBT_H
