#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include<QGraphicsView>
#include<QGraphicsScene>
#include"TreeNode.h"
#include<QTextEdit>
#include<QGraphicsEllipseItem>
#include<QGraphicsLineItem>
#include<QGraphicsItem>
#include<QMessageBox>
#include<vector>
template class TreeNode<int>;//模板显式实例化
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public:

    int step=0;//用于每次绘制二叉树时，判断绘制步骤
    int flag=0;//用于下一步按键的计时
    TreeNode<int> *PreIn;
    TreeNode<int> *PrePost;
    TreeNode<int> *InPost;
    TreeNode<int> *tempPtr=nullptr;//全局变量用于某些单个函数的指示
    bool bugflag =false;
    // QGraphicsView *treeView; // 用于显示二叉树的视图
    // QGraphicsScene *scene;   // 场景，包含二叉树的图形表示

public://成员函数
    //获取lineEdit中的数据，将string类型变成一个个int类型数字
    std::vector<int> getnum(std::string s) {
        int x = 0;
        std::vector<int> v;
        for (std::string::size_type i = 0; i < s.length(); i++) {
            if (s[i] >= '0' && s[i] <= '9') {
                while (s[i] >= '0' && s[i] <= '9' && i < s.length()) {
                    x = x * 10 + s[i] - '0';
                    i++;
                }
                v.push_back(x);
                x = 0;
            }
        }
        return v;
    }
    //通过前序中序建立二叉树
    TreeNode<int>* buildTree(const std::vector<int>& preOrder, const std::vector<int>& inOrder);

    //通过中序后序建立二叉树
    TreeNode<int>* buildTreeFromInPost(const std::vector<int>& inOrder, const std::vector<int>& postOrder);

    //通过前序后序建立二叉树
    TreeNode<int>* buildTreeFromPrePost(const std::vector<int>& preOrder, const std::vector<int>& postOrder, int preStart, int preEnd, int postStart, int postEnd);

    //打印二叉树
    void printTreePI(QTextEdit* textEdit, TreeNode<int>* node, const QString& prefix = "");

    //重写paintEvent函数
    void paintEvent(QPaintEvent *event) override;

    //绘制二叉树
    void drawTree( QPainter &painter, TreeNode<int> *node, int x, int y, int hSpacing, int vSpacing);

    // 寻找父母节点的函数
    TreeNode<int>* findParent(TreeNode<int>* root, int value);

    //用二叉链寻找最小共同祖先
    TreeNode<int>* findLCA_BinaryChain(TreeNode<int>* root, int p, int q);

    //用三叉链寻找最小共同祖先
    TreeNode<int>* findLCA_TernaryChain(TreeNode<int>* root, int p, int q);

    // 构建节点映射
    void buildNodeMap(TreeNode<int>* node, std::unordered_map<int, TreeNode<int>*>& nodeMap);

    // 获取从根到节点的路径
    std::vector<TreeNode<int>*> getPath(const std::unordered_map<int, TreeNode<int>*>& nodeMap, int value);

    // 找到节点的父节点的值
    int findParentValue(const std::unordered_map<int, TreeNode<int>*>& nodeMap, TreeNode<int>* node);

    //寻找父节点的辅助函数
    TreeNode<int>* findParentHelper(TreeNode<int>* root, int value);

    // // 使用三叉链查找最小公共祖先
    // TreeNode<int>* findLCA_WithParentPointersIterative(TreeNode<int>* nodeP, TreeNode<int>* nodeQ);

    // // 计算节点的深度
    // int depth(TreeNode<int>* node);

    // // 获得对应指针
    // TreeNode<int>* findNode(TreeNode<int>* root, int value);

//槽函数
private slots:
    void on_Btn_PreIn_clicked();

    void on_Btn_PrePost_clicked();

    void on_Btn_InPost_clicked();

    void on_Btn_PreInNext_clicked();

    void on_Btn_PrePostNext_clicked();

    void on_Btn_InPostNext_clicked();

    void on_Btn_findParent_clicked();

    void on_Btn_findAncestor_clicked();

    void on_Btn_clear_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
