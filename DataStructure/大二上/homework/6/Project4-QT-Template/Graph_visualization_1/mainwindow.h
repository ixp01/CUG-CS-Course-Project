#ifndef MAINWINDOW_H
#define MAINWINDOW_H
using namespace std;
#include <QMainWindow>
#include <QDebug>
#include <QPushButton>
#include "vis_widget.h"
#include <string>
#include <QTimer>
#include <QString>
#include<set>
#include<stack>
#include <queue>
#include <QDebug>
#include<QLabel>
#include<QLineEdit>
#include<QString>
#include<QTextBrowser>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

    ~MainWindow();
    vis_widget *wid_dij,*wid_dfs,*wid_prim,*wid;
    QLabel *beginP;
    QLineEdit *line_beginP;
    void Setwid(std::string s);
    QPushButton *btn_nxt,*btn_auto,*btn_rand,*btn_samp,*btn_stop,*btn_reset;//下一步 自动 随机数据 样例 停止自动演示 重置
    QTimer *timer;
    std::string Sample_graph;
    std::string Cur_graph;//当前图的数据

    QTextBrowser*DFSinfo,*Priminfo,*Dijinfo;


    void nextStep();
    void Init_graph();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
