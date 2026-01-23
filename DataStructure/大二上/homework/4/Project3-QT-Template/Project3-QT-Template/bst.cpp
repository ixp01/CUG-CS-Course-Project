#include "BST.h"
#include <QString>
#include <string>
#include <vector>
#include <iostream>


BST::BST() {};


// 删除树中的所有节点
void BST::DeleteTree(BSTNode *node) {
    if (node != nullptr) {
        DeleteTree(node->left);
        DeleteTree(node->right);
        delete node;
    }
}

// 插入节点
void BST::Insert(const QString &English, const QString &Chinese, const QString &path) {
    root = InsertRec(root, English, Chinese, path);
}

// 插入节点的递归实现
BSTNode* BST::InsertRec(BSTNode *node, const QString &English, const QString &Chinese, const QString &path) {
    if (node == nullptr) {
        return new BSTNode(English, Chinese, path);
    }

    if (English < node->English) {
        node->left = InsertRec(node->left, English, Chinese, path);
    } else {
        node->right = InsertRec(node->right, English, Chinese, path);
    }

    return node;
}

// 查找节点并返回匹配结果
void BST::Find(const std::string &s, std::vector<std::string> &vForEnglish,
               std::vector<std::string> &vForChinese, std::vector<std::string> &vForPath) {
    vForEnglish.clear();
    vForChinese.clear();
    vForPath.clear();
    FindRec(root, QString::fromStdString(s), vForEnglish, vForChinese, vForPath);
}

// 查找递归
void BST::FindRec(BSTNode *node, const QString &s,
                  std::vector<std::string> &vForEnglish,
                  std::vector<std::string> &vForChinese,
                  std::vector<std::string> &vForPath) {
    if (node == nullptr) {
        return;
    }

    // 如果当前节点的 English 字段与目标字符串匹配，加入结果向量
    if (node->English.contains(s, Qt::CaseInsensitive)) {
        vForEnglish.push_back(node->English.toStdString());
        vForChinese.push_back(node->Chinese.toStdString());
        vForPath.push_back(node->path.toStdString());
    }

    // 继续搜索左子树和右子树
    if (s < node->English) {
        FindRec(node->left, s, vForEnglish, vForChinese, vForPath);
    } else {
        FindRec(node->right, s, vForEnglish, vForChinese, vForPath);
    }
}
