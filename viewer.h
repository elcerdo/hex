#pragma once

#include "board.h"
#include <QGraphicsView>
#include <QGraphicsPolygonItem>

class Tile : public QGraphicsPolygonItem
{
    public:
        Tile(const Board::Graph::Node& node, const int& state, const bool& interactive, const QPolygonF& polygon, QGraphicsItem* item=NULL);
        int getState() const;
        void setState(const int state);
        Board::Graph::Node getNode() const;

    protected:
        void mousePressEvent(QGraphicsSceneMouseEvent* event);
        void update();

    protected:
        Board::Graph::Node node;
        int state;
        bool interactive;
};

class Viewer : public QGraphicsView
{
    Q_OBJECT
    public:
        Viewer(const Board& board, QWidget* parent=NULL);
        void displayState(const BoardState& state);
        void reconstructState(BoardState& state) const;
        void notifyChange(const Tile& tile);

    protected:
        void keyPressEvent(QKeyEvent* event);
        void drawForeground(QPainter* painter, const QRectF& rect);

    public:
        bool draw_edges;

    protected:
        const Board& board;
        QPolygonF polygon;
};

