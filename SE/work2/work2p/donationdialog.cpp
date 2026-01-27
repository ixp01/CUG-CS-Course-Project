#include "donationdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

DonationDialog::DonationDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUI();
}

DonationDialog::~DonationDialog()
{
}

void DonationDialog::setupUI()
{
    setWindowTitle("新增捐助申请");
    setMinimumWidth(500);
    
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // 表单区域
    QFormLayout *formLayout = new QFormLayout();
    
    // 捐助类型
    typeCombo = new QComboBox(this);
    typeCombo->addItems({"企业", "个人"});
    connect(typeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &DonationDialog::onDonorTypeChanged);
    formLayout->addRow("捐助类型*:", typeCombo);
    
    // 姓名/企业名称
    nameEdit = new QLineEdit(this);
    nameEdit->setPlaceholderText("请输入捐助人姓名或企业名称");
    formLayout->addRow("捐助人*:", nameEdit);
    
    // 证件号
    idEdit = new QLineEdit(this);
    idEdit->setPlaceholderText("请输入身份证号或营业执照号");
    formLayout->addRow("证件号*:", idEdit);
    
    // 联系方式
    contactEdit = new QLineEdit(this);
    contactEdit->setPlaceholderText("请输入联系电话或邮箱");
    formLayout->addRow("联系方式:", contactEdit);
    
    // 捐助金额
    amountSpin = new QDoubleSpinBox(this);
    amountSpin->setRange(0.01, 999999999.99);
    amountSpin->setDecimals(2);
    amountSpin->setSuffix(" 元");
    amountSpin->setValue(1000.00);
    formLayout->addRow("捐助金额*:", amountSpin);
    
    // 捐助说明
    descEdit = new QTextEdit(this);
    descEdit->setPlaceholderText("请输入捐助意向说明（可选）");
    descEdit->setMaximumHeight(100);
    formLayout->addRow("捐助说明:", descEdit);
    
    mainLayout->addLayout(formLayout);
    
    // 提示信息
    QLabel *tipLabel = new QLabel("注：标记*为必填项", this);
    tipLabel->setStyleSheet("color: #8c8c8c; font-size: 12px;");
    mainLayout->addWidget(tipLabel);
    
    mainLayout->addStretch();
    
    // 按钮区域
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    
    cancelBtn = new QPushButton("取消", this);
    cancelBtn->setMinimumSize(80, 32);
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
    buttonLayout->addWidget(cancelBtn);
    
    submitBtn = new QPushButton("提交申请", this);
    submitBtn->setMinimumSize(80, 32);
    submitBtn->setStyleSheet("QPushButton { background-color: #1890ff; color: white; "
                            "border: none; border-radius: 4px; }"
                            "QPushButton:hover { background-color: #40a9ff; }");
    connect(submitBtn, &QPushButton::clicked, this, &DonationDialog::onSubmit);
    buttonLayout->addWidget(submitBtn);
    
    mainLayout->addLayout(buttonLayout);
}

void DonationDialog::onDonorTypeChanged(int index)
{
    if (index == 0) { // 企业
        idEdit->setPlaceholderText("请输入营业执照号");
    } else { // 个人
        idEdit->setPlaceholderText("请输入身份证号");
    }
}

bool DonationDialog::validateInput()
{
    if (nameEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "提示", "请输入捐助人姓名或企业名称！");
        nameEdit->setFocus();
        return false;
    }
    
    if (idEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "提示", "请输入证件号！");
        idEdit->setFocus();
        return false;
    }
    
    if (amountSpin->value() <= 0) {
        QMessageBox::warning(this, "提示", "捐助金额必须大于0！");
        amountSpin->setFocus();
        return false;
    }
    
    return true;
}

void DonationDialog::onSubmit()
{
    if (!validateInput()) {
        return;
    }
    
    DonationRecord record;
    record.donorType = typeCombo->currentText();
    record.donorName = nameEdit->text().trimmed();
    record.idNumber = idEdit->text().trimmed();
    record.contact = contactEdit->text().trimmed();
    record.amount = amountSpin->value();
    record.description = descEdit->toPlainText().trimmed();
    
    if (DataManager::instance().addDonation(record) > 0) {
        accept();
    } else {
        QMessageBox::critical(this, "错误", "提交失败，请重试！");
    }
}

