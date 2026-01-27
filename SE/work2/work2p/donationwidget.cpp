#include "donationwidget.h"
#include "ui_donationwidget.h"
#include "donationdialog.h"
#include <QHeaderView>

DonationWidget::DonationWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DonationWidget)
{
    setupUI();
    loadDonations();
}

DonationWidget::~DonationWidget()
{
    delete ui;
}

void DonationWidget::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // 顶部标题和筛选区域
    QHBoxLayout *topLayout = new QHBoxLayout();
    QLabel *titleLabel = new QLabel("捐助管理", this);
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
            this, &DonationWidget::onFilterChanged);
    topLayout->addWidget(filterCombo);
    
    mainLayout->addLayout(topLayout);
    
    // 表格
    tableWidget = new QTableWidget(this);
    tableWidget->setColumnCount(9);
    tableWidget->setHorizontalHeaderLabels({
        "ID", "捐助类型", "捐助人", "证件号", "联系方式", 
        "金额(元)", "状态", "提交时间", "证书编号"
    });
    
    tableWidget->horizontalHeader()->setStretchLastSection(true);
    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableWidget->setAlternatingRowColors(true);
    
    // 设置列宽
    tableWidget->setColumnWidth(0, 50);
    tableWidget->setColumnWidth(1, 80);
    tableWidget->setColumnWidth(2, 120);
    tableWidget->setColumnWidth(3, 150);
    tableWidget->setColumnWidth(4, 120);
    tableWidget->setColumnWidth(5, 100);
    tableWidget->setColumnWidth(6, 80);
    tableWidget->setColumnWidth(7, 150);
    
    connect(tableWidget, &QTableWidget::itemSelectionChanged, 
            this, &DonationWidget::onTableSelectionChanged);
    
    mainLayout->addWidget(tableWidget);
    
    // 底部按钮区域
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    
    btnAdd = new QPushButton("新增捐助申请", this);
    btnAdd->setMinimumHeight(35);
    btnAdd->setStyleSheet("QPushButton { background-color: #1890ff; color: white; "
                         "border: none; border-radius: 4px; padding: 8px 16px; }"
                         "QPushButton:hover { background-color: #40a9ff; }");
    connect(btnAdd, &QPushButton::clicked, this, &DonationWidget::onAddDonation);
    
    btnApprove = new QPushButton("审核通过", this);
    btnApprove->setMinimumHeight(35);
    btnApprove->setStyleSheet("QPushButton { background-color: #52c41a; color: white; "
                             "border: none; border-radius: 4px; padding: 8px 16px; }"
                             "QPushButton:hover { background-color: #73d13d; }"
                             "QPushButton:disabled { background-color: #d9d9d9; }");
    connect(btnApprove, &QPushButton::clicked, this, &DonationWidget::onApproveDonation);
    
    btnReject = new QPushButton("审核拒绝", this);
    btnReject->setMinimumHeight(35);
    btnReject->setStyleSheet("QPushButton { background-color: #ff4d4f; color: white; "
                            "border: none; border-radius: 4px; padding: 8px 16px; }"
                            "QPushButton:hover { background-color: #ff7875; }"
                            "QPushButton:disabled { background-color: #d9d9d9; }");
    connect(btnReject, &QPushButton::clicked, this, &DonationWidget::onRejectDonation);
    
    btnCertificate = new QPushButton("查看证书", this);
    btnCertificate->setMinimumHeight(35);
    btnCertificate->setStyleSheet("QPushButton { background-color: #faad14; color: white; "
                                 "border: none; border-radius: 4px; padding: 8px 16px; }"
                                 "QPushButton:hover { background-color: #ffc53d; }"
                                 "QPushButton:disabled { background-color: #d9d9d9; }");
    connect(btnCertificate, &QPushButton::clicked, this, &DonationWidget::onViewCertificate);
    
    buttonLayout->addWidget(btnAdd);
    buttonLayout->addWidget(btnApprove);
    buttonLayout->addWidget(btnReject);
    buttonLayout->addWidget(btnCertificate);
    buttonLayout->addStretch();
    
    mainLayout->addLayout(buttonLayout);
    
    updateButtonStates();
}

void DonationWidget::loadDonations(const QString& filter)
{
    tableWidget->setRowCount(0);
    
    QList<DonationRecord> donations;
    if (filter == "全部") {
        donations = DataManager::instance().getAllDonations();
    } else {
        donations = DataManager::instance().getDonationsByStatus(filter);
    }
    
    for (const auto& record : donations) {
        int row = tableWidget->rowCount();
        tableWidget->insertRow(row);
        
        tableWidget->setItem(row, 0, new QTableWidgetItem(QString::number(record.id)));
        tableWidget->setItem(row, 1, new QTableWidgetItem(record.donorType));
        tableWidget->setItem(row, 2, new QTableWidgetItem(record.donorName));
        tableWidget->setItem(row, 3, new QTableWidgetItem(record.idNumber));
        tableWidget->setItem(row, 4, new QTableWidgetItem(record.contact));
        tableWidget->setItem(row, 5, new QTableWidgetItem(QString::number(record.amount, 'f', 2)));
        
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
        tableWidget->setItem(row, 8, new QTableWidgetItem(record.certificateNo));
    }
}

