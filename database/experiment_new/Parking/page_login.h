#ifndef PAGE_LOGIN_H
#define PAGE_LOGIN_H

#include <QWidget>

namespace Ui {
class Page_login;
}

class Page_login : public QWidget
{
    Q_OBJECT

public:
    explicit Page_login(QWidget *parent = nullptr);
    ~Page_login();

private slots:
    void on_Btn_Exit_clicked();

    void on_Btn_Register_clicked();

    void on_Btn_Login_clicked();

signals:
    void sendLoginSuccess();
    void sendRegisterSuccess();

private:
    Ui::Page_login *ui;
};

#endif // PAGE_LOGIN_H
