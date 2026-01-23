#include "logindialog.h"
#include "ui_login.h"
#include "../include/usermainwindow.h"
#include "../include/adminmainwindow.h"

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoginDialog)
    , userId(-1)
    , adminMode(false)
{
    ui->setupUi(this);
    
    // 设置窗口属性
    setFixedSize(400, 300);
    setModal(true);
    
    // 注意：on_loginButton_clicked和on_registerButton_clicked会自动连接
    
    // 设置默认焦点
    ui->usernameEdit->setFocus();
}

LoginDialog::~LoginDialog()
{
    delete ui;
}




void LoginDialog::on_loginButton_clicked()
{
    // 获取输入的用户名和密码
    QString username = ui->usernameEdit->text().trimmed();
    QString password = ui->passwordEdit->text();
    
    // 空白输入验证
    if (username.isEmpty()) {
        QMessageBox::warning(this, "输入错误", "请输入用户名！");
        ui->usernameEdit->setFocus();
        return;
    }
    
    if (password.isEmpty()) {
        QMessageBox::warning(this, "输入错误", "请输入密码！");
        ui->passwordEdit->setFocus();
        return;
    }
    
    // 获取数据库管理实例
    DatabaseManager& dbManager = DatabaseManager::instance();
    
    // 根据选择的用户类型进行认证
    if (ui->adminRadio->isChecked()) {
        // 管理员登录
        int managerId = dbManager.authenticateManager(username, password);
        if (managerId > 0) {
            // 登录成功，打开管理员界面
            this->hide(); // 隐藏登录对话框
            
            AdminMainWindow* adminWindow = new AdminMainWindow(managerId, username);
            adminWindow->setAttribute(Qt::WA_DeleteOnClose); // 窗口关闭时自动删除
            adminWindow->show();
            
            // 关闭登录对话框
            this->accept();
        } else {
            // 登录失败
            QMessageBox::warning(this, "登录失败", "管理员用户名或密码错误！");
            ui->passwordEdit->clear();
            ui->passwordEdit->setFocus();
        }
    } else {
        // 普通用户登录 (userRadio被选中或默认)
        int userId = dbManager.authenticateUser(username, password);
        if (userId > 0) {
            // 登录成功，打开用户界面
            this->hide(); // 隐藏登录对话框
            
            UserMainWindow* userWindow = new UserMainWindow(userId, username);
            userWindow->setAttribute(Qt::WA_DeleteOnClose); // 窗口关闭时自动删除
            userWindow->show();
            
            // 关闭登录对话框
            this->accept();
        } else {
            // 登录失败
            QMessageBox::warning(this, "登录失败", "用户名或密码错误！");
            ui->passwordEdit->clear();
            ui->passwordEdit->setFocus();
        }
    }
}


void LoginDialog::on_registerButton_clicked()
{
    // 注册功能暂时显示提示信息
    QMessageBox::information(this, "提示", "注册功能正在开发中...");
}

