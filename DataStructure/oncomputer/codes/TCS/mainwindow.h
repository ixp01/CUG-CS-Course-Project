#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QComboBox>
#include <QSpinBox>
#include <QTableWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QProgressBar>
#include <QSplitter>
#include <QFileDialog>
#include <QMessageBox>
#include <QDateTime>
#include<QHeaderView>
#include "citygraph.h"

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
    void loadData();
    void validateWuhan();
    void findShortestPath();
    void findAllPaths();
    void findKShortestPaths();
    void exportResults();
    void onAlgorithmChanged();
    void onStartCityChanged();
    void onEndCityChanged();

private:
    void setupUI();
    void setupControlPanel();
    void setupResultPanel();
    void setupStatusBar();
    void updateCityList();
    void displayResults(const std::vector<PathInfo>& paths, 
                       const PathInfo& dijkstraResult,
                       const PathInfo& floydResult);
    void displayPath(const PathInfo& pathInfo, const QString& algorithmName);
    QString pathToString(const PathInfo& pathInfo);
    void clearResults();
    void showProgress(const QString& message);
    void hideProgress();

    Ui::MainWindow *ui;
    CityGraph graph;
    
    // UI控件
    QWidget* centralWidget;
    QSplitter* mainSplitter;
    
    // 左侧控制面板
    QGroupBox* controlGroup;
    QComboBox* startComboBox;
    QComboBox* endComboBox;
    QComboBox* algorithmComboBox;
    QComboBox* avoidComboBox;
    QSpinBox* kSpinBox;
    QSpinBox* maxDepthSpinBox;
    
    // 功能按钮
    QPushButton* loadButton;
    QPushButton* validateButton;
    QPushButton* shortestPathButton;
    QPushButton* allPathsButton;
    QPushButton* kPathsButton;
    QPushButton* exportButton;
    
    // 右侧结果展示
    QGroupBox* resultGroup;
    QTableWidget* resultTable;
    QTextEdit* detailText;
    
    // 状态显示
    QLabel* statusLabel;
    QProgressBar* progressBar;
    
    // 数据存储
    std::vector<PathInfo> lastAllPaths;
    PathInfo lastDijkstraResult;
    PathInfo lastFloydResult;
    QString lastStartCity;
    QString lastEndCity;
};

#endif // MAINWINDOW_H
