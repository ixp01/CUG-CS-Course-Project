#ifndef VIS_WIDGET_H
#define VIS_WIDGET_H

#include <QWidget>
#include <QPainter>
#include <QPen>
#include <cmath>
#include <QString>
#include <QDebug>
#include <string>
#include <QLabel>
#include<QTextBrowser>

class vis_widget : public QWidget
{
    Q_OBJECT
public:
    explicit vis_widget(QWidget *parent = nullptr);


    void buildgraph(std::string);
    void setStep(std::string);
    void nextStep();
    void setText(QString s);
    QPen pen(int x);


    void paintEvent(QPaintEvent *event);
    QPen pen_B,pen_R,pen_G;
    void drawNode(QPoint t,QString s,QPen pen);
    void drawEdge(QPoint t1,QPoint t2,int r,QString s,QPen pen);

    int edge[20][20];//设置边的权值矩阵
    int color_E[20][20];//设置边的颜色矩阵
    int color_N[20];//设置点的颜色
    std::string Sample_graph; int is_sample;//PPT上的例图
    std::vector<int> steps;//存储步骤的容器
    std::vector<int>::iterator Current_step;//当前步骤的指针
    QLabel *Txt;
    QTextBrowser* infoDisplay;//传递输出器指针


    int siz_node,siz_step;
    //
    void updateInfoDisplay(const std::string& info);
    //将图的边权重信息字符串转换为整数数组
    std::vector<int> getnum(std::string s) {
        int x = 0;
        std::vector<int> v;
        for (int i = 0; i < s.length(); i++) {
            if (s[i] >= '0' && s[i] <= '9') {
                while (s[i] >= '0' && s[i] <= '9' && i < s.length()) x = x * 10 + s[i] - '0', i++;
                v.push_back(x);
                x = 0;
            }
        }
        return v;
    }
    void getPtr(QTextBrowser* ptr) {
        infoDisplay = ptr;
    }

signals:
    std::string steppath(int a,int b);
    std::string stepchoose(int a, int b);
};

#endif // VIS_WIDGET_H
