#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QPushButton>
#include<QStackedWidget>
#include"twidget.h"
#include"binarytree.h"

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
//成员
public:
    Ui::MainWindow *ui;
    QPushButton *btn_tree;//使用二叉树
    TWidget * wid_tree;//调用ui界面指针
    BinaryTree<int>* tree;


};
#endif // MAINWINDOW_H
