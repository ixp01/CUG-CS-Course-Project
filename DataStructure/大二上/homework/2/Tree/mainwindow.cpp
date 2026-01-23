#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    //初始化ui
    ui->setupUi(this);
    setWindowFlags(windowFlags()&~Qt::WindowMaximizeButtonHint);    // 禁止最大化按钮
    setFixedSize(this->width(),this->height());                     // 禁止拖动窗口大小
}

MainWindow::~MainWindow()
{
    delete ui;
}

//信号和槽
//前中构建二叉树-----------------------------------------------------------------------------------------------------------

void MainWindow::on_Btn_PreIn_clicked()
{
    std::string s1 = ui->line_Pre->text().toStdString();
    std::vector<int> tmp1 = getnum(s1);
    std::string s2 = ui->line_In->text().toStdString();
    std::vector<int> tmp2 = getnum(s2);
    bugflag=false;//判断是否出现bug
    //当容器里数据为空或者两个容器内的数据长度不等的情况下，报错
    if(tmp1.size()!=tmp2.size())
    {
        QMessageBox::warning(this, "警告", "无法构建二叉树:两边序列数不一样");
        return;
    }
    PreIn = buildTree(tmp1, tmp2);//构建二叉树
    ui->textEdit_PreIn->clear();//先将显示的组件清零
    printTreePI(ui->textEdit_PreIn, PreIn);//调用打印二叉树的函数
    if(bugflag){QMessageBox::warning(this,"warning","建立过程中出现了错误");}
    bugflag=false;
    flag=0;//将标记置为0

}

//前序中序建立二叉树
TreeNode<int> *MainWindow::buildTree(const std::vector<int> &preOrder, const std::vector<int> &inOrder)
{
    // 检查前序或中序序列是否为空
    if (preOrder.empty() || inOrder.empty()) {
        return nullptr;
    }

    int rootValue = preOrder[0]; // 首先将前序第一个元素置为根节点的值
    TreeNode<int>* root = new TreeNode<int>(rootValue); // 将根节点初始化

    size_t inRootIndex = 0; // 设置中序遍历中根节点
    for (inRootIndex = 0; inRootIndex < inOrder.size(); ++inRootIndex) {
        if (inOrder[inRootIndex] == rootValue) break;
    }

    // 如果在中序序列中没有找到根节点，返回 nullptr
    if (inRootIndex == inOrder.size()) {
        delete root; // 释放已分配的根节点内存
        bugflag=true;
        return nullptr;
    }

    size_t leftTreeSize = inRootIndex;//根节点索引就是左子树数量

    // 递归构建左子树
    if (leftTreeSize > 0) { // 确保左子树的子序列不为空
        root->left = buildTree({preOrder.begin() + 1, preOrder.begin() + 1 + leftTreeSize},
                               {inOrder.begin(), inOrder.begin() + inRootIndex});
    }

    // 递归构建右子树
    if (inRootIndex + 1 < inOrder.size()) { // 确保右子树的子序列不为空
        root->right = buildTree({preOrder.begin() + 1 + leftTreeSize, preOrder.end()},
                                {inOrder.begin() + inRootIndex + 1, inOrder.end()});
    }

    // 更新父节点指针
    if (root->left) root->left->parent = root;
    if (root->right) root->right->parent = root;

    return root;
}


//用前序中序打印二叉树
void MainWindow::printTreePI(QTextEdit *textEdit, TreeNode<int> *node, const QString &prefix)
{
    if (!node) return;

    // 构建当前节点的字符串表示
    QString nodeStr;
    nodeStr += QString::number(node->value);
    nodeStr += " 左：";
    if (node->left) {
        nodeStr += QString::number(node->left->value);//加入左节点的值
    } else {
        nodeStr += "null";
    }
    nodeStr += ",右：";
    if (node->right) {
        nodeStr += QString::number(node->right->value);//加入右节点的值
    } else {
        nodeStr += "null";
    }

    // 将当前节点的字符串表示添加到textEdit中
    textEdit->append(prefix + nodeStr);

    // 递归地打印左子树和右子树
    printTreePI(textEdit, node->left, prefix + "    ");
    printTreePI(textEdit, node->right, prefix + "    ");
}

