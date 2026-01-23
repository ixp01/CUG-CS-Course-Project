#include "mainwindow.h"
#include "ui_mainwindow.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("BinaryTree");
    resize(600,500);

    setWindowFlags(windowFlags()&~Qt::WindowMaximizeButtonHint);    // 禁止最大化按钮
    setFixedSize(this->width(),this->height());                     // 禁止拖动窗口大小
    wid_tree = new TWidget(this);  //初始化二叉树界面
    tree=new BinaryTree<int>();//初始化BinaryTree类型指针
    wid_tree->SetPtr(tree);

}

MainWindow::~MainWindow()
{
    delete ui;
}



