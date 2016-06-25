#pragma once

#include "board.h"
#include <QGraphicsView>
#include <QGraphicsPolygonItem>

class Tile : public QGraphicsPolygonItem
{
    public:
        Tile(const QPolygonF& polygon, QGraphicsItem* item=NULL);

};

class Viewer : public QGraphicsView
{
    Q_OBJECT
    public:
        Viewer(const Board& board, QWidget* parent=NULL);

    protected:
        void keyPressEvent(QKeyEvent* event);

    public:
        bool draw_edges;

    protected:
        const Board& board;
        QPolygonF polygon;
};

