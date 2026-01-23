#include "tire.h"
#include <QWidget>
#include <QEvent>
#include <QString>
#include <QStringList>
#include <QLabel>
#include <QMessageBox>
#include <QStandardItemModel>
#include <cstdio>
#include <iostream>

Tire::Tire() {
    root = new TireNode();
}

// v1 为英文单词，v2 为相应的中文翻译
Tire::Tire(std::vector<std::string>* v1, std::vector<std::string>* v2) {
    root = new TireNode();
    for (size_t i = 0; i < v1->size(); ++i) {
        insert((*v1)[i], (*v2)[i]);
    }
}

// 插入单词及其翻译
void Tire::insert(const std::string& word, const std::string& translation) {
    TireNode* currentNode = root;
    for (char c : word) {// 遍历单词中的每个字符
        if (currentNode->children.find(c) == currentNode->children.end()) {
            currentNode->children[c] = new TireNode();  // 创建新节点
        }
        currentNode = currentNode->children[c];  // 继续向下
    }
    currentNode->isEndOfWord = true;  // 标记为单词结尾
    currentNode->translation = translation;  // 存储翻译
}
// 前缀匹配递归辅助函数
void Tire::CollectWords(TireNode* node, const std::string& prefix,
                        std::vector<std::string>* vForEnglish,
                        std::vector<std::string>* vForChinese) {
    if (node->isEndOfWord) {
        // 如果当前节点是单词的结束，记录单词和翻译
        vForEnglish->push_back(prefix);
        vForChinese->push_back(node->translation);
    }

    // 遍历所有子节点，递归收集单词
    for (const auto& pair : node->children) {
        CollectWords(pair.second, prefix + pair.first, vForEnglish, vForChinese);
    }
}

// 查找并记录路径，返回是否找到单词
bool Tire::Find(TireNode* node, const std::string& s,
                std::vector<std::string>* vForEnglish,
                std::vector<std::string>* vForChinese,
                std::vector<std::string>* vforpath) {
    TireNode* currentNode = node;
    std::string currentPrefix;// 用于存储当前的前缀
    for (char c : s) {
        // 每次字符对比，计数器加1
        comparisonCount++;
        // 输出当前字符的中间路径
        currentPrefix += c;
        std::cout << "tire middle key: " << comparisonCount << ": " << currentPrefix<< std::endl;
        if (currentNode->children.find(c) == currentNode->children.end()) {
            return false;  // 找不到该路径
        }
        currentNode = currentNode->children[c];  // 继续向下
    }
    // 如果到达一个单词的结尾，记录这个单词
    if (currentNode->isEndOfWord) {
        vForEnglish->push_back(currentPrefix);  // 存储英文单词
        vForChinese->push_back(currentNode->translation);  // 存储翻译
        vforpath->push_back(currentPrefix);  // 存储路径
    }

    // 遍历所有以该前缀为开头的单词
    for (auto& pair : currentNode->children) {
        Find(pair.second, s + pair.first, vForEnglish, vForChinese, vforpath);
    }
    // 收集以当前前缀为开头的所有单词
    CollectWords(currentNode, currentPrefix, vForEnglish, vForChinese);;
    return true;
}

// 查找单词并返回对应的翻译和路径
void Tire::Find(std::string s,
                std::vector<std::string>* vForEnglish,
                std::vector<std::string>* vForChinese,
                std::vector<std::string>* vforpath) {
    vforpath->clear();  // 清空路径记录
    vForEnglish->clear();  // 清空英文单词列表
    vForChinese->clear();  // 清空中文翻译列表
    comparisonCount = 0;  // 重置对比计数器
    Find(root, s, vForEnglish, vForChinese, vforpath);
    // 输出查找过程中进行的对比次数
    std::cout << "tire的对比次数:" << comparisonCount << std::endl;
}
