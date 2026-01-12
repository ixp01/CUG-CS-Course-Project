#ifndef ASTAR_H
#define ASTAR_H
#include "searchalgo.h"

class AStar : public SearchAlgorithm {
    Q_OBJECT
public:
    explicit AStar(QObject *parent = nullptr) : SearchAlgorithm(parent) {}
    PathResult findPath(const QString &start, const QString &goal) override;
};

#endif // ASTAR_H 