//下一步
void MainWindow::on_Btn_PreInNext_clicked()
{

    flag++; // 每次点击按钮，flag增加
    tempPtr=PreIn;
    update(); // 触发重绘，这将调用paintEvent

}

//前后构建二叉树------------------------------------------------------------------------------------------------------------------------------------------------------------

void MainWindow::on_Btn_PrePost_clicked()
{
    //获取lineedit上的数据
    std::string s1 = ui->line_Pre->text().toStdString();
    std::vector<int> tmp1 = getnum(s1);
    std::string s2 = ui->line_Post->text().toStdString();
    std::vector<int> tmp2 = getnum(s2);
    //起始检查函数
    if(tmp1.size()!=tmp2.size())
    {
        QMessageBox::warning(this, "警告", "无法构建二叉树:两边序列数不一样");
        return;
    }

    PrePost = buildTreeFromPrePost(tmp1, tmp2, 0, tmp1.size() - 1, 0, tmp2.size() - 1); // 假设PrePost是存储根节点指针的成员变量
    ui->textEdit_PrePost->clear(); // 清空文本编辑器
    printTreePI(ui->textEdit_PrePost, PrePost);//调用打印二叉树的函数
}

//用前序后序建立二叉树
TreeNode<int> *MainWindow::buildTreeFromPrePost(const std::vector<int> &preOrder, const std::vector<int> &postOrder, int preStart, int preEnd, int postStart, int postEnd)
{
    // 递归终止条件：如果前序序列的起始索引大于结束索引，或者后序序列的起始索引大于结束索引，返回空
    if (preStart > preEnd || postStart > postEnd) {
        return NULL;
    }

    // 根节点是前序序列的第一个元素，即preOrder[preStart]
    int rootVal = preOrder[preStart];
    // 创建一个新的TreeNode节点作为根节点
    TreeNode<int>* root = new TreeNode<int>(rootVal);

    // 如果前序序列中只有一个节点，即根节点，直接返回该根节点
    if (preStart == preEnd) {
        return root;
    }

    // 查找后序序列中根节点右子树的第一个节点，即左子树的根节点
    int leftSubtreeRootVal = preOrder[preStart + 1]; // 前序序列中，根节点之后的第一个节点是左子树的根节点
    int leftSubtreeRootPosInPost = -1;
    // 在后序序列中查找左子树的根节点 左子树根节点的右边则为右子树
    for (int i = postStart; i <= postEnd; ++i) {
        if (postOrder[i] == leftSubtreeRootVal) {
            leftSubtreeRootPosInPost = i;
            break;
        }
    }

    // 如果在后序序列中没有找到左子树的根节点，显示错误信息并返回空
    if (leftSubtreeRootPosInPost == -1) {
        QMessageBox::critical(this, "错误", "前序与后序序列错误！");
        return NULL;
    }

    // 计算左子树的节点数量
    int leftSize = leftSubtreeRootPosInPost - postStart + 1;

    // 递归构建左子树
    root->left = buildTreeFromPrePost(preOrder, postOrder, preStart + 1, preStart + leftSize, postStart, leftSubtreeRootPosInPost);
    // 递归构建右子树
    root->right = buildTreeFromPrePost(preOrder, postOrder, preStart + leftSize + 1, preEnd, leftSubtreeRootPosInPost + 1, postEnd - 1);

    // 设置左子树和右子树的父节点指针为当前根节点
    if (root->left) root->left->parent = root;
    if (root->right) root->right->parent = root;

    // 返回构建好的根节点
    return root;

}

//下一步
void MainWindow::on_Btn_PrePostNext_clicked()
{
    flag++; // 每次点击按钮，flag增加
    tempPtr=PrePost;
    update(); // 触发重绘，这将调用paintEvent
}

//中后构建二叉树------------------------------------------------------------------------------------------------------------------------------------------------------------

