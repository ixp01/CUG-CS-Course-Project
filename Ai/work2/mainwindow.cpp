#include "mainwindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , solver(new NQueensSolver(this))
    , totalIterations(0)
    , totalConflicts(0)
    , totalRestarts(0)
    , maxIterations(1000000)
{
    setupUI();
    createConnections();
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUI()
{
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    QHBoxLayout *controlLayout = new QHBoxLayout();
    QHBoxLayout *infoLayout = new QHBoxLayout();

    nSpinBox = new QSpinBox(this);
    nSpinBox->setRange(200, 10000);
    nSpinBox->setValue(200);
    nSpinBox->setSingleStep(100);

    solveButton = new QPushButton("开始求解", this);

    statusLabel = new QLabel("就绪", this);
    iterationLabel = new QLabel("迭代次数: 0", this);
    conflictsLabel = new QLabel("冲突次数: 0", this);
    restartLabel = new QLabel("重启次数: 0", this);
    timeLabel = new QLabel("用时: 0 ms", this);

    QFont labelFont("Arial", 10);
    statusLabel->setFont(labelFont);
    iterationLabel->setFont(labelFont);
    conflictsLabel->setFont(labelFont);
    restartLabel->setFont(labelFont);
    timeLabel->setFont(labelFont);

    controlLayout->addWidget(new QLabel("n =", this));
    controlLayout->addWidget(nSpinBox);
    controlLayout->addWidget(solveButton);

    infoLayout->addWidget(statusLabel);
    infoLayout->addWidget(iterationLabel);
    infoLayout->addWidget(conflictsLabel);
    infoLayout->addWidget(restartLabel);
    infoLayout->addWidget(timeLabel);
    infoLayout->addStretch();

    chessBoard = new ChessBoard(this);
    mainLayout->addLayout(controlLayout);
    mainLayout->addLayout(infoLayout);
    mainLayout->addWidget(chessBoard);
    setWindowTitle("N皇后问题求解器 (n ≥ 200)");
    resize(800, 600);
}

void MainWindow::createConnections()
{
    connect(solveButton, &QPushButton::clicked, this, &MainWindow::startSolving);
    connect(solver, &NQueensSolver::progressUpdated, this, &MainWindow::updateProgress);
    connect(solver, &NQueensSolver::solutionFound, this, &MainWindow::onSolutionFound);
}

void MainWindow::startSolving()
{
    int n = nSpinBox->value();
    chessBoard->setN(n);
    solveButton->setEnabled(false);
    statusLabel->setText("正在求解...");
    totalIterations = 0;
    totalConflicts = 0;
    totalRestarts = 0;
    iterationLabel->setText("迭代次数: 0");
    conflictsLabel->setText("冲突次数: 0");
    restartLabel->setText("重启次数: 0");
    timeLabel->setText("用时: 0 ms");
    timer.restart();

    int maxRestarts = 10;
    QFuture<QVector<int>> future = QtConcurrent::run([=]() {
        return solver->solve(n, maxIterations, maxRestarts);
    });

    QFutureWatcher<QVector<int>> *watcher = new QFutureWatcher<QVector<int>>(this);
    connect(watcher, &QFutureWatcher<QVector<int>>::finished, [=]() {
        watcher->deleteLater();
    });
    watcher->setFuture(future);
}

void MainWindow::updateProgress(int iteration, int conflicts, int restarts)
{
    totalIterations = iteration;
    totalConflicts = conflicts;
    totalRestarts = restarts;
    iterationLabel->setText(QString("迭代次数: %1").arg(totalIterations));
    conflictsLabel->setText(QString("冲突次数: %1").arg(totalConflicts));
    restartLabel->setText(QString("重启次数: %1").arg(totalRestarts));
    statusLabel->setText("正在求解...");
}

void MainWindow::onSolutionFound(const QVector<int>& solution)
{
    chessBoard->setSolution(solution);
    solveButton->setEnabled(true);
    statusLabel->setText("求解完成！");
    qint64 elapsed = timer.elapsed();
    timeLabel->setText(QString("用时: %1 ms").arg(elapsed));
    QString message = QString("找到解决方案！\n"
                            "总迭代次数: %1\n"
                            "最终冲突数: %2\n"
                            "重启次数: %3\n"
                            "用时: %4 ms")
                            .arg(totalIterations)
                            .arg(totalConflicts)
                            .arg(totalRestarts)
                            .arg(elapsed);
    QMessageBox::information(this, "成功", message);
}
