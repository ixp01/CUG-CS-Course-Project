#ifndef NQUEENSSOLVER_H
#define NQUEENSSOLVER_H

#include <QObject>
#include <QVector>
#include <random>
#include <algorithm>

class NQueensSolver : public QObject
{
    Q_OBJECT

public:
    explicit NQueensSolver(QObject *parent = nullptr);
    QVector<int> solve(int n, int maxIterations = 1000, int maxRestarts = 10);

signals:
    void progressUpdated(int iteration, int conflicts, int restarts);
    void solutionFound(const QVector<int>& solution);

private:
    void initializeBoard(int n);
    int calculateConflicts(int row, int col) const;
    int findMaxConflictRow() const;
    int findMinConflictCol(int row);
    void updateConflicts(int row, int oldCol, int newCol);
    bool verifySolution() const;

    QVector<int> queens;
    QVector<int> colConflicts;
    QVector<int> diag1Conflicts;  // 主对角线冲突
    QVector<int> diag2Conflicts;  // 副对角线冲突
    int n;
    std::mt19937 rng;
};

#endif // NQUEENSSOLVER_H 