void MainWindow::on_Btn_InPost_clicked() {
    //获得lineedit上的数据
    std::string s1 = ui->line_In->text().toStdString();
    std::vector<int> tmp1 = getnum(s1);
    std::string s2 = ui->line_Post->text().toStdString();
    std::vector<int> tmp2 = getnum(s2);

    if (tmp1.empty() || tmp2.empty() || tmp1.size() != tmp2.size()) {
        QMessageBox::warning(this, "警告", "无法构建二叉树");
        return;
    }
    bugflag=false;
    InPost = buildTreeFromInPost(tmp1, tmp2);

    ui->textEdit_InPost->clear(); // 清空文本编辑器
    printTreePI(ui->textEdit_InPost, InPost);//调用打印二叉树的函数
    if(bugflag){QMessageBox::warning(this,"warning","建立过程中出现了错误");}
    bugflag=false;
    flag=0;
}

//通过中序后序建立二叉树
TreeNode<int>* MainWindow::buildTreeFromInPost(const std::vector<int>& inOrder, const std::vector<int>& postOrder) {
    if (inOrder.empty() || postOrder.empty()) return nullptr;

    int rootValue = postOrder.back();//获得根节点的值
    TreeNode<int>* root = new TreeNode<int>(rootValue);//++创建头节点

    size_t inRootIndex = 0;//设置在中序序列中的索引
    for (inRootIndex = 0; inRootIndex < inOrder.size(); ++inRootIndex) {
        if (inOrder[inRootIndex] == rootValue) break;
    }


    // 如果在中序序列中没有找到根节点，返回 nullptr
    if (inRootIndex == inOrder.size()) {
        delete root; // 释放已分配的根节点内存
        bugflag=true;
        return nullptr;
    }

    size_t leftTreeSize = inRootIndex;//根节点索引就是左子树数量

    root->left = buildTreeFromInPost({inOrder.begin(), inOrder.begin() + inRootIndex},
                                     {postOrder.begin(), postOrder.begin() + inRootIndex});//传入左边序列的中序序列和后序序列
    root->right = buildTreeFromInPost({inOrder.begin() + inRootIndex + 1, inOrder.end()},
                                      {postOrder.begin() + inRootIndex, postOrder.end() - 1});//传入右边序列的中序序列和后序序列

    // 更新父节点指针
    if (root->left) root->left->parent = root;
    if (root->right) root->right->parent = root;

    return root;
}



//下一步
void MainWindow::on_Btn_InPostNext_clicked()
{
    flag++; // 每次点击按钮，flag增加
    tempPtr=InPost;
    update(); // 触发重绘，这将调用paintEvent
}




//-------------------------------------------------------------------------------------------------

//寻找父节点-------------------------------------------------------------------------------------------------------------------------------------------------
void MainWindow::on_Btn_findParent_clicked()
{
    std::string s1 = ui->line_findParent->text().toStdString();
    std::vector<int> tmp1 = getnum(s1);
    int value = tmp1[0]; // 假设tmp1[0]是要查找的节点的值

    TreeNode<int>* parent = findParent(tempPtr, value);
    if (parent) {
        QMessageBox::information(this, "找到父节点", "找到节点 " + QString::number(value) + " 的父节点，值为 " + QString::number(parent->value));
    } else {
        QMessageBox::warning(this, "未找到父节点", "未找到节点 " + QString::number(value) + " 的父节点");
    }
}

//寻找父节点的函数
TreeNode<int> *MainWindow::findParent(TreeNode<int> *root, int value)
{
    if (!root) return nullptr;

    // 从根节点开始，递归遍历左子树和右子树
    return findParentHelper(root, value);//查找函数
}

//寻找父节点的辅助函数
TreeNode<int> *MainWindow::findParentHelper(TreeNode<int> *root, int value)
{
    if (!root) return nullptr;

    // 如果当前节点的左子节点或右子节点值为目标值，则当前节点是其父节点
    if ((root->left && root->left->value == value) || (root->right && root->right->value == value)) {
        return root;
    }

    // 递归搜索左子树
    TreeNode<int>* leftParent = findParentHelper(root->left, value);
    if (leftParent) return leftParent;

    // 递归搜索右子树
    return findParentHelper(root->right, value);
}


