#ifndef GREEDY_H
#define GREEDY_H
#include "searchalgo.h"

class Greedy : public SearchAlgorithm {
    Q_OBJECT
public:
    explicit Greedy(QObject *parent = nullptr) : SearchAlgorithm(parent) {}
    PathResult findPath(const QString &start, const QString &goal) override;
};

#endif // GREEDY_H 