#pragma once

#include "board.h"
#include <QWidget>
#include <QPolygonF>

class Viewer : public QWidget
{
    Q_OBJECT
    public:
        Viewer(const Board& board, QWidget* parent=NULL);

    protected:
        void paintEvent(QPaintEvent* event);
        void keyPressEvent(QKeyEvent* event);

    public:
        bool draw_edges;

    protected:
        const Board& board;
        QPolygonF polygon;

};

