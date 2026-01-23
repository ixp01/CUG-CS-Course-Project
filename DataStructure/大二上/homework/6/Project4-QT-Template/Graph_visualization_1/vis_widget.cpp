#include "vis_widget.h"
#include <cstring>

vis_widget::vis_widget(QWidget *parent) : QWidget(parent)
{
    pen_B.setColor(Qt::black);pen_B.setWidth(2);//设置笔颜色
    pen_R.setColor(Qt::red);pen_R.setWidth(2);
    pen_G.setColor(Qt::green);pen_G.setWidth(2);
    //将一块内存区域的内容设置为指定的值 指针 字符 容器大小
    std::memset(edge,0,sizeof(edge));//图的权重（矩阵）
    std::memset(color_E,0,sizeof(color_E));//边的颜色状态（矩阵）
    std::memset(color_N,0,sizeof(color_N));//点的颜色状态
    siz_node=siz_step=0;//节点个数，步骤步数

    Txt=new QLabel(this);
    Txt->move(0,300);
    Txt->resize(300,100);
    Txt->setFont(QFont("Microsoft YaHei", 16, 75));
    Txt->setAlignment(Qt::AlignCenter);
    //设置样本矩阵数据 点的个数 边的内容
    Sample_graph="8 0 1 680 0 2 1380 0 3 730 0 4 530 1 2 2600 1 5 960 2 3 2500 3 6 840 4 5 820 4 6 570 4 7 650 5 7 500 6 7 1140";
    is_sample=0;
    resize(300,400);
    //    repaint();
}

void vis_widget::buildgraph(std::string s)//构建图的结构。通过字符串表示的图信息，初始化图的节点、边及其权重，设置图的状态。
{
    std::memset(edge,0,sizeof(edge));
    std::memset(color_E,0,sizeof(color_E));
    std::memset(color_N,0,sizeof(color_N));
    siz_node=siz_step=0;
    steps.clear();
    std::vector<int> v=getnum(s);//获取全部的数字
    siz_node=v[0];//获取节点个数
    for (int i=1;i<v.size();i+=3) {
        edge[v[i]][v[i+1]]=v[i+2];//设置两点之间的权值
        color_E[v[i]][v[i+1]]=1;//初始化两点之间边的颜色
        edge[v[i+1]][v[i]]=v[i+2];//由于是矩阵，所以对称位置也需要设置
        color_E[v[i+1]][v[i]]=1;
    }
    for  (int i=1;i<=siz_node;i++) {
        color_N[i]=0;//将节点颜色设为空
    }
    if  (s==Sample_graph) is_sample=1; else is_sample=0;//如果是样本图，就设置成样本内容
    update();
}

void vis_widget::setStep(std::string s)//设置步骤序列。设置prim dfs dijkstra
{
    steps=getnum(s);//s为自身输入的步骤句子
    Current_step=steps.begin();
}

void vis_widget::nextStep()//执行下一步操作，按照设置的步骤序列动态更新图的状态（如改变边的颜色等）。
{// 函数根据 steps 中的指令逐步更新边的颜色。每个步骤由三个数字表示：起点、终点、边的类型（颜色）。颜色值（1, 2, 3）分别对应不同的边颜色（黑色、红色、绿色）。
    if (steps.empty() || Current_step==steps.end()) return;
    int n=*Current_step; Current_step++;//获得当前步骤
    while (n--) {
        if (Current_step+2>steps.end()) return ;//代表是最后一步
        int s=*Current_step,t=*(Current_step+1),type=*(Current_step+2);//current_step里面装的是颜色
        color_E[s][t]=type;color_E[t][s]=type;//修改边的颜色

        Current_step+=3;
    }

    update();
}


// void vis_widget::nextStep() {
//     if (steps.empty() || Current_step == steps.end()) return;
//     int n = *Current_step; Current_step++; // 获得当前步骤的数量
//     std::string info;

//     while (n--) {
//         int s = *Current_step, t = *(Current_step + 1), type = *(Current_step + 2);
//         color_E[s][t] = type; color_E[t][s] = type; // 更新边的颜色
//         Current_step += 3;

//         // 构建待选边和被选择边的信息，并转换 QString 为 std::string
//         info += QString::number(s) + "->" + QString::number(t) + "\n"; // 直接追加边信息
//     }

//     // 输出当前步骤的所有边信息
//     infoDisplay->append(QString::fromStdString(info));

//     // 如果还有更多步骤，继续执行
//     if (Current_step != steps.end()) {
//         updateInfoDisplay(); // 调用更新信息显示的方法
//     }

//     update(); // 重绘图形
// }

// // 添加一个新的方法来更新信息显示
// void vis_widget::updateInfoDisplay() {
//     std::string info;
//     // 假设我们有一个方法来获取当前所有待选边和被选择边的信息
//     // 这里只是一个示例，您需要根据实际情况来实现这个方法
//     info = getCurrentEdgesInfo();

//     // 将信息转换为 QString 并显示在 QTextBrowser 中
//     infoDisplay->append(QString::fromStdString(info));
// }
// void vis_widget::nextStep() {
//     if (Current_step == steps.end()) return;

//     int n = *Current_step; Current_step++;
//     std::string info;
//     while (n--) {
//         int s = *Current_step, t = *(Current_step + 1), type = *(Current_step + 2);
//         color_E[s][t] = type; color_E[t][s] = type; // 更新边的颜色
//         Current_step += 3;

