#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include <QWidget>
#include <QVector>

class ChessBoard : public QWidget
{
    Q_OBJECT

public:
    explicit ChessBoard(QWidget *parent = nullptr);
    void setSolution(const QVector<int>& solution);
    void setN(int n);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QVector<int> queens;
    int n;
    double cellSize;
};

#endif // CHESSBOARD_H 