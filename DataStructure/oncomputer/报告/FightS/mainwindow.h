#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QComboBox>
#include <QLineEdit>
#include <QDateTimeEdit>
#include <QTextBrowser>
#include <QLabel>
#include <QDockWidget>
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpinBox>
#include <algorithm>
#include "Manage.h"
#include<QFormLayout>
#include<QMessageBox>
#include<QDialogButtonBox>

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
    void showSyncMessageBox(const QString& type);  // 添加同步消息框函数声明

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    // 航班管理相关槽函数
    void on_addFlightButton_clicked();
    void on_deleteFlightButton_clicked();
    void on_importButton_clicked();
    void on_refreshFlightButton_clicked();
    
    // 票务操作相关槽函数
    void on_purchaseButton_clicked();
    void on_refundButton_clicked();
    void on_reserveButton_clicked();
    void on_processReserveButton_clicked();  // 处理预约的槽函数
    void on_refreshTicketButton_clicked();
    
    // 查询相关槽函数
    void on_searchButton_clicked();
    
    // 航班状态管理相关槽函数
    void on_changeFlightStatusButton_clicked();
    void showFlightStatusDialog();
    void notifyPassengersOfStatusChange(const QString& flightID, FlightStatus newStatus);
    QList<Flight*> findAlternativeFlights(const QString& originalFlightID);
    
    // 信号处理槽函数
    void onFlightAdded(Flight* flight);
    void onFlightRemoved(QString flightID);
    void onFlightStatusChanged(QString flightID, FlightStatus status);
    void onTicketPurchased(Ticket* ticket);
    void onTicketRefunded(QString ticketID);
    void onNotificationReceived(QString message);
    void onDataFileChanged();  // 数据文件变化处理
    void onDataModified();     // 数据修改处理（新增）

    // 排序相关槽函数
    void on_sortByPriceButton_clicked();
    void on_sortByTimeButton_clicked();
    void on_refreshSearchButton_clicked();

    void on_searchButton1_clicked();

    void on_saveButton_clicked();  // 添加保存按钮的槽函数声明

private:
    // UI 初始化相关函数
    void initializeUI();  // 初始化所有UI组件
    void setupUI();      // 设置基本UI布局
    void setupTables();  // 设置表格属性
    void setupStyles();  // 设置样式
    void connectSignals();  // 连接信号和槽

    // 表格相关函数
    QTableWidget* createTable(const QStringList& headers);
    void updateTable(QTableWidget* table, const QList<Flight*>& flights)
    {
        table->setRowCount(0);
        for (const Flight* flight : flights) {
            int row = table->rowCount();
            table->insertRow(row);
            setTableRow(table, row, flight);
        }
    }
    void setTableRow(QTableWidget* table, int row, const Flight* flight);
    void updateFlightTable();
    void updateResultTable(const QList<Flight*>& flights);
    void updateTicketTable(const QList<Ticket*>& tickets = QList<Ticket*>());  // 更新票务表格

    // 对话框相关函数
    void showAddFlightDialog();
    void showMessage(const QString& title, const QString& message, QMessageBox::Icon icon = QMessageBox::Information);
    QMap<QString, QWidget*> createFlightInputs(QWidget* parent);
    void addInputsToLayout(QFormLayout* layout, const QMap<QString, QWidget*>& inputs);
    Flight createFlightFromInputs(const QMap<QString, QWidget*>& inputs);

    // 其他辅助函数
    void updateFlightComboBox();
    void updateCityComboBoxes();

    // 添加新的保存函数声明
    void saveFlightsToFile();
    void saveTicketsToFile();
    
    // 通用刷新函数
    void refreshAllData();
    
    // 安全字符串转换函数
    QString safeFromStdString(const std::string& str, const QString& fallback = "N/A");

private:
    Ui::MainWindow *ui;
    FlightManager *flightManager;
    TicketManager *ticketManager;
    RouteGraphManager *routeManager;
    NotificationManager *notificationManager;

    // UI 组件
    QTableWidget *flightTable;
    QTableWidget *resultTable;
    QVBoxLayout *mainLayout;
    
    // 防循环机制
    bool isUpdatingFromFile;  // 标记是否正在从文件更新数据
};

#endif // MAINWINDOW_H
