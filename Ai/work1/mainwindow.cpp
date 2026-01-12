/**
 * 罗马尼亚度假问题 - 路径搜索系统主窗口实现
 * 
 * 功能：
 * - 提供图形化界面展示罗马尼亚地图
 * - 支持选择起点、终点和搜索算法
 * - 可视化显示搜索结果路径
 * - 实时显示搜索过程的详细日志
 * - 比较不同算法的性能和结果
 */

#include "ucs.h"
#include "greedy.h"
#include "astar.h"
#include "graph.h"
#include "mainwindow.h"
 #include "ui_mainwindow.h"
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QMessageBox>
#include <QDebug>
#include <QMap>
#include <QPair>
#include <QSet>

// 城市在图形界面上的坐标位置（像素坐标）
// 这些坐标经过调整以获得较好的视觉效果
static QMap<QString, QPointF> CITY_POS = {
    {"Arad", {60, 80}}, {"Zerind", {80, 30}}, {"Oradea", {120, 20}}, {"Sibiu", {180, 80}},
    {"Timisoara", {30, 160}}, {"Lugoj", {80, 200}}, {"Mehadia", {120, 250}}, {"Drobeta", {120, 320}},
    {"Craiova", {200, 350}}, {"Rimnicu Vilcea", {220, 160}}, {"Fagaras", {260, 60}},
    {"Pitesti", {300, 220}}, {"Bucharest", {400, 300}}, {"Giurgiu", {420, 380}},
    {"Urziceni", {480, 220}}, {"Hirsova", {540, 180}}, {"Eforie", {600, 200}},
    {"Vaslui", {540, 300}}, {"Iasi", {600, 340}}, {"Neamt", {650, 300}}
};

/**
 * 主窗口构造函数
 * 初始化界面、创建算法对象、连接信号槽
 * @param parent 父窗口指针
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , scene(new QGraphicsScene(this))          // 创建图形场景
    , ucsAlgo(new UCS(this))                   // 创建UCS算法对象
    , greedyAlgo(new Greedy(this))             // 创建贪婪算法对象
    , astarAlgo(new AStar(this))               // 创建A*算法对象
{
    ui->setupUi(this);                         // 设置UI界面
    ui->graphicsView->setScene(scene);         // 将场景关联到图形视图
    initComboBoxes();                          // 初始化下拉框选项
    drawMap();                                 // 绘制地图
    
    // 连接按钮点击信号到搜索槽函数
    connect(ui->btnSearch, &QPushButton::clicked, this, &MainWindow::on_btnSearch_clicked);
    
    // 连接算法对象的日志信号到主窗口的日志处理槽函数
    connect(ucsAlgo, &UCS::logMessage, this, &MainWindow::handleLogMessage);
    connect(greedyAlgo, &Greedy::logMessage, this, &MainWindow::handleLogMessage);
    connect(astarAlgo, &AStar::logMessage, this, &MainWindow::handleLogMessage);
}

/**
 * 析构函数：清理资源
 */
MainWindow::~MainWindow()
{
    delete ui;
}

/**
 * 初始化起点和终点下拉框
 * 将所有城市名称添加到下拉框中，并设置默认值
 */
void MainWindow::initComboBoxes() {
    ui->comboBoxStart->clear();
    ui->comboBoxGoal->clear();
    
    // 遍历地图中的所有城市，添加到下拉框
    for (const auto &city : ROMANIA_MAP.keys()) {
        ui->comboBoxStart->addItem(city);
        ui->comboBoxGoal->addItem(city);
    }
    
    // 设置默认的起点和终点
    ui->comboBoxStart->setCurrentText("Arad");
    ui->comboBoxGoal->setCurrentText("Bucharest");
}

/**
 * 绘制罗马尼亚地图
 * 包括城市节点、连接边和边权重标注
 */
