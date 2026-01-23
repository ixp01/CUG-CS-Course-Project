#ifndef TWIDGET_H
#define TWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QFont>
#include"binarytree.h"
#include<QGraphicsView>
#include<QGraphicsScene>
#include<QDialog>
#include<QMessageBox>
#include<QString>
#include<QGraphicsEllipseItem>
#include<QGraphicsSimpleTextItem>
class TWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TWidget(QWidget *parent = nullptr);
    ~TWidget();

public:
    //类成员
    BinaryTree<int>* tree;
    vector<int> m_PreOrder;//前序序列
    vector<int> m_InOrder;//中序序列
    vector<int> m_PostOrder;//后序序列
    bool isDrawing; // 是否正在绘制
    int step; // 当前绘制步骤
    int currentPreIndex; // 前序序列当前索引
    int currentInIndex; // 中序序列当前索引
    int currentPostIndex; // 后序序列当前索引
public:

    //ui组件
    QPushButton *btn_inputData; // 输入序列数据的按钮
    QPushButton *btn_creat; // 创建树的按钮
    QPushButton *btn_clear; // 清空树的按钮
    QPushButton *btn_print; // 打印构造过程的按钮
    QPushButton *btn_nextStep; // 打印下一步的按钮
    QPushButton *btn_findParent; // 查找节点父节点的按钮
    QPushButton *btn_lowestCommonAncestor; // 查找最小公共祖先的按钮
    QPushButton *btn_check; // 检查是否可以构建二叉树
    QGraphicsView *treeView; // 用于显示二叉树的视图
    QGraphicsScene *scene;   // 场景，包含二叉树的图形表示
public:
    void initUI();
    void SetPtr(BinaryTree<int>* x);//设置指针
    void refresh();//清空页面
    void deleteTree(TreeNode<int>* node);
    void connects();//执行按键和槽的连接
    bool checkSequences(const std::vector<int>& m_PreOrder, const std::vector<int>& m_InOrder, const std::vector<int>& m_PostOrder);//检查是否能构建二叉树
    void paintEvent(QPaintEvent *event) override; // 重写paintEvent函数
    void drawNextStep(); // 绘制下一步
    void drawBinaryTree(); // 绘制二叉树
    void drawTree(QGraphicsScene *scene, TreeNode<int> *node, QGraphicsItem *parentItem); // 绘制二叉树的函数
    void setDia(QDialog *dia, QLabel *lab1, QPushButton *check, QLineEdit *editPre,QLineEdit *editIn,
                QLineEdit *editPost, QString s_lab, QString title);
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
signals:
    //void someSignal();

public slots:
    //void someSlot();

};

#endif // TWIDGET_H
