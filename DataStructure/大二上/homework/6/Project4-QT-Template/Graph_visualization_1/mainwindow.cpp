#include "mainwindow.h"
#include "ui_mainwindow.h"

#define ran(x) ((rand()*RAND_MAX%(x)+rand())%(x))

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    resize(1000,600);
    setWindowTitle("Graph visualization");
    srand(time(NULL));
//    wid=new vis_widget(this);
//    wid->move(0,0);
//    wid->buildgraph("7 0 1 28 0 5 10 1 2 16 1 6 14 2 3 12 3 6 18 3 4 22 4 6 24 5 4 25");
//    wid->setStep("3 0 1 2 1 6 3 1 2 3 4 1 2 1 1 6 2 6 4 3 6 3 3");

    timer=new QTimer();
    timer->setInterval(1000 * 1);//定义计时器为1s

    Sample_graph="8 0 1 680 0 2 1380 0 3 730 0 4 530 1 2 2600 1 5 960 2 3 2500 3 6 840 4 5 820 4 6 570 4 7 650 5 7 500 6 7 1140";//定义了样本信息
    Cur_graph=Sample_graph;
    wid_dfs=new vis_widget(this);wid_dfs->setText("DFS");
    wid_prim=new vis_widget(this);wid_prim->setText("Prim");
    wid_dij=new vis_widget(this);wid_dij->setText("Dijkstra");
    wid_dfs->move(20,0);
    wid_prim->move(350,0);
    wid_dij->move(680,0);
    Init_graph();

    btn_rand=new QPushButton(this);btn_rand->setText("随机数据");btn_rand->resize(100,50);
    btn_samp=new QPushButton(this);btn_samp->setText("样例数据");btn_samp->resize(100,50);
    btn_nxt=new QPushButton(this);btn_nxt->setText("单步演示");btn_nxt->resize(100,50);
    btn_auto=new QPushButton(this);btn_auto->setText("自动演示");btn_auto->resize(100,50);
    btn_stop=new QPushButton(this);btn_stop->setText("停止自动演示");btn_stop->resize(100,50);
    btn_reset=new QPushButton(this);btn_reset->setText("重置");btn_reset->resize(100,50);

    DFSinfo=new QTextBrowser(this);
    Priminfo=new QTextBrowser(this);
    Dijinfo=new QTextBrowser(this);

    DFSinfo->move(70,600);DFSinfo->setFixedSize(200,160);DFSinfo->setText("DFS遍历顺序");
    Priminfo->move(400,600);Priminfo->setFixedSize(200,160);Priminfo->setText("Prim遍历顺序");
    Dijinfo->move(730,600);Dijinfo->setFixedSize(250,160);Dijinfo->setText("Dij遍历顺序");

    btn_samp->move(350,430);btn_rand->move(550,430);
    btn_reset->move(225,510);btn_nxt->move(225+150,510);btn_auto->move(225+300,510);btn_stop->move(225+450,510);

    beginP=new QLabel(this);
    beginP->setText("起始点");
    beginP->move(225+600,480);

    line_beginP=new QLineEdit(this);
    line_beginP->move(225+580,530);

    connect(timer,&QTimer::timeout,this,[=](){
        nextStep();
    });

    connect(btn_samp,&QPushButton::clicked,this,[=](){
        Cur_graph=Sample_graph;
        Init_graph();
    });

    connect(btn_rand,&QPushButton::clicked,this,[=](){
        std::string s;
        int n=ran(4)+5;//节点个数最小为5，最大为8
        int m=ran(n)+n;//边数最多为2n-1
        s+=std::to_string(n)+" ";
        bool tag[n][n];
        memset(tag,0,sizeof(tag));
        for (int i=0;i<n;i++) tag[i][i]=1;
        for (int i=0;i<m;i++) {
            int x=0,y=0,dis=ran(20)+1;
            while (tag[x][y]) x=ran(n),y=ran(n);
            s+=std::to_string(x)+" "+std::to_string(y)+" "+std::to_string(dis)+" ";
        }
        Cur_graph=s;//更新Cur_graph为新生成的数据
//        Cur_graph="5 0 1 1 0 2 1 0 3 1 1 4 1 ";
        Init_graph();
    });

    connect(btn_reset,&QPushButton::clicked,this,[=](){
        Init_graph();
    });

    connect(btn_nxt,&QPushButton::clicked,this,[=](){
        nextStep();
        timer->stop();
    });

    connect(btn_auto,&QPushButton::clicked,this,[=](){
        timer->start();
    });

    connect(btn_stop,&QPushButton::clicked,this,[=](){
        timer->stop();
    });


    /////

    qDebug() << this->x() << this->y();
}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::nextStep()//所有图同时下一步
{
    wid_dfs->nextStep();
    wid_prim->nextStep();
    wid_dij->nextStep();
}

