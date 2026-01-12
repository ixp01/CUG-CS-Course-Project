#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSpinBox>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFuture>
#include <QtConcurrent>
#include <QElapsedTimer>
#include "chessboard.h"
#include "nqueenssolver.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void startSolving();
    void updateProgress(int iteration, int conflicts, int restarts);
    void onSolutionFound(const QVector<int>& solution);

private:
    void setupUI();
    void createConnections();

    QSpinBox *nSpinBox;
    QPushButton *solveButton;
    QLabel *statusLabel;
    QLabel *iterationLabel;
    QLabel *conflictsLabel;
    QLabel *restartLabel;
    QLabel *timeLabel;
    ChessBoard *chessBoard;
    NQueensSolver *solver;
    
    int totalIterations;
    int totalConflicts;
    int totalRestarts;
    QElapsedTimer timer;
    int maxIterations;
};

#endif // MAINWINDOW_H
