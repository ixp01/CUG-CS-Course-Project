#include "page_register.h"
#include "ui_page_register.h"
#include"driversql.h"
#include<QMessageBox>

page_register::page_register(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::page_register), m_ptrDriverSql(DriverSql::getinstance())
{
    ui->setupUi(this);
}

page_register::~page_register()
{
    delete ui;
}
//退出键
void page_register::on_pushButton_2_clicked()
{
    exit(0);
}

//确定键
void page_register::on_pushButton_clicked()
{
    // 创建 UserInfo 结构体实例并填充用户输入的数据
    UserInfo info;
    info.m_username = ui->line_userName->text();
    info.m_password = ui->line_passWord->text();
    info.phone = ui->line_phone->text();

    // 检查用户名是否已存在
    if (m_ptrDriverSql->isExist(info.m_username)) {
        QMessageBox::information(nullptr, "警告", "该用户名已被注册");
        return;
    } else {
        // 如果用户名不存在，则添加用户到数据库
        bool success = m_ptrDriverSql->AddUser(info);
        if (success) {
            QMessageBox::information(nullptr, "信息", "您已注册成功");
            // 可以在这里添加额外的注册成功后的操作，比如清空输入框或者跳转到登录界面
            ui->line_userName->clear();
            ui->line_passWord->clear();
            ui->line_phone->clear();
        } else {
            QMessageBox::information(nullptr, "错误", "注册失败，请重试");
        }
    }

}

