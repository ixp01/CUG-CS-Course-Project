#ifndef BST_H
#define BST_H

#include <vector>
#include <string>
#include <iostream>
#include <QDebug>

class BST {
private:
    struct Node {
        std::string word;    // 英文单词
        std::string translation;  // 中文翻译
        Node* left;
        Node* right;

        Node(const std::string& w, const std::string& t) : word(w), translation(t), left(nullptr), right(nullptr) {}
    };

    Node* root;
    int x=0;//记录匹配次数

    // 递归构造平衡树
    Node* buildBalancedTree(const std::vector<std::string>& words, const std::vector<std::string>& translations, int start, int end);

    // 查找并记录路径
    void findHelper(Node* node, const std::string& s, std::vector<std::string>& vForEnglish,
                    std::vector<std::string>& vForChinese, std::vector<std::string>& path);

public:
    // 构造函数
    BST(const std::vector<std::string>& words, const std::vector<std::string>& translations);

    // 查找方法
    void Find(std::string s, std::vector<std::string>* vForEnglish, std::vector<std::string>* vForChinese,
              std::vector<std::string>* vforpath);

    // 销毁树
    void clear(Node* node);
    ~BST();
};

#endif // BST_H
