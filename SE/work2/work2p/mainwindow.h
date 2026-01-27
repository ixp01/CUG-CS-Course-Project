#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QStackedWidget>
#include <QLabel>
#include "donationwidget.h"
#include "applicationwidget.h"
#include "reportwidget.h"
#include "datamanager.h"

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
    void onNavigationItemClicked(int index);

private:
    void setupUI();
    void updateStatistics();
    
    Ui::MainWindow *ui;
    
    // 导航和内容区域
    QListWidget *navigationList;
    QStackedWidget *contentStack;
    
    // 统计信息标签
    QLabel *statsLabel;
    
    // 各个功能模块
    DonationWidget *donationWidget;
    ApplicationWidget *applicationWidget;
    ReportWidget *reportWidget;
};
#endif // MAINWINDOW_H