//寻找最小祖先----------------------------------------------------------------------------------------------------------------------------------------------------
void MainWindow::on_Btn_findAncestor_clicked()
{
    std::string s1 = ui->line_findAncestor->text().toStdString();
    std::vector<int> tmp1 = getnum(s1);
    int p = tmp1[0];
    int q = tmp1[1];

    // 使用二叉链方法查找 LCA
    TreeNode<int>* lcaBinary = findLCA_BinaryChain(tempPtr, p, q);
    // 使用三叉链方法查找 LCA
    TreeNode<int>* lcaTernary = findLCA_TernaryChain(tempPtr, p, q);

    // // 使用迭代比较法查找 LCA
    // TreeNode<int>* lcaIterative = findLCA_WithParentPointersIterative(findNode(tempPtr, p), findNode(tempPtr, q));

    QMessageBox::information(this, "LCA (Binary Chain)", "The LCA of nodes " + QString::number(p) + " and " + QString::number(q) + " using binary chain is: " +
                                                             QString::number(lcaBinary ? lcaBinary->value : -1));

    QMessageBox::information(this, "LCA (Ternary Chain)", "The LCA of nodes " + QString::number(p) + " and " + QString::number(q) +
                                                              " using ternary chain is: " + QString::number(lcaTernary ? lcaTernary->value : -1));

    // QMessageBox::information(this, "LCA (Iterative Comparison)", "The LCA of nodes " +
    //                                                                  QString::number(p) + " and " + QString::number(q) +
    //                                                                  " using iterative comparison is: " + QString::number(lcaIterative ? lcaIterative->value : -1));
}



// 使用二叉链方法查找 LCA
TreeNode<int> *MainWindow::findLCA_BinaryChain(TreeNode<int> *root, int p, int q)
{
    // 如果当前节点为空，或者当前节点的值等于p或q，返回当前节点 递归终止条件
    if (!root || root->value == p || root->value == q) {
        return root;
    }

    // 递归地在当前节点的左子树上查找p和q
    TreeNode<int>* left = findLCA_BinaryChain(root->left, p, q);

    // 递归地在当前节点的右子树上查找p和q
    TreeNode<int>* right = findLCA_BinaryChain(root->right, p, q);

    // 如果左子树和右子树的递归查找都找到了p和q，说明p和q分别在当前节点的两侧
    if (left && right) {
        return root; // 返回当前节点作为LCA
    }

    // 如果p和q都在当前节点的同一侧，返回非空的一侧的查找结果
    // 如果left非空，则q在左子树中，如果right非空，则p在右子树中
    return left ? left : right;
}


// 使用三叉链方法查找 LCA
TreeNode<int>* MainWindow::findLCA_TernaryChain(TreeNode<int>* root, int p, int q) {
    // 创建一个映射，用于存储每个节点值到其对应的TreeNode对象的映射
    std::unordered_map<int, TreeNode<int>*> nodeMap;
    buildNodeMap(root, nodeMap); // 递归构建节点映射，包含每个节点的父节点指针

    // 使用构建的映射，获取从根节点到节点 p 的路径
    std::vector<TreeNode<int>*> pathP = getPath(nodeMap, p);
    // 使用构建的映射，获取从根节点到节点 q 的路径
    std::vector<TreeNode<int>*> pathQ = getPath(nodeMap, q);

    // 初始化LCA为根节点，从根节点开始向上查找第一个不同的节点
    TreeNode<int>* lca = root;
    for (int i = 0; i < std::min(pathP.size(), pathQ.size()); ++i) {
        // 如果在对应位置上的节点不同，说明找到了LCA
        if (pathP[i] != pathQ[i]) {
            return lca;
        }
        // 如果相同，更新LCA为该节点
        lca = pathP[i];
    }
    // 如果p和q在同一路径上，返回路径上的最后一个共同节点
    return lca;
}

// 构建节点映射
void MainWindow::buildNodeMap(TreeNode<int>* node, std::unordered_map<int, TreeNode<int>*>& nodeMap) {
    if (!node) return;
    nodeMap[node->value] = node;
    buildNodeMap(node->left, nodeMap);
    buildNodeMap(node->right, nodeMap);
}

// 获取从根到节点的路径
std::vector<TreeNode<int>*> MainWindow::getPath(const std::unordered_map<int, TreeNode<int>*>& nodeMap, int value) {
    // 创建一个空路径，用于存储从根节点到目标节点的路径
    std::vector<TreeNode<int>*> path;
    // 循环，直到value为-1（表示到达根节点）
    while (value != -1) {
        // 将当前节点添加到路径的末尾
        path.push_back(nodeMap.at(value));
        // 查找当前节点的父节点的值
        value = findParentValue(nodeMap, path.back());
    }
    // 反转路径，因为我们是从目标节点向根节点方向构建的路径
    std::reverse(path.begin(), path.end());
    // 返回构建的路径
    return path;
}

