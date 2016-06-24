#include "viewer.h"

#include <QPainter>
#include <cmath>
#include <QDebug>

Viewer::Viewer(const Board& board, QWidget* parent) : board(board), QWidget(parent)
{
    for (int kk=0; kk<6; kk++)
    {
        const double angle = 2*M_PI*static_cast<double>(kk)/6;
        polygon << .9*QPointF(std::cos(angle), std::sin(angle));
    }
}

QPointF
project(const Board::Coord& coord)
{
    static const QPointF ex(1+std::cos(M_PI/3), -sin(M_PI/3));
    static const QPointF ey(1+std::cos(M_PI/3), sin(M_PI/3));
    return static_cast<qreal>(coord.first)*ex + static_cast<qreal>(coord.second)*ey;
}

void
Viewer::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::HighQualityAntialiasing);

    painter.translate(width()/2, height()/2);
    painter.scale(15,15);
    painter.translate(-project(std::make_pair(board.size-1, board.size-1))/2.);

    typedef Board::Graph::NodeIt NodeIt;
    for (NodeIt ni(board.graph); ni!=lemon::INVALID; ++ni)
    {
        painter.save();
        painter.translate(project(board.coords[ni]));
        painter.drawConvexPolygon(polygon);
        painter.restore();
    }
}
