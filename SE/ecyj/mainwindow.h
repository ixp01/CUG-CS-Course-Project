#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlTableModel>
#include <QSqlRelationalTableModel>
#include <QTableView>
#include <QPushButton>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // 表格切换
    void onShowManufacturers();
    void onShowEquipment();
    void onShowTestRecords();
    void onShowTestItems();
    
    // 详细数据操作
    void onViewDetailData();
    
    // 数据操作
    void onAddRecord();
    void onDeleteRecord();
    void onRefreshData();
    
    // 功能操作
    void onImportCSV();
    void onExportPDF();
    void onPrintReport();
    void onStatistics();
    void onClearDatabase();
    void onAbout();

private:
    void setupUI();
    void setupMenuBar();
    void setupToolBar();
    void loadTableData(const QString &tableName);
    void updateDetailButtonState();
    
    QTableView *m_tableView;
    QSqlTableModel *m_currentModel;
    QString m_currentTable;
    
    QPushButton *m_btnManufacturers;
    QPushButton *m_btnEquipment;
    QPushButton *m_btnTestRecords;
    QPushButton *m_btnTestItems;
    QPushButton *m_btnViewDetail;
};

#endif // MAINWINDOW_H