void MainWindow::drawMap() {
    scene->clear();                                           // 清空场景
    QSet<QPair<QString, QString>> drawnEdges;                // 记录已绘制的边，避免重复
    
    // 绘制边和边权重
    for (auto it = ROMANIA_MAP.begin(); it != ROMANIA_MAP.end(); ++it) {
        QString city = it.key();
        for (const auto &neighbor : it.value()) {
            QString neighborCity = neighbor.first;
            int distance = neighbor.second;
            
            // 避免重复绘制同一条边（无向图的边会在两个城市中都出现）
            QPair<QString, QString> edge1 = qMakePair(city, neighborCity);
            QPair<QString, QString> edge2 = qMakePair(neighborCity, city);
            if (drawnEdges.contains(edge1) || drawnEdges.contains(edge2)) {
                continue;
            }
            drawnEdges.insert(edge1);
            
            QPointF p1 = CITY_POS[city];
            QPointF p2 = CITY_POS[neighborCity];
            
            // 绘制连接线（灰色）
            scene->addLine(QLineF(p1, p2), QPen(Qt::gray, 2));
            
            // 在边的中点显示距离数值
            QPointF midPoint = (p1 + p2) / 2;
            QGraphicsTextItem *distanceText = scene->addText(QString::number(distance));
            distanceText->setPos(midPoint.x() - 10, midPoint.y() - 10);
            distanceText->setDefaultTextColor(Qt::blue);        // 蓝色字体
            
            // 设置较小的字体，避免影响视觉效果
            QFont font = distanceText->font();
            font.setPointSize(8);
            distanceText->setFont(font);
        }
    }
    
    // 绘制城市节点
    for (auto it = CITY_POS.begin(); it != CITY_POS.end(); ++it) {
        QPointF pos = it.value();
        
        // 绘制城市圆点（黄色填充，黑色边框）
        QGraphicsEllipseItem *ellipse = scene->addEllipse(
            pos.x()-8, pos.y()-8, 16, 16, 
            QPen(Qt::black), QBrush(Qt::yellow)
        );
        
        // 在城市上方显示城市名称
        QGraphicsTextItem *text = scene->addText(it.key());
        text->setPos(pos.x()-15, pos.y()-30);
    }
}

/**
 * 搜索按钮点击事件处理函数
 * 执行选定的搜索算法并显示结果
 */
void MainWindow::on_btnSearch_clicked() {
    // 清空控制台输出区域，准备显示新的搜索过程
    ui->textEditConsole->clear();
    
    // 移除上一次搜索的高亮路径
    for (QGraphicsLineItem* item : highlightLines) {
        scene->removeItem(item);
        delete item;
    }
    highlightLines.clear();

    // 获取用户选择的参数
    QString start = ui->comboBoxStart->currentText();    // 起点城市
    QString goal = ui->comboBoxGoal->currentText();      // 终点城市
    QString algo = ui->comboBoxAlgo->currentText();      // 搜索算法
    
    PathResult result;
    
    // 根据选择的算法执行相应的搜索
    if (algo == "UCS") {
        result = ucsAlgo->findPath(start, goal);
    } else if (algo == "Greedy") {
        result = greedyAlgo->findPath(start, goal);
    } else if (algo == "A*") {
        result = astarAlgo->findPath(start, goal);
    }
    
    // 显示搜索结果
    showResult(result);

    // 在地图上高亮显示找到的路径
    if (!result.path.isEmpty()) {
        for (int i = 0; i < result.path.size()-1; ++i) {
            QPointF p1 = CITY_POS[result.path[i]];
            QPointF p2 = CITY_POS[result.path[i+1]];
            
            // 绘制红色粗线表示路径
            QGraphicsLineItem* line = scene->addLine(QLineF(p1, p2), QPen(Qt::red, 4));
            highlightLines.append(line);    // 保存线条指针，用于下次清理
        }
    }
}

/**
 * 显示搜索结果
 * 在界面上显示找到的路径、总代价和访问节点数
 * @param result 搜索算法返回的结果
 */
void MainWindow::showResult(const PathResult &result) {
    if (result.path.isEmpty()) {
        // 未找到路径的情况
        ui->labelPath->setText("路径：无可达路径");
        ui->labelCost->setText("");
        ui->labelVisited->setText("");
        return;
    }
    
    // 构造路径字符串，使用箭头连接城市名
    QString pathStr = "路径：";
    for (int i = 0; i < result.path.size(); ++i) {
        pathStr += result.path[i];
        if (i != result.path.size()-1) pathStr += " → ";
    }
    
    // 在界面上显示结果信息
    ui->labelPath->setText(pathStr);
    ui->labelCost->setText(QString("总代价：%1 km").arg(result.totalCost));
    ui->labelVisited->setText(QString("访问节点数：%1").arg(result.visitedNodes));
}

/**
 * 处理算法发出的日志消息信号
 * @param msg 要显示的消息内容
 */
void MainWindow::handleLogMessage(const QString &msg) {
    logToConsole(msg);
}

/**
 * 将消息添加到控制台输出区域
 * @param msg 要显示的消息内容
 */
void MainWindow::logToConsole(const QString &msg) {
    ui->textEditConsole->append(msg);
}





