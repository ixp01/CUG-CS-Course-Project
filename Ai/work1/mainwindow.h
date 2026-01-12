#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QList>
#include "ucs.h"
#include "greedy.h"
#include "astar.h"
#include <QGraphicsScene>

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
    void on_btnSearch_clicked();
    void handleLogMessage(const QString &msg);

private:
    Ui::MainWindow *ui;
    QGraphicsScene *scene;
    UCS *ucsAlgo;
    Greedy *greedyAlgo;
    AStar *astarAlgo;
    void initComboBoxes();
    void drawMap();
    void showResult(const PathResult &result);
    void logToConsole(const QString &msg);
    QList<QGraphicsLineItem*> highlightLines; // 保存高亮路径线条
};
#endif // MAINWINDOW_H
