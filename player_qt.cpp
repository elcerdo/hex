#include "player_qt.h"

#include <QDebug>

PlayerQt::PlayerQt(const Board& board, Viewer* viewer) : Player(board, "human"), QObject(viewer), answer_mutex(), answer_condition(), move(lemon::INVALID)
{
    connect(this, SIGNAL(requestPlayerMove(const int)), viewer, SLOT(requestPlayerMove(const int)));
    connect(viewer, SIGNAL(gotPlayerMove(const int, const Move&)), this, SLOT(gotPlayerMove(const int, const Move&)));
}

void
PlayerQt::update(const BoardState& state)
{
    qDebug() << "update player_qt" << player;
}

Move
PlayerQt::getMove()
{
    qDebug() << "get move player_qt" << player;

    answer_mutex.lock();
    emit requestPlayerMove(player);
    answer_condition.wait(&answer_mutex);
    Move move_copy = move;
    move = lemon::INVALID;
    answer_mutex.unlock();

    qDebug() << "got move player_qt" << player;

    return move_copy;
}

void
PlayerQt::gotPlayerMove(const int answer_player, const Move& answer_move)
{
    if (player != answer_player) return;

    Q_ASSERT( move == lemon::INVALID );
    move = answer_move;
    answer_condition.wakeAll();
}

