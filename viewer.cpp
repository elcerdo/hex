#include "viewer.h"

#include <QPainter>
#include <QKeyEvent>
#include <QDebug>
#include <cmath>

Tile::Tile(const QPolygonF& polygon, QGraphicsItem* item) : QGraphicsPolygonItem(polygon, item)
{
    setPen(QPen(Qt::black));
    setBrush(QBrush(Qt::gray));
}

QPointF
project(const Board::Coord& coord)
{
    static const QPointF ex(1+std::cos(M_PI/3), sin(M_PI/3));
    static const QPointF ey(1+std::cos(M_PI/3), -sin(M_PI/3));
    return coord.first*ex + coord.second*ey;
}

Viewer::Viewer(const Board& board, QWidget* parent) : draw_edges(true), board(board), QGraphicsView(NULL, parent)
{
    for (int kk=0; kk<6; kk++)
    {
        const double angle = 2*M_PI*static_cast<double>(kk)/6;
        polygon << .9*QPointF(std::cos(angle), std::sin(angle));
    }

    QGraphicsScene* scene = new QGraphicsScene(this);
    {
        typedef Board::Graph::NodeIt NodeIt;
        for (NodeIt ni(board.graph); ni!=lemon::INVALID; ++ni)
        {
            Tile* item = new Tile(polygon);
            item->setPos(project(board.coords[ni]));
            scene->addItem(item);
        }

    }

    setScene(scene);
    setRenderHint(QPainter::Antialiasing);
    setRenderHint(QPainter::HighQualityAntialiasing);
    scale(20, 20);
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

    QGraphicsView::keyPressEvent(event);
}

/*
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
*/
