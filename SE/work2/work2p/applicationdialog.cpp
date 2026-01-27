#include "applicationdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

ApplicationDialog::ApplicationDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUI();
}

ApplicationDialog::~ApplicationDialog()
{
}

void ApplicationDialog::setupUI()
{
    setWindowTitle("新增用款申请");
    setMinimumWidth(500);
    
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // 表单区域
    QFormLayout *formLayout = new QFormLayout();
    
    // 学校名称
    schoolEdit = new QLineEdit(this);
    schoolEdit->setPlaceholderText("请输入申请学校名称");
    formLayout->addRow("学校名称*:", schoolEdit);
    
    // 申请人
    applicantEdit = new QLineEdit(this);
    applicantEdit->setPlaceholderText("请输入申请人姓名");
    formLayout->addRow("申请人*:", applicantEdit);
    
    // 联系方式
    contactEdit = new QLineEdit(this);
    contactEdit->setPlaceholderText("请输入联系电话");
    formLayout->addRow("联系方式*:", contactEdit);
    
    // 申请金额
    amountSpin = new QDoubleSpinBox(this);
    amountSpin->setRange(0.01, 999999999.99);
    amountSpin->setDecimals(2);
    amountSpin->setSuffix(" 元");
    amountSpin->setValue(5000.00);
    formLayout->addRow("申请金额*:", amountSpin);
    
    // 用途说明
    purposeEdit = new QTextEdit(this);
    purposeEdit->setPlaceholderText("请详细说明用款用途（用于教学设备、图书采购等）");
    purposeEdit->setMaximumHeight(120);
    formLayout->addRow("用款用途*:", purposeEdit);
    
    mainLayout->addLayout(formLayout);
    
    // 提示信息
    QLabel *tipLabel = new QLabel("注：标记*为必填项\n提示：申请将经过合法性校验后方可审批", this);
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
    connect(submitBtn, &QPushButton::clicked, this, &ApplicationDialog::onSubmit);
    buttonLayout->addWidget(submitBtn);
    
    mainLayout->addLayout(buttonLayout);
}

bool ApplicationDialog::validateInput()
{
    if (schoolEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "提示", "请输入学校名称！");
        schoolEdit->setFocus();
        return false;
    }
    
    if (applicantEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "提示", "请输入申请人姓名！");
        applicantEdit->setFocus();
        return false;
    }
    
    if (contactEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "提示", "请输入联系方式！");
        contactEdit->setFocus();
        return false;
    }
    
    if (amountSpin->value() <= 0) {
        QMessageBox::warning(this, "提示", "申请金额必须大于0！");
        amountSpin->setFocus();
        return false;
    }
    
    if (purposeEdit->toPlainText().trimmed().isEmpty()) {
        QMessageBox::warning(this, "提示", "请输入用款用途！");
        purposeEdit->setFocus();
        return false;
    }
    
    // 简单的合法性校验
    QString purpose = purposeEdit->toPlainText().trimmed();
    QStringList validKeywords = {"教学", "教育", "学生", "图书", "设备", "器材", 
                                  "校舍", "维修", "培训", "课程", "资助", "助学"};
    bool isValid = false;
    for (const QString& keyword : validKeywords) {
        if (purpose.contains(keyword)) {
            isValid = true;
            break;
        }
    }
    
    if (!isValid) {
        auto reply = QMessageBox::question(this, "合法性确认", 
            "用款用途中未包含教育相关关键词，是否确认提交？",
            QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::No) {
            return false;
        }
    }
    
    return true;
}

void ApplicationDialog::onSubmit()
{
    if (!validateInput()) {
        return;
    }
    
    ApplicationRecord record;
    record.schoolName = schoolEdit->text().trimmed();
    record.applicant = applicantEdit->text().trimmed();
    record.contact = contactEdit->text().trimmed();
    record.amount = amountSpin->value();
    record.purpose = purposeEdit->toPlainText().trimmed();
    
    if (DataManager::instance().addApplication(record) > 0) {
        accept();
    } else {
        QMessageBox::critical(this, "错误", "提交失败，请重试！");
    }
}

