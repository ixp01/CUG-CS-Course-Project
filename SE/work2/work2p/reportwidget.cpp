#include "reportwidget.h"
#include "ui_reportwidget.h"
#include <QHeaderView>
#include <QDateTime>
#include <QFileDialog>
#include <QTextStream>
#include <QFile>
#include <QMessageBox>
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QStringConverter>
#endif

ReportWidget::ReportWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ReportWidget)
{
    setupUI();
    loadMonthlyReport();
}

ReportWidget::~ReportWidget()
{
    delete ui;
}

void ReportWidget::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // 标题
    QLabel *titleLabel = new QLabel("财政报表", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    mainLayout->addWidget(titleLabel);
    
    // 统计卡片区域
    QHBoxLayout *statsLayout = new QHBoxLayout();
    
    // 创建统计卡片的辅助函数
    auto createStatCard = [](const QString& title, QLabel** valueLabel) -> QGroupBox* {
        QGroupBox *box = new QGroupBox(title);
        box->setStyleSheet("QGroupBox { font-weight: bold; padding: 10px; }");
        QVBoxLayout *layout = new QVBoxLayout(box);
        *valueLabel = new QLabel("¥0.00");
        QFont font = (*valueLabel)->font();
        font.setPointSize(18);
        font.setBold(true);
        (*valueLabel)->setFont(font);
        (*valueLabel)->setAlignment(Qt::AlignCenter);
        layout->addWidget(*valueLabel);
        return box;
    };
    
    QGroupBox *totalIncomeBox = createStatCard("总收入", &totalIncomeLabel);
    totalIncomeLabel->setStyleSheet("color: #52c41a;");
    statsLayout->addWidget(totalIncomeBox);
    
    QGroupBox *totalExpenseBox = createStatCard("总支出", &totalExpenseLabel);
    totalExpenseLabel->setStyleSheet("color: #ff4d4f;");
    statsLayout->addWidget(totalExpenseBox);
    
    QGroupBox *balanceBox = createStatCard("账户余额", &balanceLabel);
    balanceLabel->setStyleSheet("color: #1890ff;");
    statsLayout->addWidget(balanceBox);
    
    mainLayout->addLayout(statsLayout);
    
    // 月度报表区域
    QGroupBox *monthlyBox = new QGroupBox("月度报表");
    QVBoxLayout *monthlyLayout = new QVBoxLayout(monthlyBox);
    
    // 月份选择
    QHBoxLayout *dateLayout = new QHBoxLayout();
    dateLayout->addWidget(new QLabel("选择月份:"));
    
    yearCombo = new QComboBox(this);
    int currentYear = QDate::currentDate().year();
    for (int i = currentYear - 5; i <= currentYear + 1; i++) {
        yearCombo->addItem(QString::number(i));
    }
    yearCombo->setCurrentText(QString::number(currentYear));
    connect(yearCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ReportWidget::onMonthChanged);
    dateLayout->addWidget(yearCombo);
    
    dateLayout->addWidget(new QLabel("年"));
    
    monthCombo = new QComboBox(this);
    for (int i = 1; i <= 12; i++) {
        monthCombo->addItem(QString("%1月").arg(i));
    }
    monthCombo->setCurrentIndex(QDate::currentDate().month() - 1);
    connect(monthCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ReportWidget::onMonthChanged);
    dateLayout->addWidget(monthCombo);
    
    dateLayout->addStretch();
    
    // 本月统计
    QHBoxLayout *monthStatsLayout = new QHBoxLayout();
    monthStatsLayout->addWidget(new QLabel("本月收入:"));
    monthIncomeLabel = new QLabel("¥0.00");
    monthIncomeLabel->setStyleSheet("color: #52c41a; font-weight: bold; font-size: 14px;");
    monthStatsLayout->addWidget(monthIncomeLabel);
    
    monthStatsLayout->addSpacing(30);
    
    monthStatsLayout->addWidget(new QLabel("本月支出:"));
    monthExpenseLabel = new QLabel("¥0.00");
    monthExpenseLabel->setStyleSheet("color: #ff4d4f; font-weight: bold; font-size: 14px;");
    monthStatsLayout->addWidget(monthExpenseLabel);
    
    monthStatsLayout->addStretch();
    
    dateLayout->addLayout(monthStatsLayout);
    
    monthlyLayout->addLayout(dateLayout);
    
    // 详细记录表格
    tableWidget = new QTableWidget(this);
    tableWidget->setColumnCount(5);
    tableWidget->setHorizontalHeaderLabels({
        "ID", "类型", "金额(元)", "说明", "记录时间"
    });
    
    tableWidget->horizontalHeader()->setStretchLastSection(true);
    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableWidget->setAlternatingRowColors(true);
    
    tableWidget->setColumnWidth(0, 60);
    tableWidget->setColumnWidth(1, 80);
    tableWidget->setColumnWidth(2, 120);
    tableWidget->setColumnWidth(3, 300);
    
    monthlyLayout->addWidget(tableWidget);
    
    // 导出按钮
    QHBoxLayout *exportLayout = new QHBoxLayout();
    exportLayout->addStretch();
    
    exportBtn = new QPushButton("导出报表", this);
    exportBtn->setMinimumHeight(35);
    exportBtn->setStyleSheet("QPushButton { background-color: #1890ff; color: white; "
                            "border: none; border-radius: 4px; padding: 8px 16px; }"
                            "QPushButton:hover { background-color: #40a9ff; }");
    connect(exportBtn, &QPushButton::clicked, this, &ReportWidget::onExportReport);
    exportLayout->addWidget(exportBtn);
    
    monthlyLayout->addLayout(exportLayout);
    
    mainLayout->addWidget(monthlyBox);
}

