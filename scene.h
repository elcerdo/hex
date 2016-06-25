#pragma once

#include "board.h"
#include <QGraphicsView>

class Scene : public QGraphicsScene
{
    Q_OBJECT
    public:
        Scene(const Board& board, QObject* parent=NULL);

    protected:
        QPolygonF polygon;
};

