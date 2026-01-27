#include "applicationwidget.h"
#include "ui_applicationwidget.h"
#include "applicationdialog.h"
#include <QHeaderView>
#include <QInputDialog>

ApplicationWidget::ApplicationWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ApplicationWidget)
{
    setupUI();
    loadApplications();
}

ApplicationWidget::~ApplicationWidget()
{
    delete ui;
}

void ApplicationWidget::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // 顶部标题和筛选区域
    QHBoxLayout *topLayout = new QHBoxLayout();
    QLabel *titleLabel = new QLabel("用款申请管理", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    
    topLayout->addWidget(titleLabel);
    topLayout->addStretch();
    
    QLabel *filterLabel = new QLabel("状态筛选:", this);
    topLayout->addWidget(filterLabel);
    
    filterCombo = new QComboBox(this);
    filterCombo->addItems({"全部", "待审核", "已通过", "已拒绝"});
    connect(filterCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            this, &ApplicationWidget::onFilterChanged);
    topLayout->addWidget(filterCombo);
    
    mainLayout->addLayout(topLayout);
    
    // 表格
    tableWidget = new QTableWidget(this);
    tableWidget->setColumnCount(8);
    tableWidget->setHorizontalHeaderLabels({
        "ID", "学校名称", "申请人", "联系方式", 
        "金额(元)", "用途", "状态", "提交时间"
    });
    
    tableWidget->horizontalHeader()->setStretchLastSection(true);
    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableWidget->setAlternatingRowColors(true);
    
    // 设置列宽
    tableWidget->setColumnWidth(0, 50);
    tableWidget->setColumnWidth(1, 150);
    tableWidget->setColumnWidth(2, 100);
    tableWidget->setColumnWidth(3, 120);
    tableWidget->setColumnWidth(4, 100);
    tableWidget->setColumnWidth(5, 200);
    tableWidget->setColumnWidth(6, 80);
    
    connect(tableWidget, &QTableWidget::itemSelectionChanged, 
            this, &ApplicationWidget::onTableSelectionChanged);
    
    mainLayout->addWidget(tableWidget);
    
    // 底部按钮区域
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    
    btnAdd = new QPushButton("新增用款申请", this);
    btnAdd->setMinimumHeight(35);
    btnAdd->setStyleSheet("QPushButton { background-color: #1890ff; color: white; "
                         "border: none; border-radius: 4px; padding: 8px 16px; }"
                         "QPushButton:hover { background-color: #40a9ff; }");
    connect(btnAdd, &QPushButton::clicked, this, &ApplicationWidget::onAddApplication);
    
    btnApprove = new QPushButton("审核通过", this);
    btnApprove->setMinimumHeight(35);
    btnApprove->setStyleSheet("QPushButton { background-color: #52c41a; color: white; "
                             "border: none; border-radius: 4px; padding: 8px 16px; }"
                             "QPushButton:hover { background-color: #73d13d; }"
                             "QPushButton:disabled { background-color: #d9d9d9; }");
    connect(btnApprove, &QPushButton::clicked, this, &ApplicationWidget::onApproveApplication);
    
    btnReject = new QPushButton("审核拒绝", this);
    btnReject->setMinimumHeight(35);
    btnReject->setStyleSheet("QPushButton { background-color: #ff4d4f; color: white; "
                            "border: none; border-radius: 4px; padding: 8px 16px; }"
                            "QPushButton:hover { background-color: #ff7875; }"
                            "QPushButton:disabled { background-color: #d9d9d9; }");
    connect(btnReject, &QPushButton::clicked, this, &ApplicationWidget::onRejectApplication);
    
    buttonLayout->addWidget(btnAdd);
    buttonLayout->addWidget(btnApprove);
    buttonLayout->addWidget(btnReject);
    buttonLayout->addStretch();
    
    mainLayout->addLayout(buttonLayout);
    
    updateButtonStates();
}