void ReportWidget::loadMonthlyReport()
{
    updateStatistics();
    
    tableWidget->setRowCount(0);
    
    int year = yearCombo->currentText().toInt();
    int month = monthCombo->currentIndex() + 1;
    
    QList<FinanceRecord> records = DataManager::instance().getFinanceRecordsByMonth(year, month);
    
    for (const auto& record : records) {
        int row = tableWidget->rowCount();
        tableWidget->insertRow(row);
        
        tableWidget->setItem(row, 0, new QTableWidgetItem(QString::number(record.id)));
        
        QTableWidgetItem *typeItem = new QTableWidgetItem(record.type);
        if (record.type == "收入") {
            typeItem->setForeground(QColor("#52c41a"));
        } else {
            typeItem->setForeground(QColor("#ff4d4f"));
        }
        tableWidget->setItem(row, 1, typeItem);
        
        QTableWidgetItem *amountItem = new QTableWidgetItem(
            QString::number(record.amount, 'f', 2));
        amountItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        tableWidget->setItem(row, 2, amountItem);
        
        tableWidget->setItem(row, 3, new QTableWidgetItem(record.description));
        tableWidget->setItem(row, 4, new QTableWidgetItem(
            record.recordTime.toString("yyyy-MM-dd HH:mm:ss")));
    }
}

void ReportWidget::updateStatistics()
{
    // 总体统计
    double totalIncome = DataManager::instance().getTotalIncome();
    double totalExpense = DataManager::instance().getTotalExpense();
    double balance = DataManager::instance().getBalance();
    
    totalIncomeLabel->setText(QString("¥%1").arg(totalIncome, 0, 'f', 2));
    totalExpenseLabel->setText(QString("¥%1").arg(totalExpense, 0, 'f', 2));
    balanceLabel->setText(QString("¥%1").arg(balance, 0, 'f', 2));
    
    // 设置余额颜色
    if (balance > 0) {
        balanceLabel->setStyleSheet("color: #52c41a;");
    } else if (balance < 0) {
        balanceLabel->setStyleSheet("color: #ff4d4f;");
    } else {
        balanceLabel->setStyleSheet("color: #1890ff;");
    }
    
    // 月度统计
    int year = yearCombo->currentText().toInt();
    int month = monthCombo->currentIndex() + 1;
    
    double monthIncome = DataManager::instance().getMonthlyIncome(year, month);
    double monthExpense = DataManager::instance().getMonthlyExpense(year, month);
    
    monthIncomeLabel->setText(QString("¥%1").arg(monthIncome, 0, 'f', 2));
    monthExpenseLabel->setText(QString("¥%1").arg(monthExpense, 0, 'f', 2));
}

void ReportWidget::refreshData()
{
    loadMonthlyReport();
}

void ReportWidget::onMonthChanged()
{
    loadMonthlyReport();
}

void ReportWidget::onExportReport()
{
    QString fileName = QFileDialog::getSaveFileName(this, 
        "导出报表", 
        QString("财政报表_%1年%2月.txt")
            .arg(yearCombo->currentText())
            .arg(monthCombo->currentIndex() + 1, 2, 10, QChar('0')),
        "文本文件 (*.txt);;所有文件 (*)");
    
    if (fileName.isEmpty()) {
        return;
    }
    
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "错误", "无法创建文件！");
        return;
    }
    
    QTextStream out(&file);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    out.setCodec("UTF-8");  // Qt5需要设置编码
#else
    out.setEncoding(QStringConverter::Utf8);  // Qt6使用新API
#endif
    
    int year = yearCombo->currentText().toInt();
    int month = monthCombo->currentIndex() + 1;
    
    // 写入报表
    out << "========================================\n";
    out << "       教育基金会财政报表\n";
    out << "========================================\n\n";
    out << QString("报表月份：%1年%2月\n").arg(year).arg(month);
    out << QString("生成时间：%1\n\n").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));
    
    out << "----------------------------------------\n";
    out << "一、总体财务状况\n";
    out << "----------------------------------------\n";
    out << QString("总收入：¥%1\n").arg(totalIncomeLabel->text().mid(1));
    out << QString("总支出：¥%1\n").arg(totalExpenseLabel->text().mid(1));
    out << QString("账户余额：¥%1\n\n").arg(balanceLabel->text().mid(1));
    
    out << "----------------------------------------\n";
    out << "二、本月财务状况\n";
    out << "----------------------------------------\n";
    out << QString("本月收入：¥%1\n").arg(monthIncomeLabel->text().mid(1));
    out << QString("本月支出：¥%1\n\n").arg(monthExpenseLabel->text().mid(1));
    
    out << "----------------------------------------\n";
    out << "三、本月详细记录\n";
    out << "----------------------------------------\n\n";
    
    QList<FinanceRecord> records = DataManager::instance().getFinanceRecordsByMonth(year, month);
    
    if (records.isEmpty()) {
        out << "本月暂无财务记录。\n";
    } else {
        for (const auto& record : records) {
            out << QString("[%1] %2 | ¥%3\n")
                .arg(record.type)
                .arg(record.recordTime.toString("yyyy-MM-dd HH:mm"))
                .arg(record.amount, 0, 'f', 2);
            out << QString("    说明：%1\n\n").arg(record.description);
        }
    }
    
    out << "========================================\n";
    out << "报表结束\n";
    out << "========================================\n";
    
    file.close();
    
    QMessageBox::information(this, "成功", "报表已导出！");
}

