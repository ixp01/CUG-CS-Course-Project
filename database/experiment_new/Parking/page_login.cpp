#include "page_login.h"
#include "ui_page_login.h"

Page_login::Page_login(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Page_login)
{
    ui->setupUi(this);
}

Page_login::~Page_login()
{
    delete ui;
}

void Page_login::on_Btn_Exit_clicked()
{
     exit(0);
}


void Page_login::on_Btn_Register_clicked()
{
  emit sendRegisterSuccess();
}


void Page_login::on_Btn_Login_clicked()
{
   emit sendLoginSuccess();
}

