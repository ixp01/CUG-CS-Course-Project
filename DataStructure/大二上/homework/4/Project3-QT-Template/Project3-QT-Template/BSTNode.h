#ifndef BSTNODE_H
#define BSTNODE_H
#include<qstring.h>
#endif // BSTNODE_H
class BSTNode {
public:
    QString English;
    QString Chinese;
    QString path;
    BSTNode *left;
    BSTNode *right;

    BSTNode(const QString &param1, const QString &param2, const QString &param3)
        : English(param1), Chinese(param2), path(param3) {
        // 构造函数实现
    }
};
