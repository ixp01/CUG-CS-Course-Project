#include "nqueenssolver.h"
#include <numeric>
#include <chrono>

NQueensSolver::NQueensSolver(QObject *parent)
    : QObject(parent)
    , rng(std::chrono::steady_clock::now().time_since_epoch().count())
{
}

QVector<int> NQueensSolver::solve(int n, int maxIterations, int maxRestarts)
{
    this->n = n;
    int totalIterations = 0;
    int restarts = 0;
    int maxNoImprove = 2 * n;
    QVector<int> bestSolution;
    int minConflicts = n * n;
    bool solved = false;

    while (restarts <= maxRestarts) {
        initializeBoard(n);
        int lastImprovement = 0;
        int prevConflicts = n * n;
        int conflicts = 0;
        for (int i = 0; i < n; ++i) {
            conflicts += calculateConflicts(i, queens[i]);
        }
        conflicts /= 2;
        for (int iter = 0; iter < maxIterations; ++iter) {
            if (conflicts == 0) {
                emit solutionFound(queens);
                return queens;
            }
            int row = findMaxConflictRow();
            int newCol = findMinConflictCol(row);
            int oldCol = queens[row];
            if (newCol != oldCol) {
                updateConflicts(row, oldCol, newCol);
                queens[row] = newCol;
            }
            conflicts = 0;
            for (int i = 0; i < n; ++i) {
                conflicts += calculateConflicts(i, queens[i]);
            }
            conflicts /= 2;
            if (conflicts < minConflicts) {
                minConflicts = conflicts;
                bestSolution = queens;
                lastImprovement = iter;
            }
            totalIterations++;
            if (iter % 100 == 0) {
                emit progressUpdated(totalIterations, conflicts, restarts);
            }
            if (iter - lastImprovement > maxNoImprove) {
                break;
            }
        }
        restarts++;
    }
    // 只有没找到无冲突解时才emit一次
    if (minConflicts > 0 && !bestSolution.isEmpty()) {
        emit solutionFound(bestSolution);
    }
    return bestSolution;
}

void NQueensSolver::initializeBoard(int n)
{
    queens.resize(n);
    colConflicts.resize(n);
    diag1Conflicts.resize(2 * n - 1);
    diag2Conflicts.resize(2 * n - 1);
    std::iota(queens.begin(), queens.end(), 0);
    std::shuffle(queens.begin(), queens.end(), rng);
    std::fill(colConflicts.begin(), colConflicts.end(), 0);
    std::fill(diag1Conflicts.begin(), diag1Conflicts.end(), 0);
    std::fill(diag2Conflicts.begin(), diag2Conflicts.end(), 0);
    for (int i = 0; i < n; ++i) {
        colConflicts[queens[i]]++;
        diag1Conflicts[i + queens[i]]++;
        diag2Conflicts[n - 1 + i - queens[i]]++;
    }
}

int NQueensSolver::calculateConflicts(int row, int col) const
{
    return colConflicts[col] + diag1Conflicts[row + col] + diag2Conflicts[n - 1 + row - col] - 3;
}

int NQueensSolver::findMaxConflictRow() const
{
    int maxConflicts = -1;
    int maxRow = 0;
    for (int i = 0; i < n; ++i) {
        int conflicts = calculateConflicts(i, queens[i]);
        if (conflicts > maxConflicts) {
            maxConflicts = conflicts;
            maxRow = i;
        }
    }
    return maxRow;
}

int NQueensSolver::findMinConflictCol(int row)
{
    int minConflicts = n * n;
    QVector<int> minCols;
    for (int col = 0; col < n; ++col) {
        int conflicts = calculateConflicts(row, col);
        if (conflicts < minConflicts) {
            minConflicts = conflicts;
            minCols.clear();
            minCols.push_back(col);
        } else if (conflicts == minConflicts) {
            minCols.push_back(col);
        }
    }
    // 多个最小冲突列时随机选一个
    if (!minCols.isEmpty()) {
        std::uniform_int_distribution<int> dist(0, minCols.size() - 1);
        return minCols[dist(rng)];
    }
    return queens[row];
}

void NQueensSolver::updateConflicts(int row, int oldCol, int newCol)
{
    colConflicts[oldCol]--;
    diag1Conflicts[row + oldCol]--;
    diag2Conflicts[n - 1 + row - oldCol]--;
    colConflicts[newCol]++;
    diag1Conflicts[row + newCol]++;
    diag2Conflicts[n - 1 + row - newCol]++;
}

bool NQueensSolver::verifySolution() const
{
    for (int i = 0; i < n; ++i) {
        if (calculateConflicts(i, queens[i]) > 0) {
            return false;
        }
    }
    return true;
}
