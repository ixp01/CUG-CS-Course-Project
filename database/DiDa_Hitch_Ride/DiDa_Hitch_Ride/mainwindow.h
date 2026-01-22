#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include"page_login.h"
#include"page_register.h"
#include<dlg_adddriver.h>
#include"driversql.h"
#include<QLabel>
//#include <QSqlDatabase>
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_Btn_exit_clicked();
    //初始化标签
    void onLoginNameReceived(const QString &username);

// private:
//     QSqlDatabase db;

    void on_pushButton_5_clicked();

    void on_Btn_save_clicked();

    void on_Btn_clear_clicked();

    void on_Btn_delete_clicked();

    void on_Btn_update_clicked();

    void on_Btn_search_clicked();

private:
    void updateTable();

private:
    Ui::MainWindow *ui;
    Page_Login m_dlgLogin;
    page_register m_DlgRegister;
    DriverSql *m_ptrDriverSql;
    Dlg_AddDriver m_dlgAddDriver;
    // QLabel *label_user;
};
#endif // MAINWINDOW_H