void DonationWidget::refreshData()
{
    loadDonations(filterCombo->currentText());
}

void DonationWidget::onAddDonation()
{
    DonationDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        refreshData();
        QMessageBox::information(this, "成功", "捐助申请已提交！");
    }
}

void DonationWidget::onApproveDonation()
{
    int currentRow = tableWidget->currentRow();
    if (currentRow < 0) return;
    
    int id = tableWidget->item(currentRow, 0)->text().toInt();
    QString status = tableWidget->item(currentRow, 6)->text();
    
    if (status != "待审核") {
        QMessageBox::warning(this, "提示", "只能审核待审核状态的申请！");
        return;
    }
    
    // 生成证书编号
    QString certificateNo = QString("CERT%1%2")
        .arg(QDateTime::currentDateTime().toString("yyyyMMdd"))
        .arg(id, 4, 10, QChar('0'));
    
    if (DataManager::instance().updateDonationStatus(id, "已通过", certificateNo)) {
        QMessageBox::information(this, "成功", 
            QString("审核通过！\n证书编号：%1").arg(certificateNo));
        refreshData();
    } else {
        QMessageBox::critical(this, "错误", "审核失败！");
    }
}

void DonationWidget::onRejectDonation()
{
    int currentRow = tableWidget->currentRow();
    if (currentRow < 0) return;
    
    int id = tableWidget->item(currentRow, 0)->text().toInt();
    QString status = tableWidget->item(currentRow, 6)->text();
    
    if (status != "待审核") {
        QMessageBox::warning(this, "提示", "只能审核待审核状态的申请！");
        return;
    }
    
    auto reply = QMessageBox::question(this, "确认", "确定要拒绝该申请吗？",
                                      QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        if (DataManager::instance().updateDonationStatus(id, "已拒绝")) {
            QMessageBox::information(this, "成功", "已拒绝该申请！");
            refreshData();
        } else {
            QMessageBox::critical(this, "错误", "操作失败！");
        }
    }
}

void DonationWidget::onViewCertificate()
{
    int currentRow = tableWidget->currentRow();
    if (currentRow < 0) return;
    
    int id = tableWidget->item(currentRow, 0)->text().toInt();
    DonationRecord record = DataManager::instance().getDonationById(id);
    
    if (record.status != "已通过" || record.certificateNo.isEmpty()) {
        QMessageBox::warning(this, "提示", "该捐助尚未通过审核或没有证书！");
        return;
    }
    
    QString certificate = QString(
        "━━━━━━━━━━━━━━━━━━━━━━\n"
        "       教育基金会捐助证书\n"
        "━━━━━━━━━━━━━━━━━━━━━━\n\n"
        "证书编号：%1\n\n"
        "兹证明：\n\n"
        "    %2（%3）\n\n"
        "向本教育基金会捐助人民币：\n\n"
        "    ￥%4 元\n\n"
        "特此证明，以资鼓励！\n\n"
        "                教育基金会（盖章）\n"
        "                %5\n"
        "━━━━━━━━━━━━━━━━━━━━━━\n"
    ).arg(record.certificateNo)
     .arg(record.donorName)
     .arg(record.donorType)
     .arg(QString::number(record.amount, 'f', 2))
     .arg(record.approveTime.toString("yyyy年MM月dd日"));
    
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("捐助证书");
    msgBox.setText(certificate);
    msgBox.setIcon(QMessageBox::Information);
    msgBox.exec();
}

void DonationWidget::onFilterChanged(int index)
{
    Q_UNUSED(index);
    loadDonations(filterCombo->currentText());
}

void DonationWidget::onTableSelectionChanged()
{
    updateButtonStates();
}

void DonationWidget::updateButtonStates()
{
    int currentRow = tableWidget->currentRow();
    bool hasSelection = currentRow >= 0;
    
    if (hasSelection) {
        QString status = tableWidget->item(currentRow, 6)->text();
        btnApprove->setEnabled(status == "待审核");
        btnReject->setEnabled(status == "待审核");
        btnCertificate->setEnabled(status == "已通过");
    } else {
        btnApprove->setEnabled(false);
        btnReject->setEnabled(false);
        btnCertificate->setEnabled(false);
    }
}

