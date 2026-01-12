#ifndef UCS_H
#define UCS_H
#include "searchalgo.h"

class UCS : public SearchAlgorithm {
    Q_OBJECT
public:
    explicit UCS(QObject *parent = nullptr) : SearchAlgorithm(parent) {}
    PathResult findPath(const QString &start, const QString &goal) override;
};

#endif // UCS_H 