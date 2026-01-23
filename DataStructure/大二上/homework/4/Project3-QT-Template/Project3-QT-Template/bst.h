#ifndef BST_H
#define BST_H

#include <QString>
#include <vector>
#include <string>
#include<BSTNode.h>
class BST {

public:
    BST();
    ~BST();

    void Insert(const QString &English, const QString &Chinese, const QString &path);
    void Find(const std::string &s, std::vector<std::string> &vForEnglish,
              std::vector<std::string> &vForChinese, std::vector<std::string> &vForPath);

private:
    BSTNode *root;

    BSTNode* InsertRec(BSTNode *node, const QString &English, const QString &Chinese, const QString &path);
    void FindRec(BSTNode *node, const QString &s, std::vector<std::string> &vForEnglish,
                 std::vector<std::string> &vForChinese, std::vector<std::string> &vForPath);
    void DeleteTree(BSTNode *node);
};

#endif // BST_H
