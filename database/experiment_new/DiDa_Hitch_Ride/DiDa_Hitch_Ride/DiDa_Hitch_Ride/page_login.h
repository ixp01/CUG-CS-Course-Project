#ifndef PAGE_LOGIN_H
#define PAGE_LOGIN_H

#include <QWidget>
#include"driversql.h"

namespace Ui {
class Page_Login;
}

class Page_Login : public QWidget
{
    Q_OBJECT

public:
    explicit Page_Login(QWidget *parent = nullptr);
    ~Page_Login();

private slots:
    void on_Btn_Exit_clicked();

    void on_Btn_Login_clicked();

    void on_Btn_Register_clicked();

signals:
    void sendLoginSuccess();
    void sendRegisterSuccess();
    // void sendLoginName(const QString &username);

private:
    Ui::Page_Login *ui;
    DriverSql *m_ptrDriverSql;
};

#endif // PAGE_LOGIN_H
