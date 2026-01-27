#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QMenuBar>
#include <QToolBar>
#include <QAction>
#include <QStatusBar>
#include <QMessageBox>
#include <QFileDialog>
#include "databasemanager.h"

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
    // 菜单栏操作
    void onNewInspection();
    void onSaveData();
    void onExportReport();
    void onPrintReport();
    void onExit();
    void onAbout();
    
    // 工具栏操作
    void onRefreshData();
    void onClearForm();
    
    // 设备信息操作
    void onSaveDeviceInfo();
    void onLoadDeviceInfo();
    
    // 实验配置操作
    void onAddExperimentItem();
    void onRemoveExperimentItem();
    void onSaveExperiment();
    
    // 结果采集操作
    void onCaptureImage();
    void onManualInput();
    void onSaveResults();
    
    // 查询统计操作
    void onSearchData();
    void onShowStatistics();
    
    // 数据库错误处理
    void onDatabaseError(const QString &error);

private:
    Ui::MainWindow *ui;
    
    // 数据库管理器
    DatabaseManager *dbManager;
    
    // 当前操作的数据ID
    int currentDeviceId;
    int currentExperimentId;
    
    // UI组件
    QTabWidget *tabWidget;
    
    // 菜单
    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *queryMenu;
    QMenu *helpMenu;
    
    // 工具栏
    QToolBar *mainToolBar;
    
    // 动作
    QAction *newAction;
    QAction *saveAction;
    QAction *exportAction;
    QAction *printAction;
    QAction *exitAction;
    QAction *refreshAction;
    QAction *clearAction;
    QAction *aboutAction;
    
    // 初始化函数
    void initUI();
    void initMenuBar();
    void initToolBar();
    void initConnections();
    void createDeviceInfoPage();
    void createExperimentPage();
    void createResultPage();
    void createQueryPage();
    void createReportPage();
    
    // 辅助函数
    void setupStyles();
    void updateStatusBar(const QString &message);
};
#endif // MAINWINDOW_H
