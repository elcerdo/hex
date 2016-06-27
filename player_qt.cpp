#include "player_qt.h"

#include <QDebug>

PlayerQt::PlayerQt(const Board& board, Viewer* viewer_) : Player(board), QObject(viewer_), viewer(viewer_)
{
}

void
PlayerQt::update(const BoardState& state)
{
    qDebug() << "update qt player " << player;
}

Move
PlayerQt::getMove()
{
    qDebug() << "getMove qt player " << player;
}

