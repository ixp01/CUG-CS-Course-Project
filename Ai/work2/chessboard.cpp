#include "chessboard.h"
#include <QPainter>
#include <QPaintEvent>

ChessBoard::ChessBoard(QWidget *parent)
    : QWidget(parent)
    , n(8)
    , cellSize(0)
{
    setMinimumSize(400, 400);
}

void ChessBoard::setSolution(const QVector<int>& solution)
{
    queens = solution;
    update();
}

void ChessBoard::setN(int n)
{
    this->n = n;
    update();
}

void ChessBoard::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 计算单元格大小
    cellSize = qMin(width(), height()) / static_cast<double>(n);

    // 绘制棋盘
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            QRectF cell(j * cellSize, i * cellSize, cellSize, cellSize);
            
            // 交替绘制黑白格子
            painter.fillRect(cell, (i + j) % 2 == 0 ? Qt::white : Qt::lightGray);
            
            // 如果有皇后，绘制皇后
            if (i < queens.size() && queens[i] == j) {
                painter.setPen(Qt::red);
                painter.setBrush(Qt::red);
                painter.drawEllipse(cell.adjusted(cellSize * 0.1, cellSize * 0.1,
                                                -cellSize * 0.1, -cellSize * 0.1));
            }
        }
    }
} 