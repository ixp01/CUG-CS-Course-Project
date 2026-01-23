// #include "avl.h"
// #include <iostream>

// // 构造函数，根据传入的向量数据构建AVL树
// AVL::AVL(std::vector<std::string>* v1, std::vector<std::string>* v2)
// {
//     if (v1!= nullptr && v2!= nullptr) {
//         for (size_t i = 0; i < v1->size(); ++i) {
//             Insert(root,(*v1)[i], (*v2)[i]);
//         }
//     }
// }

// // 析构函数，释放AVL树的所有节点内存
// AVL::~AVL() {
//     DeleteTree(root);
// }

// // 在AVL树中查找指定字符串的函数实现
// void AVL::Find(const std::string& s, std::vector<std::string>& vForEnglish, std::vector<std::string>& vForChinese, std::vector<std::string>& vforpath) {
//     AVLNode* current=root;
//     std::vector<AVLNode*> path;
//     int pathN=0;
//     int count=0;
//     if(current==nullptr) return;
//     while (current!=nullptr) {
//         pathN++;
//         vforpath.push_back(current->en);
//         if(s==current->en.substr(0,s.length()))
//         {
//             if(count<10)
//             {
//                 path.push_back(current);
//                 SORT(path);
//             }
//             else
//             {
//                 if(current->en < path[9]->en)
//                 {
//                     path[9]=current;
//                     SORT(path);
//                 }
//             }
//         }
//         if(current->en<s)
//         {
//             current=current->right;
//         }
//         else if(current->en>s)
//         {
//             current=current->left;
//         }
//         else
//         {
//             for(int i=0;i<path.size();i++)
//             {
//                 vForEnglish.push_back(path[i]->en);
//                 vForChinese.push_back(path[i]->cn);
//             }
//             break;
//         }
//     }
//     for(int i=0;i<vforpath.size();i++)
//     {
//         qDebug() << QString::fromStdString(vforpath[i]);
//     }
//     qDebug() << "AVL查询次数：" << pathN;
// }


// // 递归插入节点函数实现，会进行平衡调整
// AVLNode* AVL::Insert(AVLNode* &root,std::string& enData,std::string& cnData)//插入
// {
//     if(root==nullptr)    //寻找插入位置
//     {
//         root = new AVLNode(enData,cnData);
//     }
//     else if(enData>root->en)    //插入值比当前结点值大，插入到当前结点的右子树上
//     {
//         root->right = Insert(root->right,enData,cnData);
//         if(Height(root->right)-Height(root->left)==2)    //插入后出现失衡
//         {
//             if (enData > root->right->en)    //情况一：插入右子树的右节点，进行左旋
//                 root = LeftRotate(root);
//             else if (enData < root->right->en)    //情况三：插入右子树的左节点,进行先右再左旋转
//                 root = RightLeftRotate(root);
//         }
//     }
//     else if(enData<root->en)    //插入值比当前节点值小，插入到当前结点的左子树上
//     {
//         root->left = Insert(root->left,enData,cnData);
//         if(Height(root->left)-Height(root->right)==2)//如果插入导致失衡
//         {
//             if (enData < root->right->en)
//                 root = RightRotate(root);    //情况二：插入到左子树的左孩子节点上，进行右旋
//             else if (enData > root->right->en)
//                 root = LeftRightRotate(root);    //情况四：插入左子树的右节点,进行先左再右旋转
//         }
//     }
//     return root;
// }

// // 计算节点的高度
// int AVL::Height(AVLNode* node) {
//     if (node == nullptr) {
//         return 0;
//     }
//     int leftHeight = Height(node->left);
//     int rightHeight = Height(node->right);
//     return std::max(leftHeight, rightHeight) + 1;
// }

// // 左旋操作，用于平衡调整
// AVLNode* AVL::LeftRotate(AVLNode* node) {
//     AVLNode* preChild = node->right;
//     node->right=preChild->left;
//     preChild->left=node;
//     node=preChild;

//     return node;
// }

// // 右旋操作，用于平衡调整
// AVLNode* AVL::RightRotate(AVLNode* node) {
//     AVLNode* preChild = node->left;
//     node->left=preChild->right;
//     preChild->right=node;
//     node=preChild;

//     return node;
// }

// // 先左旋再右旋操作，用于平衡调整
// AVLNode* AVL::LeftRightRotate(AVLNode* node) {
//     node->left = LeftRotate(node->left);
//     return RightRotate(node);
// }

// // 先右旋再左旋操作，用于平衡调整
// AVLNode* AVL::RightLeftRotate(AVLNode* node) {
//     node->right = RightRotate(node->right);
//     return LeftRotate(node);
// }

// // 递归删除AVL树节点的函数实现
// void AVL::DeleteTree(AVLNode* node) {
//     if (node!= nullptr) {
//         DeleteTree(node->left);
//         DeleteTree(node->right);
//         delete node;
//     }
// }