// 找到节点的父节点的值
int MainWindow::findParentValue(const std::unordered_map<int, TreeNode<int>*>& nodeMap, TreeNode<int>* node) {
    // 遍历节点映射，找到当前节点的父节点
    for (const auto& pair : nodeMap) {
        // 如果当前节点是某个节点的左子节点或右子节点，返回那个节点的值
        if (pair.second->left == node || pair.second->right == node) {
            return pair.first;
        }
    }
    // 如果没有找到父节点，返回-1（表示当前节点是根节点）
    return -1;
}

//第二种使用三叉链的方法，使用迭代比较算法-------------------------------------------------------------------------------------------------------------

// TreeNode<int> *MainWindow::findLCA_WithParentPointersIterative(TreeNode<int> *nodeP, TreeNode<int> *nodeQ)
// {
//     while (nodeP != nodeQ) {
//         if (depth(nodeP) > depth(nodeQ)) {
//             nodeP = nodeP->parent;
//         } else {
//             nodeQ = nodeQ->parent;
//         }
//     }
//     return nodeP; // 当nodeP和nodeQ相等时，它们就是LCA
// }

// int MainWindow::depth(TreeNode<int> *node)
// {
//     int d = 0;
//     TreeNode<int>* current = node;
//     while (current->parent != nullptr) { // 确保 parent 不为空
//         d++;
//         current = current->parent;
//     }
//     return d;
// }

// TreeNode<int> *MainWindow::findNode(TreeNode<int> *root, int value)
// {
//     if (root == nullptr) return nullptr;
//     if (root->value == value) return root;
//     TreeNode<int>* leftResult = findNode(root->left, value);
//     if (leftResult != nullptr) return leftResult;
//     return findNode(root->right, value);
// }



//绘制图像--------------------------------------------------------------------------------------------------------------------

void MainWindow::paintEvent(QPaintEvent *event) {
    QWidget::paintEvent(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true); // 设置抗锯齿
    QPen pen(Qt::black, 2, Qt::SolidLine);
    painter.setPen(pen); // 设置QPainter的笔触
    //scene->clear(); // 清空场景中的所有项

    //tempPtr = PreIn; // 设置tempPtr指向PreIn，这样drawTree将绘制PreIn指向的树
    step = 0; // 重置step为0，每次重绘时开始计数
    drawTree( painter, tempPtr, 750, 40, 50, 50); // 从根节点开始绘制
}

void MainWindow::drawTree( QPainter &painter, TreeNode<int> *node, int x, int y, int hSpacing, int vSpacing) {
    if (!node) return;

    if (step == flag) return; // 如果step等于flag，停止绘制

    // 绘制当前节点
    painter.setBrush(QBrush(Qt::lightGray));
    painter.drawEllipse(x - 10, y - 10, 20, 20); // 绘制节点为圆形
    painter.setPen(QPen(Qt::black));
    painter.drawText(x, y + 15, QString::number(node->value)); // 绘制节点值

    step++; // 每次递归调用之前增加step
    // 绘制左子树
    if (node->left && step != flag) {
        qreal childX = x - hSpacing;
        qreal childY = y + vSpacing;
        painter.drawLine(x, y, childX, childY); // 画线连接父节点和子节点
        drawTree( painter, node->left, childX, childY, hSpacing / 2, vSpacing);
    }

    // 绘制右子树
    if (node->right && step != flag) {
        qreal childX = x + hSpacing;
        qreal childY = y + vSpacing;
        painter.drawLine(x, y, childX, childY); // 画线连接父节点和子节点
        drawTree( painter, node->right, childX, childY, hSpacing / 2, vSpacing);
    }

}



//清除按键的槽函数----------------------------------------------------------------------------------------------------------------

void MainWindow::on_Btn_clear_clicked()
{
    // 清除场景中的所有项
    //scene->clear();

    // 重置step和flag
    step = 0;
    flag = 0;
    update();

}

