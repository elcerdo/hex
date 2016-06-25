#include "scene.h"

#include <QGraphicsPolygonItem>
#include <QKeyEvent>
#include <cmath>

QPointF
project(const Board::Coord& coord)
{
    static const QPointF ex(1+std::cos(M_PI/3), sin(M_PI/3));
    static const QPointF ey(1+std::cos(M_PI/3), -sin(M_PI/3));
    return coord.first*ex + coord.second*ey;
}

Scene::Scene(const Board& board, QObject* parent) : QGraphicsScene(parent)
{
    for (int kk=0; kk<6; kk++)
    {
        const double angle = 2*M_PI*static_cast<double>(kk)/6;
        polygon << .9*QPointF(std::cos(angle), std::sin(angle));
    }

    {
        typedef Board::Graph::NodeIt NodeIt;
        for (NodeIt ni(board.graph); ni!=lemon::INVALID; ++ni)
        {
            QGraphicsPolygonItem* item = new QGraphicsPolygonItem(polygon);
            item->setPen(QPen(Qt::black));
            item->setBrush(QBrush(Qt::gray));
            item->setPos(project(board.coords[ni]));
            addItem(item);
        }

    }
}