void ApplicationWidget::loadApplications(const QString& filter)
{
    tableWidget->setRowCount(0);
    
    QList<ApplicationRecord> applications;
    if (filter == "全部") {
        applications = DataManager::instance().getAllApplications();
    } else {
        applications = DataManager::instance().getApplicationsByStatus(filter);
    }
    
    for (const auto& record : applications) {
        int row = tableWidget->rowCount();
        tableWidget->insertRow(row);
        
        tableWidget->setItem(row, 0, new QTableWidgetItem(QString::number(record.id)));
        tableWidget->setItem(row, 1, new QTableWidgetItem(record.schoolName));
        tableWidget->setItem(row, 2, new QTableWidgetItem(record.applicant));
        tableWidget->setItem(row, 3, new QTableWidgetItem(record.contact));
        tableWidget->setItem(row, 4, new QTableWidgetItem(QString::number(record.amount, 'f', 2)));
        tableWidget->setItem(row, 5, new QTableWidgetItem(record.purpose));
        
        QTableWidgetItem *statusItem = new QTableWidgetItem(record.status);
        if (record.status == "待审核") {
            statusItem->setForeground(QColor("#faad14"));
        } else if (record.status == "已通过") {
            statusItem->setForeground(QColor("#52c41a"));
        } else {
            statusItem->setForeground(QColor("#ff4d4f"));
        }
        tableWidget->setItem(row, 6, statusItem);
        
        tableWidget->setItem(row, 7, new QTableWidgetItem(
            record.submitTime.toString("yyyy-MM-dd HH:mm")));
    }
}

void ApplicationWidget::refreshData()
{
    loadApplications(filterCombo->currentText());
}

void ApplicationWidget::onAddApplication()
{
    ApplicationDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        refreshData();
        QMessageBox::information(this, "成功", "用款申请已提交！");
    }
}

void ApplicationWidget::onApproveApplication()
{
    int currentRow = tableWidget->currentRow();
    if (currentRow < 0) return;
    
    int id = tableWidget->item(currentRow, 0)->text().toInt();
    QString status = tableWidget->item(currentRow, 6)->text();
    
    if (status != "待审核") {
        QMessageBox::warning(this, "提示", "只能审核待审核状态的申请！");
        return;
    }
    
    // 检查余额是否充足
    ApplicationRecord app = DataManager::instance().getApplicationById(id);
    double balance = DataManager::instance().getBalance();
    
    if (balance < app.amount) {
        QMessageBox::warning(this, "提示", 
            QString("账户余额不足！\n当前余额：¥%1\n申请金额：¥%2")
            .arg(balance, 0, 'f', 2)
            .arg(app.amount, 0, 'f', 2));
        return;
    }
    
    bool ok;
    QString comment = QInputDialog::getText(this, "审核意见", 
        "请输入审核意见（可选）:", QLineEdit::Normal, "", &ok);
    
    if (ok) {
        if (DataManager::instance().updateApplicationStatus(id, "已通过", comment)) {
            QMessageBox::information(this, "成功", "审核通过！款项已支出。");
            refreshData();
        } else {
            QMessageBox::critical(this, "错误", "审核失败！");
        }
    }
}

void ApplicationWidget::onRejectApplication()
{
    int currentRow = tableWidget->currentRow();
    if (currentRow < 0) return;
    
    int id = tableWidget->item(currentRow, 0)->text().toInt();
    QString status = tableWidget->item(currentRow, 6)->text();
    
    if (status != "待审核") {
        QMessageBox::warning(this, "提示", "只能审核待审核状态的申请！");
        return;
    }
    
    bool ok;
    QString comment = QInputDialog::getText(this, "拒绝原因", 
        "请输入拒绝原因:", QLineEdit::Normal, "", &ok);
    
    if (ok) {
        if (DataManager::instance().updateApplicationStatus(id, "已拒绝", comment)) {
            QMessageBox::information(this, "成功", "已拒绝该申请！");
            refreshData();
        } else {
            QMessageBox::critical(this, "错误", "操作失败！");
        }
    }
}

void ApplicationWidget::onFilterChanged(int index)
{
    Q_UNUSED(index);
    loadApplications(filterCombo->currentText());
}

void ApplicationWidget::onTableSelectionChanged()
{
    updateButtonStates();
}

void ApplicationWidget::updateButtonStates()
{
    int currentRow = tableWidget->currentRow();
    bool hasSelection = currentRow >= 0;
    
    if (hasSelection) {
        QString status = tableWidget->item(currentRow, 6)->text();
        btnApprove->setEnabled(status == "待审核");
        btnReject->setEnabled(status == "待审核");
    } else {
        btnApprove->setEnabled(false);
        btnReject->setEnabled(false);
    }
}

