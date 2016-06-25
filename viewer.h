#pragma once

#include "scene.h"
#include <QGraphicsView>
#include <QPolygonF>

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

        QGraphicsScene* scene;
};

