#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include <QString>
#include <QFile>
#include<QVector>
#include <QTextStream>
#include <QDebug>
#include<QSet>
#include<algorithm>
#include<QMessageBox>
#include <climits>           // 用于 INT_MAX
#include <queue>             // 用于 priority_queue
#include <utility>           // 用于 pair
#include <QProgressBar>      // 进度条
#include <QLabel>            // 状态标签
#include <QPushButton>       // 按钮
#include <QTimer>            // 定时器
// 前向声明
class CityGraph;
using namespace std;
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
    void loadcitydata();//读取数据
    void loadisneighbor();//读取数据

    //问题1
    void question1();
    void dfs(QSet<int>&v,int n,int loc);

    //问题2
    void question2();//问题2
    QVector<int> dijkstra(int start, int end, QVector<QVector<int>>& matrix);
    QVector<int> reconstructPath(const QVector<int>& prev, int start,int end) ;
    QVector<int> floydWarshall(int start, int end) ;
    QVector<int> getPath(int start, int end,QVector<QVector<int>> next) ;
    void dfs(int start,int end);
    void dfs(QVector<QPair<QVector<int>, int>>&paths,int n,int start,int end,QVector<int>path,int distance);

    //问题3
    void question3();
    void pass(int start,int end,int passcity);

    //问题4
    void question4();
    QVector<QVector<int>> yenKShortestPaths(int start, int end, int k);

    int nametoloc(QString n);//将名称转为位置
    int searchdistance(QString city1,QString city2);
    
    // 新增功能
    void updateProgressBar(int value);
    void showCalculationStatus(const QString& status);
    void enableAdvancedFeatures();
    
    // 创建一个 QMap 用于存储城市间的距离
    QMap<QString, QMap<QString, int>> cityDistances;
    QMap<QString, QMap<QString, int>> isneighbor;
    QVector<QVector<int>>cityDis;
    QVector<QVector<int>>neighbor;
    QVector<QString>name;
    
    // 新的图类实例
    CityGraph* m_cityGraph;
    
    // UI增强组件
    QProgressBar* m_progressBar;
    QLabel* m_statusLabel;
    QPushButton* m_visualizeButton;
    QTimer* m_animationTimer;

private slots:
    void on_search_clicked();

    void on_pushButton_3_clicked();

    void on_searchn_clicked();
    
    // 新的槽函数
    void on_visualize_clicked();
    void on_compare_algorithms_clicked();
    void animatePathFinding();

private:
    Ui::MainWindow *ui;
    
    // 私有辅助函数
    void setupEnhancedUI();
    void connectNewSignals();
    QString formatPathString(const QVector<int>& path, bool useGraphNames = false) const;
    void saveResultsToFile(const QString& filename, const QVector<QPair<QVector<int>, int>>& paths);
    void compareAlgorithmResults(int start, int end);
};
#endif // MAINWINDOW_H