// void AVL::SORT(std::vector<AVLNode*>& nodeVector)
// {
//     int n = nodeVector.size();
//     for (int i = 0; i < n - 1; ++i) {
//         for (int j = 0; j < n - i - 1; ++j) {
//             if (nodeVector[j]->en > nodeVector[j + 1]->en) {
//                 std::swap(nodeVector[j], nodeVector[j + 1]);
//             }
//         }
//     }
// }






#include "avl.h"
#include <iostream>
#include <algorithm>

// 构造函数：根据传入的向量数据快速构建AVL树
AVL::AVL(std::vector<std::string>* v1, std::vector<std::string>* v2) {
    if (v1 != nullptr && v2 != nullptr && v1->size() == v2->size()) {
        root = BuildBalancedTree(*v1, *v2, 0, v1->size() - 1);
    }
}

// 析构函数：释放AVL树的所有节点内存
AVL::~AVL() {
    DeleteTree(root);
}

// 快速构建平衡AVL树的函数
AVLNode* AVL::BuildBalancedTree(const std::vector<std::string>& v1, const std::vector<std::string>& v2, int start, int end) {
    if (start > end) return nullptr;

    int mid = (start + end) / 2;  // 找到中间节点
    AVLNode* node = new AVLNode(v1[mid], v2[mid]);

    node->left = BuildBalancedTree(v1, v2, start, mid - 1);
    node->right = BuildBalancedTree(v1, v2, mid + 1, end);

    return node;
}

// 查找函数
void AVL::Find(const std::string& s, std::vector<std::string>& vForEnglish,
               std::vector<std::string>& vForChinese, std::vector<std::string>& vforpath) {
    AVLNode* current = root;
    int pathN = 0;

    while (current != nullptr) {
        pathN++;
        vforpath.push_back(current->en);

        // 检查当前节点是否符合条件
        if (current->en.substr(0, s.length()) == s) {
            vForEnglish.push_back(current->en);
            vForChinese.push_back(current->cn);
        }

        // 根据字典序继续查找
        if (s < current->en) {
            current = current->left;
        } else if (s > current->en) {
            current = current->right;
        } else {
            break;  // 找到完全匹配，停止查找
        }
    }

    // 对结果排序（按英文字母顺序）
    std::vector<std::pair<std::string, std::string>> results;
    for (size_t i = 0; i < vForEnglish.size(); ++i) {
        results.emplace_back(vForEnglish[i], vForChinese[i]);
    }

    std::sort(results.begin(), results.end());

    vForEnglish.clear();
    vForChinese.clear();

    for (const auto& pair : results) {
        vForEnglish.push_back(pair.first);
        vForChinese.push_back(pair.second);
    }

    // 输出路径调试信息
    for (const auto& p : vforpath) {
        qDebug() << QString::fromStdString(p);
    }
    qDebug() << "AVL查询次数：" << pathN;
}

// 删除AVL树节点的函数
void AVL::DeleteTree(AVLNode* node) {
    if (node != nullptr) {
        DeleteTree(node->left);
        DeleteTree(node->right);
        delete node;
    }
}

// 插入节点函数
AVLNode* AVL::Insert(AVLNode* &root, std::string& enData, std::string& cnData) {
    if (root == nullptr) {
        root = new AVLNode(enData, cnData);
    } else if (enData > root->en) {
        root->right = Insert(root->right, enData, cnData);
        if (Height(root->right) - Height(root->left) == 2) {
            if (enData > root->right->en)
                root = LeftRotate(root);
            else
                root = RightLeftRotate(root);
        }
    } else if (enData < root->en) {
        root->left = Insert(root->left, enData, cnData);
        if (Height(root->left) - Height(root->right) == 2) {
            if (enData < root->left->en)
                root = RightRotate(root);
            else
                root = LeftRightRotate(root);
        }
    }
    return root;
}

// 计算节点的高度
int AVL::Height(AVLNode* node) {
    if (node == nullptr) return 0;
    return std::max(Height(node->left), Height(node->right)) + 1;
}

// 左旋操作
AVLNode* AVL::LeftRotate(AVLNode* node) {
    AVLNode* preChild = node->right;
    node->right = preChild->left;
    preChild->left = node;
    return preChild;
}

// 右旋操作
AVLNode* AVL::RightRotate(AVLNode* node) {
    AVLNode* preChild = node->left;
    node->left = preChild->right;
    preChild->right = node;
    return preChild;
}

// 先左旋再右旋操作
AVLNode* AVL::LeftRightRotate(AVLNode* node) {
    node->left = LeftRotate(node->left);
    return RightRotate(node);
}

// 先右旋再左旋操作
AVLNode* AVL::RightLeftRotate(AVLNode* node) {
    node->right = RightRotate(node->right);
    return LeftRotate(node);
}
