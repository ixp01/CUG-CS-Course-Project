#ifndef ADMINMAINWINDOW_H
#define ADMINMAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QComboBox>
#include <QDateEdit>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QHeaderView>
#include <QMessageBox>
#include <QFileDialog>
#include <QAbstractItemView>
#include <QInputDialog>
#include <QProgressDialog>
#include "data.h"

QT_BEGIN_NAMESPACE
namespace Ui { class AdminMainWindow; }
QT_END_NAMESPACE

class AdminMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    AdminMainWindow(int managerId, const QString& username, QWidget *parent = nullptr);
    ~AdminMainWindow();

private slots:
    // 航班管理
    void onAddFlight();
    void onEditFlight();
    void onDeleteFlight();
    void onRefreshFlights();
    void onSearchFlight();
    void onFlightDoubleClicked(int row, int column);
    
    // 用户管理
    void onViewUser();
    void onResetPassword();
    void onRefreshUsers();
    void onSearchUser();
    void onUserDoubleClicked(int row, int column);
    
    // 统计报表
    void onGenerateStats();
    void onExportStats();
    
    // 系统管理
    void onBackupDatabase();
    void onRestoreDatabase();
    void onOptimizeDatabase();
    void onRefreshLog();
    void onClearLog();
    void onExportLog();
    
    // 菜单操作
    void onImportData();
    void onExportData();
    void onPreferences();
    void onChangePassword();
    void onAbout();
    void onUserManual();
    void onExit();

    void on_addFlightBtn_clicked();

    void on_editFlightBtn_clicked();

    void on_deleteFlightBtn_clicked();

private:
    Ui::AdminMainWindow *ui;
    
    // 管理员信息
    int currentManagerId;
    QString currentUsername;
    
    // 数据
    QVector<Flight> currentFlights;
    QVector<User> currentUsers;
    
    // 私有方法
    void setupUI();
    void setupTableHeaders();
    void loadFlights();
    void loadUsers();
    void displayFlights(const QVector<Flight>& flights);
    void displayUsers(const QVector<User>& users);
    void showFlightDialog(const Flight& flight = Flight(), bool isEdit = false);
    void generateOverallStats(const QDate& fromDate, const QDate& toDate);
    void generateFlightStats(const QDate& fromDate, const QDate& toDate);
    void generateUserStats(const QDate& fromDate, const QDate& toDate);
    void generateRevenueStats(const QDate& fromDate, const QDate& toDate);
    void updateSystemLog();
    
    // 表格列索引常量
    enum FlightColumn {
        COL_FLIGHT_NUMBER = 0,
        COL_AIRLINE,
        COL_DEPARTURE_CITY,
        COL_ARRIVAL_CITY,
        COL_DEPARTURE_TIME,
        COL_ARRIVAL_TIME,
        COL_TOTAL_SEATS,
        COL_AVAILABLE_SEATS,
        COL_PRICE,
        COL_STATUS
    };
    
    enum UserColumn {
        COL_USER_ID = 0,
        COL_USERNAME,
        COL_EMAIL,
        COL_PHONE,
        COL_USER_TYPE,
        COL_REGISTER_TIME
    };
};

#endif // ADMINMAINWINDOW_H 
