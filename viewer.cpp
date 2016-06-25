#include "viewer.h"

#include <QPainter>
#include <QKeyEvent>
#include <QDebug>
#include <cmath>
#include <QGraphicsSceneMouseEvent>

Tile::Tile(const Board::Graph::Node& node_, const int& state_, const bool& interactive_, const QPolygonF& polygon, QGraphicsItem* item) : node(node_), state(state_), interactive(interactive_), QGraphicsPolygonItem(polygon, item)
{
    setPen(QPen(Qt::black));
    update();
}

int
Tile::getState() const
{
    return state;
}

void
Tile::setState(const int state_)
{
    if (state_ == state) return;
    state = state_;
    update();
}

void
Tile::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (!interactive) return;
    state++;
    state %= 3;
    update();
    event->accept();
}

void
Tile::update()
{
    switch (state)
    {
        case 0:
            setBrush(QBrush(Qt::white));
            break;
        case 1:
            setBrush(QBrush(Qt::black));
            break;
        case 2:
            setBrush(QBrush(Qt::gray));
            break;
        default:
            Q_ASSERT(false);
    }

    QGraphicsPolygonItem::update();
}

QPointF
project(const Board::Coord& coord)
{
    static const QPointF ex(1+std::cos(M_PI/3), sin(M_PI/3));
    static const QPointF ey(1+std::cos(M_PI/3), -sin(M_PI/3));
    return coord.first*ex + coord.second*ey;
}

int
initial_state(const Board& board, const Board::Graph::Node& node)
{
    for (int state=0; state<board.borders.size(); state++)
    {
        if (board.borders[state].first == node) return state;
        if (board.borders[state].second == node) return state;
    }
    return 2;
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
            const int state = initial_state(board, ni);
            Tile* item = new Tile(ni, state, state == board.borders.size(), polygon);
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
