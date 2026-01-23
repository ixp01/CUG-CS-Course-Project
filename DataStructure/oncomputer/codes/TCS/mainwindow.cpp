#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QApplication>
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , lastDijkstraResult(std::vector<int>(), -1)
    , lastFloydResult(std::vector<int>(), -1)
{
    ui->setupUi(this);
    setupUI();
    
    // 自动加载数据
    if (graph.loadFromCSV("省会城市邻接矩阵.csv")) {
        updateCityList();
        statusLabel->setText("数据加载成功！");
    } else {
        statusLabel->setText("数据加载失败，请手动加载CSV文件");
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupUI() {
    setWindowTitle("交通咨询系统 - 数据结构课程设计");
    setMinimumSize(1200, 800);
    
    // 创建中央widget和分割器
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    mainSplitter = new QSplitter(Qt::Horizontal, centralWidget);
    
    // 左侧控制面板
    setupControlPanel();
    
    // 右侧结果展示
    setupResultPanel();
    
    // 底部状态栏
    setupStatusBar();
    
    // 布局
    QHBoxLayout* mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->addWidget(mainSplitter);
    mainSplitter->addWidget(controlGroup);
    mainSplitter->addWidget(resultGroup);
    mainSplitter->setSizes({300, 900});
}

void MainWindow::setupControlPanel() {
    controlGroup = new QGroupBox("控制面板", this);
    controlGroup->setMaximumWidth(350);
    
    QVBoxLayout* controlLayout = new QVBoxLayout(controlGroup);
    
    // 数据加载区域
    QGroupBox* dataGroup = new QGroupBox("数据管理");
    QVBoxLayout* dataLayout = new QVBoxLayout(dataGroup);
    
    loadButton = new QPushButton("加载CSV数据");
    loadButton->setIcon(style()->standardIcon(QStyle::SP_DialogOpenButton));
    connect(loadButton, &QPushButton::clicked, this, &MainWindow::loadData);
    dataLayout->addWidget(loadButton);
    
    // 路径查询区域
    QGroupBox* queryGroup = new QGroupBox("路径查询");
    QGridLayout* queryLayout = new QGridLayout(queryGroup);
    
    queryLayout->addWidget(new QLabel("起点:"), 0, 0);
    startComboBox = new QComboBox();
    connect(startComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onStartCityChanged);
    queryLayout->addWidget(startComboBox, 0, 1);
    
    queryLayout->addWidget(new QLabel("终点:"), 1, 0);
    endComboBox = new QComboBox();
    connect(endComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onEndCityChanged);
    queryLayout->addWidget(endComboBox, 1, 1);
    
    queryLayout->addWidget(new QLabel("算法:"), 2, 0);
    algorithmComboBox = new QComboBox();
    algorithmComboBox->addItems({"Dijkstra", "Floyd", "所有路径DFS"});
    connect(algorithmComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onAlgorithmChanged);
    queryLayout->addWidget(algorithmComboBox, 2, 1);
    
    queryLayout->addWidget(new QLabel("绕过城市:"), 3, 0);
    avoidComboBox = new QComboBox();
    avoidComboBox->addItem("无");
    queryLayout->addWidget(avoidComboBox, 3, 1);
    
    queryLayout->addWidget(new QLabel("最大深度:"), 4, 0);
    maxDepthSpinBox = new QSpinBox();
    maxDepthSpinBox->setRange(3, 15);
    maxDepthSpinBox->setValue(10);
    queryLayout->addWidget(maxDepthSpinBox, 4, 1);
    
    queryLayout->addWidget(new QLabel("K值:"), 5, 0);
    kSpinBox = new QSpinBox();
    kSpinBox->setRange(1, 20);
    kSpinBox->setValue(3);
    queryLayout->addWidget(kSpinBox, 5, 1);
    
    // 功能按钮区域
    QGroupBox* buttonGroup = new QGroupBox("功能操作");
    QVBoxLayout* buttonLayout = new QVBoxLayout(buttonGroup);
    
    validateButton = new QPushButton("验证武汉中心性");
    validateButton->setIcon(style()->standardIcon(QStyle::SP_DialogApplyButton));
    connect(validateButton, &QPushButton::clicked, this, &MainWindow::validateWuhan);
    buttonLayout->addWidget(validateButton);
    
    shortestPathButton = new QPushButton("查找最短路径");
    shortestPathButton->setIcon(style()->standardIcon(QStyle::SP_ArrowForward));
    connect(shortestPathButton, &QPushButton::clicked, this, &MainWindow::findShortestPath);
    buttonLayout->addWidget(shortestPathButton);
    
    allPathsButton = new QPushButton("查找所有路径");
    allPathsButton->setIcon(style()->standardIcon(QStyle::SP_FileDialogDetailedView));
    connect(allPathsButton, &QPushButton::clicked, this, &MainWindow::findAllPaths);
    buttonLayout->addWidget(allPathsButton);
    
    kPathsButton = new QPushButton("查找K短路径");
    kPathsButton->setIcon(style()->standardIcon(QStyle::SP_FileDialogListView));
    connect(kPathsButton, &QPushButton::clicked, this, &MainWindow::findKShortestPaths);
    buttonLayout->addWidget(kPathsButton);
    
    exportButton = new QPushButton("导出结果");
    exportButton->setIcon(style()->standardIcon(QStyle::SP_DialogSaveButton));
    exportButton->setEnabled(false);
    connect(exportButton, &QPushButton::clicked, this, &MainWindow::exportResults);
    buttonLayout->addWidget(exportButton);
    
    // 添加到主控制布局
    controlLayout->addWidget(dataGroup);
    controlLayout->addWidget(queryGroup);
    controlLayout->addWidget(buttonGroup);
    controlLayout->addStretch();
}

void MainWindow::setupResultPanel() {
    resultGroup = new QGroupBox("查询结果", this);
    
    QVBoxLayout* resultLayout = new QVBoxLayout(resultGroup);
    
    // 结果表格
    resultTable = new QTableWidget(0, 4);
    resultTable->setHorizontalHeaderLabels({"序号", "路径类型", "路径", "距离(km)"});
    resultTable->horizontalHeader()->setStretchLastSection(true);
    resultTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    resultTable->setAlternatingRowColors(true);
    
    // 详细信息文本框
    detailText = new QTextEdit();
    detailText->setMaximumHeight(200);
    detailText->setReadOnly(true);
    
    // 分割器
    QSplitter* resultSplitter = new QSplitter(Qt::Vertical);
    resultSplitter->addWidget(resultTable);
    resultSplitter->addWidget(detailText);
    resultSplitter->setSizes({500, 200});
    
    resultLayout->addWidget(resultSplitter);
}

void MainWindow::setupStatusBar() {
    statusLabel = new QLabel("欢迎使用交通咨询系统");
    progressBar = new QProgressBar();
    progressBar->setVisible(false);
    
    statusBar()->addWidget(statusLabel, 1);
    statusBar()->addWidget(progressBar);
}

void MainWindow::loadData() {
    QString fileName = QFileDialog::getOpenFileName(this,
        "选择城市距离数据文件", ".", "CSV Files (*.csv)");
    
    if (!fileName.isEmpty()) {
        showProgress("正在加载数据...");
        
        if (graph.loadFromCSV(fileName)) {
            updateCityList();
            clearResults();
            statusLabel->setText("数据加载成功！共" + QString::number(graph.numCities()) + "个城市");
        } else {
            QMessageBox::warning(this, "错误", "无法加载数据文件！");
            statusLabel->setText("数据加载失败");
        }
        
        hideProgress();
    }
}

void MainWindow::updateCityList() {
    startComboBox->clear();
    endComboBox->clear();
    avoidComboBox->clear();
    avoidComboBox->addItem("无");
    
    const auto& cities = graph.getCityNames();
    for (const QString& city : cities) {
        startComboBox->addItem(city);
        endComboBox->addItem(city);
        avoidComboBox->addItem(city);
    }
    
    // 默认设置武汉为起点
    int wuhanIndex = graph.findCityIndex("武汉");
    if (wuhanIndex != -1) {
        startComboBox->setCurrentIndex(wuhanIndex);
    }
}

void MainWindow::validateWuhan() {
    showProgress("正在验证武汉中心性...");
    
    bool result = graph.validateWuhanCentrality();
    
    hideProgress();
    
    QString message;
    if (result) {
        message = "验证结果：全国其他省会城市到武汉中间不超过2个省（省会城市）的假设成立！\n\n"
                 "这证实了武汉确实处于全国的中心位置。";
        QMessageBox::information(this, "武汉中心性验证", message);
        statusLabel->setText("武汉中心性验证通过");
    } else {
        message = "验证结果：存在城市到武汉需要超过2跳的路径。\n\n"
                 "请检查控制台输出查看具体不满足条件的城市。";
        QMessageBox::warning(this, "武汉中心性验证", message);
        statusLabel->setText("武汉中心性验证未通过");
    }
    
    detailText->setText(message);
}

void MainWindow::findShortestPath() {
    int startIdx = startComboBox->currentIndex();
    int endIdx = endComboBox->currentIndex();
    
    if (startIdx == -1 || endIdx == -1 || startIdx == endIdx) {
        QMessageBox::warning(this, "错误", "请选择不同的起点和终点！");
        return;
    }
    
    showProgress("正在计算最短路径...");
    
    QString algorithm = algorithmComboBox->currentText();
    QString avoidCity = avoidComboBox->currentText();
    
    PathInfo result(std::vector<int>(), -1);
    
    if (avoidCity != "无") {
        int avoidIdx = graph.findCityIndex(avoidCity);
        if (avoidIdx != -1 && avoidIdx != startIdx && avoidIdx != endIdx) {
            result = graph.dijkstraAvoid(startIdx, endIdx, avoidIdx);
            algorithm += " (绕过" + avoidCity + ")";
        } else {
            result = graph.dijkstra(startIdx, endIdx);
        }
    } else {
        if (algorithm == "Dijkstra") {
            result = graph.dijkstra(startIdx, endIdx);
        } else if (algorithm == "Floyd") {
            result = graph.floyd(startIdx, endIdx);
        }
    }
    
    hideProgress();
    
    if (result.path.empty() || result.distance < 0) {
        QMessageBox::information(this, "结果", "未找到可行路径！");
        statusLabel->setText("未找到路径");
        return;
    }
    
    // 显示结果
    clearResults();
    displayPath(result, algorithm);
    
    lastDijkstraResult = result;
    lastStartCity = startComboBox->currentText();
    lastEndCity = endComboBox->currentText();
    exportButton->setEnabled(true);
    
    statusLabel->setText("最短路径查找完成，距离: " + 
                        QString::number(result.distance, 'f', 2) + " km");
}

void MainWindow::findAllPaths() {
    int startIdx = startComboBox->currentIndex();
    int endIdx = endComboBox->currentIndex();
    
    if (startIdx == -1 || endIdx == -1 || startIdx == endIdx) {
        QMessageBox::warning(this, "错误", "请选择不同的起点和终点！");
        return;
    }
    
    showProgress("正在查找所有路径...");
    
    int maxDepth = maxDepthSpinBox->value();
    auto allPaths = graph.findAllPaths(startIdx, endIdx, maxDepth);
    
    // 同时计算Dijkstra和Floyd结果用于比较
    auto dijkstraResult = graph.dijkstra(startIdx, endIdx);
    auto floydResult = graph.floyd(startIdx, endIdx);
    
    hideProgress();
    
    if (allPaths.empty()) {
        QMessageBox::information(this, "结果", "未找到可行路径！");
        statusLabel->setText("未找到路径");
        return;
    }
    
    // 显示结果
    displayResults(allPaths, dijkstraResult, floydResult);
    
    // 保存结果用于导出
    lastAllPaths = allPaths;
    lastDijkstraResult = dijkstraResult;
    lastFloydResult = floydResult;
    lastStartCity = startComboBox->currentText();
    lastEndCity = endComboBox->currentText();
    exportButton->setEnabled(true);
    
    statusLabel->setText("找到 " + QString::number(allPaths.size()) + 
                        " 条路径，已按距离排序");
}

void MainWindow::findKShortestPaths() {
    int startIdx = startComboBox->currentIndex();
    int endIdx = endComboBox->currentIndex();
    
    if (startIdx == -1 || endIdx == -1 || startIdx == endIdx) {
        QMessageBox::warning(this, "错误", "请选择不同的起点和终点！");
        return;
    }
    
    showProgress("正在计算K短路径...");
    
    int k = kSpinBox->value();
    auto kPaths = graph.kShortestPaths(startIdx, endIdx, k);
    
    hideProgress();
    
    if (kPaths.empty()) {
        QMessageBox::information(this, "结果", "未找到可行路径！");
        statusLabel->setText("未找到路径");
        return;
    }
    
    // 显示结果
    clearResults();
    for (int i = 0; i < kPaths.size(); i++) {
        displayPath(kPaths[i], "第" + QString::number(i+1) + "短路径");
    }
    
    // 保存结果
    lastAllPaths = kPaths;
    lastStartCity = startComboBox->currentText();
    lastEndCity = endComboBox->currentText();
    exportButton->setEnabled(true);
    
    statusLabel->setText("找到 " + QString::number(kPaths.size()) + 
                        " 条K短路径");
}

void MainWindow::displayResults(const std::vector<PathInfo>& paths, 
                               const PathInfo& dijkstraResult,
                               const PathInfo& floydResult) {
    clearResults();
    
    // 显示所有路径
    for (int i = 0; i < paths.size(); i++) {
        displayPath(paths[i], "路径" + QString::number(i+1));
    }
    
    // 显示算法比较结果
    if (!dijkstraResult.path.empty()) {
        displayPath(dijkstraResult, "Dijkstra最短");
    }
    
    if (!floydResult.path.empty()) {
        displayPath(floydResult, "Floyd最短");
    }
    
    // 显示详细比较信息
    QString detail = "算法比较结果:\n";
    detail += "================\n\n";
    
    if (!dijkstraResult.path.empty() && !floydResult.path.empty()) {
        detail += "Dijkstra算法最短距离: " + 
                 QString::number(dijkstraResult.distance, 'f', 2) + " km\n";
        detail += "Floyd算法最短距离: " + 
                 QString::number(floydResult.distance, 'f', 2) + " km\n\n";
        
        if (std::abs(dijkstraResult.distance - floydResult.distance) < 0.01) {
            detail += "结论: 两种算法得到相同的最短距离！\n";
        } else {
            detail += "结论: 两种算法结果不同，请检查实现。\n";
        }
    }
    
    detail += "\n所有路径已按快速排序算法排序，最短路径在前。\n";
    detail += "总共找到 " + QString::number(paths.size()) + " 条可行路径。";
    
    detailText->setText(detail);
}

void MainWindow::displayPath(const PathInfo& pathInfo, const QString& algorithmName) {
    if (pathInfo.path.empty()) return;
    
    int row = resultTable->rowCount();
    resultTable->insertRow(row);
    
    resultTable->setItem(row, 0, new QTableWidgetItem(QString::number(row + 1)));
    resultTable->setItem(row, 1, new QTableWidgetItem(algorithmName));
    resultTable->setItem(row, 2, new QTableWidgetItem(pathToString(pathInfo)));
    resultTable->setItem(row, 3, new QTableWidgetItem(
        QString::number(pathInfo.distance, 'f', 2)));
    
    // 为最短路径行设置特殊颜色
    if (algorithmName.contains("最短") || algorithmName.contains("第1短")) {
        for (int col = 0; col < 4; col++) {
            resultTable->item(row, col)->setBackground(QColor(144, 238, 144));
        }
    }
}

QString MainWindow::pathToString(const PathInfo& pathInfo) {
    QStringList pathStrings;
    for (int cityIdx : pathInfo.path) {
        pathStrings.append(graph.getCityName(cityIdx));
    }
    return pathStrings.join(" → ");
}

void MainWindow::clearResults() {
    resultTable->setRowCount(0);
    detailText->clear();
}

void MainWindow::exportResults() {
    if (lastAllPaths.empty() && lastDijkstraResult.path.empty()) {
        QMessageBox::warning(this, "错误", "没有可导出的结果！");
        return;
    }
    
    QString fileName = QFileDialog::getSaveFileName(this,
        "保存查询结果", 
        "交通查询结果_" + QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss") + ".txt",
        "Text Files (*.txt)");
    
    if (!fileName.isEmpty()) {
        graph.exportResults(fileName, lastAllPaths, lastDijkstraResult, 
                           lastFloydResult, lastStartCity, lastEndCity);
        QMessageBox::information(this, "成功", "结果已导出到文件：" + fileName);
        statusLabel->setText("结果已导出到: " + fileName);
    }
}

void MainWindow::showProgress(const QString& message) {
    statusLabel->setText(message);
    progressBar->setVisible(true);
    progressBar->setRange(0, 0); // 无限进度条
    QApplication::processEvents();
}

void MainWindow::hideProgress() {
    progressBar->setVisible(false);
    QApplication::processEvents();
}

void MainWindow::onAlgorithmChanged() {
    // 可以根据算法选择调整UI
}

void MainWindow::onStartCityChanged() {
    // 可以添加起点变化时的逻辑
}

void MainWindow::onEndCityChanged() {
    // 可以添加终点变化时的逻辑
}
