#include "bst.h"
#include <algorithm>

// 递归构造平衡树
BST::Node* BST::buildBalancedTree(const std::vector<std::string>& words, const std::vector<std::string>& translations, int start, int end) {
    if (start > end) return nullptr;

    int mid = (start + end) / 2;
    Node* node = new Node(words[mid], translations[mid]);

    node->left = buildBalancedTree(words, translations, start, mid - 1);
    node->right = buildBalancedTree(words, translations, mid + 1, end);

    return node;
}

// 构造函数
BST::BST(const std::vector<std::string>& words, const std::vector<std::string>& translations) {
    root = buildBalancedTree(words, translations, 0, words.size() - 1);
}

// 查找并记录路径
void BST::findHelper(Node* node, const std::string& s, std::vector<std::string>& vForEnglish,
                     std::vector<std::string>& vForChinese, std::vector<std::string>& path) {
    if (!node) return;

    path.push_back(node->word); // 记录路径

    if (s == node->word) {
        vForEnglish.push_back(node->word);
        vForChinese.push_back(node->translation);
    } else if (s < node->word) {
        findHelper(node->left, s, vForEnglish, vForChinese, path);
    } else {
        findHelper(node->right, s, vForEnglish, vForChinese, path);
    }

    // 如果单词前缀匹配，将对应的单词和翻译加入结果
    if (node->word.substr(0, s.size()) == s) {
        vForEnglish.push_back(node->word);
        vForChinese.push_back(node->translation);
    }
}

// 对外的 Find 方法
void BST::Find(std::string s, std::vector<std::string>* vForEnglish, std::vector<std::string>* vForChinese,
               std::vector<std::string>* vforpath) {
    findHelper(root, s, *vForEnglish, *vForChinese, *vforpath);

    // 排序英文单词和对应的中文翻译
    int limit = std::min(10, (int)vForEnglish->size());
    std::vector<std::pair<std::string, std::string>> sortedResults;

    for (int i = 0; i < limit; ++i) {
        sortedResults.push_back({(*vForEnglish)[i], (*vForChinese)[i]});
    }

    std::sort(sortedResults.begin(), sortedResults.end());

    vForEnglish->clear();
    vForChinese->clear();

    for (auto& result : sortedResults) {
        vForEnglish->push_back(result.first);
        vForChinese->push_back(result.second);
    }
}

// 递归销毁树
void BST::clear(Node* node) {
    if (!node) return;
    clear(node->left);
    clear(node->right);
    delete node;
}

// 析构函数
BST::~BST() {
    clear(root);
}