void MainWindow::Init_graph()//建立所有图
{
    wid_dfs->buildgraph(Cur_graph);
    wid_prim->buildgraph(Cur_graph);
    wid_dij->buildgraph(Cur_graph);


    std::string s_dfs,s_prim,s_dij;
//    s_dfs=DFS(Cur_graph).Steps();
//    qDebug() << QString::fromStdString(s_dfs);


//实现要求的三个算法,根据Cur_graph中的图信息建图。
//同时,在运行三个算法运行的同时生成其步骤信息，将三个算法对应的步骤信息分别放到s_dfs,s_prim,s_dij中，以实现动态展示
//自己设计步骤信息的生成方式，只要生成的步骤信息满足README中的格式要求，能正常展示算法流程即可。
//You code here!!!!!!!!!!!

//已有的内容 edge color_E color_N

//1 2 3 黑 红 绿

//DFS-----------------------------------------------------------------------------------------------------------------------------------------------------
    // 存储步骤字符串
    std::string steps_dfs;

    // 用于 DFS 的栈和访问标记
    std::stack<int> dfsStack;
    std::vector<bool> visited_dfs(wid_dfs->siz_node, false);

    // 用于存储当前步骤信息
    std::vector<int> temp_dfs;

    // 起始节点压入栈
    dfsStack.push(0);
    visited_dfs[0] = true;

    while (!dfsStack.empty()) {
        int currentNode = dfsStack.top();
        bool foundNext = false;
        std::vector<int> greenEdges; // 用于存储绿色边

        // 遍历邻接节点
        for (int i = 0; i < wid_dfs->siz_node; ++i) {
            if (wid_dfs->edge[currentNode][i] > 0 && !visited_dfs[i]) {
                // 将备选边标记为绿色
                temp_dfs.push_back(currentNode);
                temp_dfs.push_back(i);
                temp_dfs.push_back(3); // 绿色

                greenEdges.push_back(currentNode);
                greenEdges.push_back(i);

                foundNext = true;
            }
        }

        if (foundNext) {
            // 添加步骤信息：备选边变绿色
            steps_dfs += std::to_string(temp_dfs.size() / 3) + " ";
            for (size_t j = 0; j < temp_dfs.size(); j += 3) {
                steps_dfs += std::to_string(temp_dfs[j]) + " " +
                             std::to_string(temp_dfs[j + 1]) + " " +
                             std::to_string(temp_dfs[j + 2]) + " ";
            }
            steps_dfs += "\n";

            // 清空临时存储
            temp_dfs.clear();

            // 选择权值最小的边作为红色
            int minWeight = INT_MAX;
            int selectedU = -1, selectedV = -1;
            for (size_t j = 0; j < greenEdges.size(); j += 2) {
                int u = greenEdges[j], v = greenEdges[j + 1];
                if (wid_dfs->edge[u][v] < minWeight) {
                    minWeight = wid_dfs->edge[u][v];
                    selectedU = u;
                    selectedV = v;
                }
            }

            // 备选边变黑色
            for (size_t j = 0; j < greenEdges.size(); j += 2) {
                int u = greenEdges[j], v = greenEdges[j + 1];
                temp_dfs.push_back(u);
                temp_dfs.push_back(v);
                temp_dfs.push_back(1); // 黑色
            }

            // 选中的边变红色
            temp_dfs.push_back(selectedU);
            temp_dfs.push_back(selectedV);
            temp_dfs.push_back(2); // 红色

            // 添加步骤信息：边变色
            steps_dfs += std::to_string(temp_dfs.size() / 3) + " ";
            for (size_t j = 0; j < temp_dfs.size(); j += 3) {
                steps_dfs += std::to_string(temp_dfs[j]) + " " +
                             std::to_string(temp_dfs[j + 1]) + " " +
                             std::to_string(temp_dfs[j + 2]) + " ";
            }
            steps_dfs += "\n";

            // 清空临时存储
            temp_dfs.clear();

            // 压入栈并标记访问
            dfsStack.push(selectedV);//将下一个点压入栈中
            visited_dfs[selectedV] = true;

        } else {
            // 回退节点时，标记与当前节点相关的绿色边为黑色
            dfsStack.pop();

            for (int i = 0; i < wid_dfs->siz_node; ++i) {
                if (wid_dfs->edge[currentNode][i] > 0 && !visited_dfs[i]) {
                    temp_dfs.push_back(currentNode);
                    temp_dfs.push_back(i);
                    temp_dfs.push_back(1); // 黑色
                }
            }

            // 添加步骤信息
            steps_dfs += std::to_string(temp_dfs.size() / 3) + " ";
            for (size_t j = 0; j < temp_dfs.size(); j += 3) {
                steps_dfs += std::to_string(temp_dfs[j]) + " " +
                             std::to_string(temp_dfs[j + 1]) + " " +
                             std::to_string(temp_dfs[j + 2]) + " ";
            }
            steps_dfs += "\n";

            // 清空临时存储
            temp_dfs.clear();
        }
    }


//Prim-----------------------------------------------------------------------------------------------------------------------------------

    std::string steps_prim;        // 存储步骤字符串
    std::set<int> mstSet;          // 存储已选中的点
    mstSet.insert(0);              // 从点 0 开始
    std::vector<std::tuple<int, int, int>> candidates; // 当前备选边 (u, v, 权重)

    // 循环直到所有节点都被选入 mstSet
    while (mstSet.size() < wid_prim->siz_node) {
        candidates.clear();
        std::vector<int> temp_step; // 用于存储当前步骤的边信息

        // 遍历 mstSet 中的点，寻找连接外部节点的所有候选边
        for (int u : mstSet) {
            for (int v = 0; v < wid_prim->siz_node; ++v) {
                if (mstSet.find(v) == mstSet.end() && wid_prim->edge[u][v] > 0) {
                    candidates.emplace_back(u, v, wid_prim->edge[u][v]);
                    temp_step.push_back(u);
                    temp_step.push_back(v);
                    temp_step.push_back(3); // 备选边标记为绿色
                }
            }
        }

        // 添加备选边标记为绿色的信息到步骤字符串
        if (!temp_step.empty()) {
            steps_prim += std::to_string(temp_step.size() / 3) + " ";
            for (size_t i = 0; i < temp_step.size(); i += 3) {
                steps_prim += std::to_string(temp_step[i]) + " " +
                              std::to_string(temp_step[i + 1]) + " " +
                              std::to_string(temp_step[i + 2]) + " ";
            }
            steps_prim += "\n";
        }

        // 从候选边中选取权值最小的边
        auto minEdge = *std::min_element(
            candidates.begin(), candidates.end(),
            [](const std::tuple<int, int, int>& a, const std::tuple<int, int, int>& b) {
                return std::get<2>(a) < std::get<2>(b);
            });
        int u = std::get<0>(minEdge);
        int v = std::get<1>(minEdge);


        // 将未选中的绿色边标记为黑色
        temp_step.clear();
        for (const auto& edge : candidates) {
            int cu = std::get<0>(edge);
            int cv = std::get<1>(edge);
            if ((cu == u && cv == v) || (cu == v && cv == u)) continue;
            temp_step.push_back(cu);
            temp_step.push_back(cv);
            temp_step.push_back(1); // 黑色
        }

        // 添加黑色边信息到步骤字符串
        if (!temp_step.empty()) {
            steps_prim += std::to_string(temp_step.size() / 3+1) + " ";
            for (size_t i = 0; i < temp_step.size(); i += 3) {
                steps_prim += std::to_string(temp_step[i]) + " " +
                              std::to_string(temp_step[i + 1]) + " " +
                              std::to_string(temp_step[i + 2]) + " ";
            }
            steps_prim += "\n";
        }
        // 将最小边标记为红色并加入 mstSet
        temp_step.clear();
        temp_step.push_back(u);
        temp_step.push_back(v);
        temp_step.push_back(2); // 红色
        mstSet.insert(v);

        // 添加最小边标记为红色的信息到步骤字符串
        //steps_prim += std::to_string(temp_step.size() / 3) + " ";
        for (size_t i = 0; i < temp_step.size(); i += 3) {
            steps_prim += std::to_string(temp_step[i]) + " " +
                          std::to_string(temp_step[i + 1]) + " " +
                          std::to_string(temp_step[i + 2]) + " ";
        }
        steps_prim += "\n";

    }



//Dijkstra----------------------------------------------------------------------------------------------------------------------------------



    std::string steps_dijkstra; // 存储Dijkstra步骤字符串
    std::vector<int> dist(wid_dij->siz_node, INT_MAX); // 存储从起始节点到其他节点的最短路径
    std::vector<int> prev(wid_dij->siz_node, -1);   // 用于记录前驱节点
    std::vector<bool> visited(wid_dij->siz_node, false);  // 标记节点是否已访问

    dist[0] = 0;  // 起始节点的距离为0
    for (int count = 0; count < wid_dij->siz_node; ++count) {
        // 选择距离源节点最近的未访问节点
        int u = -1;
        int minDist = INT_MAX;
        for (int i = 0; i < wid_dij->siz_node; ++i) {
            if (!visited[i] && dist[i] < minDist) {
                minDist = dist[i];
                u = i;
            }
        }

        // 如果没有找到，说明剩下的节点不可达，退出
        if (u == -1) break;

        u = -1;
        minDist = INT_MAX;

        int sum = 0;
        for (int i = 1; i < wid_dij->siz_node; ++i) {
            if (!visited[i] && dist[i] != INT_MAX) {
                sum++;
            }
        }
        if (sum != 0) {
            steps_dijkstra += std::to_string(sum) + " ";
        }

        for (int i = 1; i < wid_dij->siz_node; ++i) {
            if (!visited[i] && dist[i] != INT_MAX) {
                if (prev[i] != -1) {
                    steps_dijkstra += std::to_string(prev[i]) + " " + std::to_string(i) + " 3 ";
                } else {
                    steps_dijkstra += std::to_string(0) + " " + std::to_string(i) + " 3 ";
                }
            }
        }
        if (sum != 0) {
            steps_dijkstra += std::to_string(sum + 1) + " ";
        }

        for (int i = 1; i < wid_dij->siz_node; ++i) {
            if (!visited[i] && dist[i] != INT_MAX) {
                if (prev[i] != -1) {
                    steps_dijkstra += std::to_string(prev[i]) + " " + std::to_string(i) + " 1 ";
                } else {
                    steps_dijkstra += std::to_string(0) + " " + std::to_string(i) + " 1 ";
                }
            }
        }

        for (int i = 0; i < wid_dij->siz_node; ++i) {
            if (!visited[i] && dist[i] < minDist) {
                minDist = dist[i];
                u = i;
            }
        }

        if (u != 0) {
            if (prev[u] != -1) {
                steps_dijkstra += std::to_string(prev[u]) + " " + std::to_string(u) + " 2 ";
            } else {
                steps_dijkstra += std::to_string(0) + " " + std::to_string(u) + " 2 ";
            }
        }

        // 标记当前节点为已访问
        visited[u] = true;

        // 更新相邻节点的最短路径
        for (int v = 0; v < wid_dij->siz_node; ++v) {
            if (!visited[v] && wid_dij->edge[u][v] != 0) {
                int newDist = dist[u] + wid_dij->edge[u][v];
                if (newDist < dist[v]) {
                    dist[v] = newDist;
                    prev[v] = u;
                }
            }
        }
    }

    // 输出Dijkstra的步骤字符串
    qDebug() << "Dijkstra steps: " << QString::fromStdString(steps_dijkstra);












    //------------------------------------------------------------------------------------------------------------------------------------------


    s_dfs = steps_dfs;
    s_prim=steps_prim;
    s_dij=steps_dijkstra;
    wid_dfs->setStep(s_dfs); //设置dfs算法步骤信息,即传递对应字符串
    wid_prim->setStep(s_prim); //设置prim算法步骤信息
    wid_dij->setStep(s_dij); //设置dijkstra算法步骤信息
    wid_dfs->getPtr(DFSinfo);
    wid_prim->getPtr(Priminfo);
    wid_dij->getPtr(Dijinfo);
    timer->stop();
}



