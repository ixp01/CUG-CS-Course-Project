#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QMessageBox>
#include "data.h"

QT_BEGIN_NAMESPACE
namespace Ui { class LoginDialog; }
QT_END_NAMESPACE

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

    // 获取登录结果
    int getUserId() const { return userId; }
    QString getUsername() const { return username; }
    bool isAdmin() const { return adminMode; }

private slots:
    void on_loginButton_clicked();
    void on_registerButton_clicked();

private:
    Ui::LoginDialog *ui;
    
    // 登录结果
    int userId;
    QString username;
    bool adminMode;
    

};

#endif // LOGINDIALOG_H 
