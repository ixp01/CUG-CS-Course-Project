#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include"page_login.h"
#include"page_register.h"
#include<dlg_adddriver.h>
#include"dlg_out.h"
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
    //void onLoginNameReceived(const QString &username);

// private:
//     QSqlDatabase db;

    void on_pushButton_5_clicked();//更新全表

    void on_Btn_save_clicked();//保存车辆入场信息

    void on_Btn_clear_clicked();//清空数据库数据

    void on_Btn_delete_clicked();//删除对应表数据

    void on_Btn_update_clicked();//更新表项

    void on_Btn_search_clicked();//搜索车辆

    void on_Btn_getPhone_clicked();//获取保安手机

    void on_Btn_out_clicked();//设置出场

    void on_Btn_searchD_clicked();//搜寻车主电话


private:
    void updateTable();

private:
    Ui::MainWindow *ui;
    Page_Login m_dlgLogin;//登录界面
    page_register m_DlgRegister;//注册界面
    DriverSql *m_ptrDriverSql;//操作停车场数据库类
    Dlg_AddDriver m_dlgAddDriver;//入场界面类
    dlg_out m_dlgOut;//出场界面类
};
#endif // MAINWINDOW_H
