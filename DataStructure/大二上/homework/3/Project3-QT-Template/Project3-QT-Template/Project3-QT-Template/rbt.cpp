#include "rbt.h"
#include <QWidget>
#include <QEvent>
#include <QString>
#include <QStringList>
#include <QLabel>
#include <QMessageBox>
#include <QStandardItemModel>
#include <cstdio>
#include <iostream>

RBT::RBT() {
    TNULL = new RBTNode();
    TNULL->isRed = false;// 哨兵节点为黑色
    root = TNULL;
}
RBT::RBT(std::vector<std::string>* v1,std::vector<std::string>* v2){//构造函数3
    TNULL = new RBTNode();
    TNULL->isRed = false;// 哨兵节点为黑色
    root = TNULL;
    for (size_t i = 0; i < v1->size(); ++i) {
        insert(v1->at(i), v2->at(i));  // 从两个容器中插入数据
    }
}

void RBT::insert(const std::string& word, const std::string& translation){//插入函数
    RBTNode* node = new RBTNode(word, translation, nullptr, TNULL, TNULL, true);  // 新节点是红色
    RBTNode* y = nullptr;  // 用于保存父节点
    RBTNode* x = root;     // 从根节点开始查找插入位置
    // 寻找插入位置：在树中找到一个合适的位置来插入新节点
    while (x != TNULL) {  // 遍历树直到找到一个空位置
        y = x;  // 保存当前节点作为父节点
        if (node->word < x->word) {  // 如果新节点的词汇小于当前节点的词汇
            x = x->left;  // 向左子树继续查找
        } else {
            x = x->right;  // 向右子树继续查找
        }
    }
    // 插入新节点到正确的位置
    node->parent = y;  // 将新节点的父节点指向找到的父节点
    if (y == nullptr) {  // 如果树为空（即根节点为空）
        root = node;  // 新节点成为根节点
    } else if (node->word < y->word) {  // 如果新节点的词汇小于父节点的词汇
        y->left = node;  // 新节点插入到父节点的左子树
    } else {
        y->right = node;  // 新节点插入到父节点的右子树
    }
    // 插入新节点后，调用修复函数维护红黑树的性质
    insertFix(node);
}

void RBT::insertFix(RBTNode* k) {
    RBTNode* u;

    // 双红调整（父节点是红色）
    while (k->parent != nullptr && k->parent->isRed) {
        if (k->parent == k->parent->parent->left) {//父节点是祖父节点的左子树
            u = k->parent->parent->right;  // 叔父节点
            if (u != nullptr && u->isRed) {
                // Case 1: 叔父节点是红色（父祖换色）
                k->parent->isRed = false;//父亲黑色
                u->isRed = false;//叔父黑色
                k->parent->parent->isRed = true;//祖父红色
                k = k->parent->parent;  // 向上调整
            } else {//叔父节点是黑色
                if (k == k->parent->right) {
                    // Case 2: 插入节点是右子节点LR
                    k = k->parent;
                    leftRotate(k);//局部结构标准化为下面Case 3 LL的形态
                }
                // Case 3: 插入节点是左子节点LL
                k->parent->isRed = false;
                k->parent->parent->isRed = true;
                rightRotate(k->parent->parent);
            }
        } else {//父节点是祖父节点的右子树
            u = k->parent->parent->left;  // 叔夫节点
            if (u != nullptr && u->isRed) {
                // Case 1: 叔父节点是红色（父祖换色）
                k->parent->isRed = false;//父亲黑色
                u->isRed = false;//叔叔黑色
                k->parent->parent->isRed = true;//祖父红色
                k = k->parent->parent;  // 向上调整
            } else {//叔父结点是黑色
                if (k == k->parent->left) {
                    // Case 2: 插入节点是左子节点RL
                    k = k->parent;
                    rightRotate(k);//局部结构标准化为下面Case 3 RR的形态
                }
                // Case 3: 插入节点是右子节点RR
                k->parent->isRed = false;
                k->parent->parent->isRed = true;
                leftRotate(k->parent->parent);
            }
        }
        // 如果当前节点已经是根节点，停止修复
        if (k == root) {
            break;
        }
    }
    // 确保根节点为黑色
    root->isRed = false;
}

// 左旋操作
void RBT::leftRotate(RBTNode* x) {
    RBTNode* y = x->right;
    x->right = y->left;
    if (y->left != TNULL) {
        y->left->parent = x;
    }
    y->parent = x->parent;
    if (x->parent == nullptr) {
        root = y;//原来x是根结点，左旋后y是根节点
    } else if (x == x->parent->left) {
        x->parent->left = y;
    } else {
        x->parent->right = y;
    }
    y->left = x;//x左旋成为y的左子树
    x->parent = y;
}

// 右旋操作
void RBT::rightRotate(RBTNode* x) {
    RBTNode* y = x->left;
    x->left = y->right;
    if (y->right != TNULL) {
        y->right->parent = x;
    }
    y->parent = x->parent;
    if (x->parent == nullptr) {
        root = y;//原来x是根结点，右旋后y是根节点
    } else if (x == x->parent->right) {
        x->parent->right = y;
    } else {
        x->parent->left = y;
    }
    y->right = x;//x右旋成为y的右子树
    x->parent = y;
}



bool RBT::Find(RBTNode* node, const std::string& s, std::vector<std::string>* vForEnglish,
               std::vector<std::string>* vForChinese, std::vector<std::string>* vforpath) {
    if (node == nullptr) {
        return false;  // 没有找到
    }

    // 每次进行一个对比，计数器加1
    comparisonCount++;
    std::cout << "Comparison " << comparisonCount << ": Comparing '" << node->word << "' with target prefix '" << s << "'... ";

    // 将当前节点的单词添加到路径中
    vforpath->push_back(node->word);

    // 检查当前节点单词是否以s为前缀
    if (node->word.substr(0, s.size()) == s) {
        // 如果是前缀匹配，则将其加入结果列表
        vForEnglish->push_back(node->word);
        vForChinese->push_back(node->translation);
    }

    // 如果当前节点的单词字典序大于前缀s，左子树可能包含匹配项
    if (s < node->word) {
        std::cout << "Target prefix is smaller, searching left subtree." << std::endl;
        // 在左子树查找
        Find(node->left, s, vForEnglish, vForChinese, vforpath);
    }
    // 如果当前节点的单词字典序小于前缀s，右子树可能包含匹配项
    else if (s > node->word) {
        std::cout << "Target prefix is greater, searching right subtree." << std::endl;
        // 在右子树查找
        Find(node->right, s, vForEnglish, vForChinese, vforpath);
    }
    else {
        // 如果当前节点的单词与前缀s相同，我们仍然需要递归遍历两个子树
        std::cout << "Prefix matches node word, continue searching both subtrees." << std::endl;
        Find(node->left, s, vForEnglish, vForChinese, vforpath);
        Find(node->right, s, vForEnglish, vForChinese, vforpath);
    }

    return true;
}

void RBT::Find(std::string s, std::vector<std::string>* vForEnglish, std::vector<std::string>* vForChinese, std::vector<std::string>* vforpath){
    vforpath->clear();  // 清空路径记录
    vForEnglish->clear();  // 清空英文单词列表
    vForChinese->clear();  // 清空中文翻译列表
    comparisonCount = 0;  // 重置对比计数器
    // 从根节点开始查找
    Find(root, s, vForEnglish, vForChinese, vforpath);
    // 输出查找过程中关键字对比的次数
    std::cout << "rbt key comprision times: " << comparisonCount << std::endl;
}
