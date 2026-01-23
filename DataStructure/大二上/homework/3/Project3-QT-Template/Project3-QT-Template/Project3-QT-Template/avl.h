// #ifndef AVL_H
// #define AVL_H

// #include <vector>
// #include <string>
// #include <QStack>
// #include <QDebug>

// // AVL树节点结构体
// struct AVLNode {
//     std::string en;
//     std::string cn;
//     AVLNode* left;
//     AVLNode* right;

//     AVLNode(const std::string& enData, const std::string& cnData)
//         :en(enData), cn(cnData), left(nullptr), right(nullptr) {}
// };

// class AVL {
// public:
//     AVL(std::vector<std::string>* v1, std::vector<std::string>* v2);
//     ~AVL();

//     // 在AVL树中查找指定字符串的函数
//     void Find(const std::string& s, std::vector<std::string>& vForEnglish, std::vector<std::string>& vForChinese, std::vector<std::string>& vforpath);

// private:
//     AVLNode* root=nullptr;

//     void SORT(std::vector<AVLNode*>& nodeVector);

//     // 插入节点函数，会进行平衡调整
//     AVLNode* Insert(AVLNode* &root,std::string& enData,std::string& cnData);

//     // 计算节点的高度
//     int Height(AVLNode* node);

//     // 左旋操作，用于平衡调整
//     AVLNode* LeftRotate(AVLNode* node);

//     // 右旋操作，用于平衡调整
//     AVLNode* RightRotate(AVLNode* node);

//     // 先左旋再右旋操作，用于平衡调整
//     AVLNode* LeftRightRotate(AVLNode* node);

//     // 先右旋再左旋操作，用于平衡调整
//     AVLNode* RightLeftRotate(AVLNode* node);

//     // 递归删除AVL树节点的函数
//     void DeleteTree(AVLNode* node);
// };

// #endif // AVL_H



#ifndef AVL_H
#define AVL_H

#include <vector>
#include <string>
#include <QStack>
#include <QDebug>

// AVL树节点结构体
struct AVLNode {
    std::string en;
    std::string cn;
    AVLNode* left;
    AVLNode* right;

    AVLNode(const std::string& enData, const std::string& cnData)
        : en(enData), cn(cnData), left(nullptr), right(nullptr) {}
};

class AVL {
public:
    AVL(std::vector<std::string>* v1, std::vector<std::string>* v2);
    ~AVL();

    // 在AVL树中查找指定字符串的函数
    void Find(const std::string& s, std::vector<std::string>& vForEnglish,
              std::vector<std::string>& vForChinese, std::vector<std::string>& vforpath);

private:
    AVLNode* root = nullptr;

    // 用于快速构建平衡AVL树
    AVLNode* BuildBalancedTree(const std::vector<std::string>& v1, const std::vector<std::string>& v2, int start, int end);

    // 插入节点函数
    AVLNode* Insert(AVLNode* &root, std::string& enData, std::string& cnData);

    // 计算节点的高度
    int Height(AVLNode* node);

    // 左旋操作
    AVLNode* LeftRotate(AVLNode* node);

    // 右旋操作
    AVLNode* RightRotate(AVLNode* node);

    // 先左旋再右旋操作
    AVLNode* LeftRightRotate(AVLNode* node);

    // 先右旋再左旋操作
    AVLNode* RightLeftRotate(AVLNode* node);

    // 递归删除AVL树节点的函数
    void DeleteTree(AVLNode* node);
};

#endif // AVL_H
