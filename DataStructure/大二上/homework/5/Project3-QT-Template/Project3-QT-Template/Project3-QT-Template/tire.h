#ifndef TIRE_H
#define TIRE_H

#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>

// Tire节点定义
struct TireNode {
    std::unordered_map<char, TireNode*> children; // 孩子节点（字符到TireNode的映射）
    bool isEndOfWord; // 标记是否为单词的结尾
    std::string translation; // 存储单词对应的中文翻译

    // 构造函数初始化
    TireNode() : isEndOfWord(false), translation("") {}
};

class Tire {
public:
    Tire();  // 默认构造函数
    Tire(std::vector<std::string>* v1, std::vector<std::string>* v2);  // 根据给定单词和翻译构造树

    // 插入单词及其翻译
    void insert(const std::string& word, const std::string& translation);

    // 查找单词并返回对应的翻译和路径
    void Find(std::string s,
              std::vector<std::string>* vForEnglish,
              std::vector<std::string>* vForChinese,
              std::vector<std::string>* vforpath);

private:
    TireNode* root;  // 根节点
    int comparisonCount = 0;  // 用于记录比较次数

    // 前缀匹配递归辅助函数
    void CollectWords(TireNode* node, const std::string& prefix,
                      std::vector<std::string>* vForEnglish,
                      std::vector<std::string>* vForChinese);

    // 查找并记录路径，返回是否找到单词
    bool Find(TireNode* node, const std::string& s,
              std::vector<std::string>* vForEnglish,
              std::vector<std::string>* vForChinese,
              std::vector<std::string>* vforpath);
};

#endif // TIRE_H