//         // 构建待选边和被选择边的信息，并转换 QString 为 std::string
//         info += (QString::number(s) + "->" + QString::number(t) + "\n").toStdString();
//     }

//     // 更新QTextBrowser显示
//     updateInfoDisplay(info);

//     update(); // 重绘图形
// }


void vis_widget::setText(QString s)//设置界面上显示的文本（通常用于显示当前状态或提示信息）。
{
    Txt->setText(s);
}

QPen vis_widget::pen(int x)//根据传入的 x 值返回不同的画笔（用于边的颜色和节点的颜色）。
{
    if (x==2) return pen_R;
    if (x==3) return pen_G;
    return pen_B;
}

void vis_widget::paintEvent(QPaintEvent *event)
{
    int x=150,y=150,r=130;
    int n=siz_node;
    if (n==0) return;
    double angle=0.0;
    std::vector<QPoint> v;
    if (is_sample) {
        v.push_back(QPoint(150,170));
        v.push_back(QPoint(240,160));
        v.push_back(QPoint(150,270));
        v.push_back(QPoint(30,190));
        v.push_back(QPoint(130,100));
        v.push_back(QPoint(230,80));
        v.push_back(QPoint(50,110));
        v.push_back(QPoint(150,30));
        for (int i=0;i<n;i++) {
            drawNode(v[i],QString::number(i),(!color_N[i])?pen_B:pen_R);
        }
    }
    else
    for (int i=0;i<n;i++) {
//        qDebug() << QPoint(double(x)+r*cos(angle),double(y)+r*sin(angle));
        v.push_back(QPoint(double(x)+r*cos(angle),double(y)+r*sin(angle)));
        drawNode(QPoint(double(x)+r*cos(angle),double(y)+r*sin(angle)),QString::number(i),(!color_N[i])?pen_B:pen_R);
        angle+=2*acos(-1)/double(n);
    }
    for (int i=0;i<n;i++) {
        for (int j=i+1;j<n;j++)  {
            if (color_E[i][j]) {
                drawEdge(v[i],v[j],10,QString::number(edge[i][j]),pen(color_E[i][j]));
            }
        }
    }
}

void vis_widget::drawNode(QPoint t,QString s,QPen pen)
{
    QPainter paint(this);
    paint.setPen(pen);
    //paint.setRenderHint(QPainter::HighQualityAntialiasing);
    paint.drawEllipse(t,10,10);
    paint.drawText(t+QPoint(-5,3),s);
}

void vis_widget::drawEdge(QPoint t1, QPoint t2, int r,QString s,QPen pen)
{
    QPoint tmp=t2-t1;

    QPainter paint(this);
    paint.setPen(pen);
    //paint.setRenderHint(QPainter::HighQualityAntialiasing);

    paint.drawText((tmp*(1.0-(siz_node+1)/2/double(siz_node+1)))+t1,s);

    double angle=atan(fabs((double)tmp.y()/(double)tmp.x()));
    int x=double(r+r/2)*cos(angle);
    int y=double(r+r/2)*sin(angle);

//生成对应边，其中注释内容为箭头标识，可表示有向边
    if  (tmp.x()>=0 && tmp.y()>0) {
         t1.rx()+=x;t1.ry()+=y;
         t2.rx()-=x;t2.ry()-=y;
         paint.drawLine(t1,t2);
//         paint.drawLine(t2+QPoint(-r*cos(angle-acos(-1)/6),-r*sin(angle-acos(-1)/6)),t2);
//         paint.drawLine(t2+QPoint(-r*cos(angle+acos(-1)/6),-r*sin(angle+acos(-1)/6)),t2);
    }
    if  (tmp.x()<0 && tmp.y()>=0) {
         t1.rx()-=x;t1.ry()+=y;
         t2.rx()+=x;t2.ry()-=y;
         paint.drawLine(t1,t2);
//         paint.drawLine(t2+QPoint(+r*cos(angle-acos(-1)/6),-r*sin(angle-acos(-1)/6)),t2);
//         paint.drawLine(t2+QPoint(+r*cos(angle+acos(-1)/6),-r*sin(angle+acos(-1)/6)),t2);
    }
    if  (tmp.x()<=0 && tmp.y()<0) {
        t1.rx()-=x;t1.ry()-=y;
        t2.rx()+=x;t2.ry()+=y;
        paint.drawLine(t1,t2);
//        paint.drawLine(t2+QPoint(+r*cos(angle-acos(-1)/6),+r*sin(angle-acos(-1)/6)),t2);
//        paint.drawLine(t2+QPoint(+r*cos(angle+acos(-1)/6),+r*sin(angle+acos(-1)/6)),t2);
    }
    if  (tmp.x()>0 && tmp.y()<=0) {
        t1.rx()+=x;t1.ry()-=y;
        t2.rx()-=x;t2.ry()+=y;
        paint.drawLine(t1,t2);
//        paint.drawLine(t2+QPoint(-r*cos(angle-acos(-1)/6),+r*sin(angle-acos(-1)/6)),t2);
//        paint.drawLine(t2+QPoint(-r*cos(angle+acos(-1)/6),+r*sin(angle+acos(-1)/6)),t2);
    }
    //qDebug() <<  tmp/2 << s;
}

// void vis_widget::updateInfoDisplay(const std::string &info)
// {
//     if (infoDisplay) {
//         infoDisplay->append(QString::fromStdString(info));
//     }
// }
