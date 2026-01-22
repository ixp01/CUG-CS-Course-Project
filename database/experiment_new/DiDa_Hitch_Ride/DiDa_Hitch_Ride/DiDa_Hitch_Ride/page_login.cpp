#include "page_login.h"
#include "ui_page_login.h"
#include<QMessageBox>

Page_Login::Page_Login(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Page_Login), m_ptrDriverSql(DriverSql::getinstance())
{
    ui->setupUi(this);
}

Page_Login::~Page_Login()
{
    delete ui;
}

void Page_Login::on_Btn_Exit_clicked()
{
    exit(0);
}


void Page_Login::on_Btn_Login_clicked()
{
    QString username = ui->line_User->text();
    QString password = ui->line_passWord->text();

    // 使用 DriverSql 类的方法验证用户名和密码
    if (m_ptrDriverSql->checkPassword(username, password))//使用数据库类的验证方法
    {
        // 登录成功
       emit sendLoginSuccess();
    }
    else
    {
        // 登录失败，密码错误
        QMessageBox::information(this, "登录失败", "密码错误，请重试");
    }


}


void Page_Login::on_Btn_Register_clicked()
{

    emit sendRegisterSuccess();

}

