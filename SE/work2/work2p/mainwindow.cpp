#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QFont>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setupUI();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupUI()
{
    setWindowTitle("教育基金会管理系统");
    setMinimumSize(1200, 700);
    
    // 创建中心Widget
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    // ========== 左侧导航栏 ==========
    QWidget *leftPanel = new QWidget(this);
    leftPanel->setFixedWidth(220);
    leftPanel->setStyleSheet(
        "QWidget { background-color: #001529; }"
    );
    
    QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setSpacing(0);
    
    // Logo区域
    QLabel *logoLabel = new QLabel("教育基金会", leftPanel);
    logoLabel->setAlignment(Qt::AlignCenter);
    logoLabel->setStyleSheet(
        "QLabel { "
        "   color: white; "
        "   font-size: 20px; "
        "   font-weight: bold; "
        "   padding: 25px; "
        "   background-color: #002140; "
        "}"
    );
    leftLayout->addWidget(logoLabel);
    
    // 导航列表
    navigationList = new QListWidget(leftPanel);
    navigationList->setStyleSheet(
        "QListWidget { "
        "   background-color: #001529; "
        "   border: none; "
        "   outline: none; "
        "   color: rgba(255, 255, 255, 0.65); "
        "   font-size: 14px; "
        "}"
        "QListWidget::item { "
        "   padding: 16px 24px; "
        "   border: none; "
        "}"
        "QListWidget::item:hover { "
        "   background-color: rgba(255, 255, 255, 0.08); "
        "   color: white; "
        "}"
        "QListWidget::item:selected { "
        "   background-color: #1890ff; "
        "   color: white; "
        "}"
    );
    
    navigationList->addItem("📊 数据概览");
    navigationList->addItem("💰 捐助管理");
    navigationList->addItem("📝 用款申请");
    navigationList->addItem("📈 财政报表");
    
    connect(navigationList, &QListWidget::currentRowChanged, 
            this, &MainWindow::onNavigationItemClicked);
    
    leftLayout->addWidget(navigationList);
    leftLayout->addStretch();
    
    // 版本信息
    QLabel *versionLabel = new QLabel("v1.0.0", leftPanel);
    versionLabel->setAlignment(Qt::AlignCenter);
    versionLabel->setStyleSheet(
        "QLabel { "
        "   color: rgba(255, 255, 255, 0.45); "
        "   font-size: 12px; "
        "   padding: 16px; "
        "}"
    );
    leftLayout->addWidget(versionLabel);
    
    mainLayout->addWidget(leftPanel);
    
    // ========== 右侧内容区域 ==========
    QWidget *rightPanel = new QWidget(this);
    rightPanel->setStyleSheet("QWidget { background-color: #f0f2f5; }");
    
    QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->setContentsMargins(20, 20, 20, 20);
    
    // 顶部统计信息栏
    statsLabel = new QLabel(this);
    statsLabel->setStyleSheet(
        "QLabel { "
        "   background-color: white; "
        "   border-radius: 4px; "
        "   padding: 16px; "
        "   font-size: 14px; "
        "   border: 1px solid #e8e8e8; "
        "}"
    );
    statsLabel->setWordWrap(true);
    rightLayout->addWidget(statsLabel);
    
    // 内容堆栈
    contentStack = new QStackedWidget(this);
    
    // 创建欢迎页面
    QWidget *welcomePage = new QWidget(this);
    QVBoxLayout *welcomeLayout = new QVBoxLayout(welcomePage);
    welcomeLayout->setAlignment(Qt::AlignCenter);
    
    QLabel *welcomeTitle = new QLabel("欢迎使用教育基金会管理系统", welcomePage);
    QFont welcomeFont = welcomeTitle->font();
    welcomeFont.setPointSize(24);
    welcomeFont.setBold(true);
    welcomeTitle->setFont(welcomeFont);
    welcomeTitle->setAlignment(Qt::AlignCenter);
    welcomeTitle->setStyleSheet("color: #1890ff;");
    
    QLabel *welcomeDesc = new QLabel(
        "本系统包含以下功能模块：\n\n"
        "• 捐助管理：处理捐助申请、身份确认、证书生成\n"
        "• 用款申请：管理学校用款申请、合法性校验、审批流程\n"
        "• 财政报表：查看月度财务报表、收支统计\n\n"
        "请从左侧导航栏选择功能模块开始使用", 
        welcomePage);
    welcomeDesc->setAlignment(Qt::AlignCenter);
    welcomeDesc->setStyleSheet(
        "color: #595959; "
        "font-size: 14px; "
        "line-height: 1.8;"
    );
    
    welcomeLayout->addWidget(welcomeTitle);
    welcomeLayout->addSpacing(30);
    welcomeLayout->addWidget(welcomeDesc);
    
    contentStack->addWidget(welcomePage);
    
    // 添加各个功能模块
    donationWidget = new DonationWidget(this);
    contentStack->addWidget(donationWidget);
    
    applicationWidget = new ApplicationWidget(this);
    contentStack->addWidget(applicationWidget);
    
    reportWidget = new ReportWidget(this);
    contentStack->addWidget(reportWidget);
    
    rightLayout->addWidget(contentStack, 1);
    
    mainLayout->addWidget(rightPanel, 1);
    
    // 默认选中第一项
    navigationList->setCurrentRow(0);
    updateStatistics();
}

void MainWindow::onNavigationItemClicked(int index)
{
    contentStack->setCurrentIndex(index);
    
    // 刷新数据
    if (index == 1) {
        donationWidget->refreshData();
    } else if (index == 2) {
        applicationWidget->refreshData();
    } else if (index == 3) {
        reportWidget->refreshData();
    }
    
    updateStatistics();
}

void MainWindow::updateStatistics()
{
    double totalIncome = DataManager::instance().getTotalIncome();
    double totalExpense = DataManager::instance().getTotalExpense();
    double balance = DataManager::instance().getBalance();
    
    auto pendingDonations = DataManager::instance().getDonationsByStatus("待审核");
    auto pendingApplications = DataManager::instance().getApplicationsByStatus("待审核");
    
    QString statsText = QString(
        "📊 <b>系统概况</b> | "
        "总收入: <span style='color:#52c41a;font-weight:bold;'>¥%1</span> | "
        "总支出: <span style='color:#ff4d4f;font-weight:bold;'>¥%2</span> | "
        "账户余额: <span style='color:#1890ff;font-weight:bold;'>¥%3</span> | "
        "待审核捐助: <b>%4</b> | "
        "待审核申请: <b>%5</b>"
    ).arg(totalIncome, 0, 'f', 2)
     .arg(totalExpense, 0, 'f', 2)
     .arg(balance, 0, 'f', 2)
     .arg(pendingDonations.size())
     .arg(pendingApplications.size());
    
    statsLabel->setText(statsText);
}
