#include "viewer.h"

#include <QPainter>
#include <cmath>
#include <QDebug>
#include <QKeyEvent>

Viewer::Viewer(const Board& board, QWidget* parent) : draw_edges(true), board(board), QWidget(parent)
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
    static const QPointF ex(1+std::cos(M_PI/3), sin(M_PI/3));
    static const QPointF ey(1+std::cos(M_PI/3), -sin(M_PI/3));
    return coord.first*ex + coord.second*ey;
}

void
Viewer::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == 68)
    {
        draw_edges = !draw_edges;
        update();
        event->accept();
        return;
    }
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

    {
        painter.save();
        painter.setPen(Qt::black);
        painter.setBrush(Qt::gray);
        typedef Board::Graph::NodeIt NodeIt;
        for (NodeIt ni(board.graph); ni!=lemon::INVALID; ++ni)
        {
            painter.save();
            painter.translate(project(board.coords[ni]));
            painter.drawConvexPolygon(polygon);
            painter.restore();
        }
        painter.restore();
    }

    if (draw_edges)
    {
        painter.save();
        painter.setPen(Qt::red);
        typedef Board::Graph::EdgeIt EdgeIt;
        for (EdgeIt ei(board.graph); ei!=lemon::INVALID; ++ei)
        {
            const Board::Coord coord_aa = board.coords[board.graph.u(ei)];
            const Board::Coord coord_bb = board.coords[board.graph.v(ei)];
            painter.drawLine(project(coord_aa), project(coord_bb));
        }
        painter.restore();
    }
}
