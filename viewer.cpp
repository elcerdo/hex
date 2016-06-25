#include "viewer.h"

#include <QPainter>
#include <QKeyEvent>
#include <QDebug>

Viewer::Viewer(const Board& board, QWidget* parent) : draw_edges(true), board(board), scene(NULL), QGraphicsView(NULL, parent)
{
    scene = new Scene(board, this);

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
