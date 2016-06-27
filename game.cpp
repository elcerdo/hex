#include "game.h"

#include <QDebug>

GameLoop::GameLoop(Player* player0_, Player* player1_, QObject* parent) : board(player0_->board), QThread(parent), player0(player0_), player1(player1_)
{
    Q_ASSERT( player0 );
    Q_ASSERT( player1 );
    Q_ASSERT( &player0->board == &player1->board );
    Q_ASSERT( board.borders.size() == 2 );

    player0->player = 0;
    player1->player = 1;
}

bool
any_victory(const BoardState& state)
{
    BoardState::Victories victories = state.checkVictories();
    return std::any_of(victories.begin(), victories.end(), [](const bool x) { return x; });
}

void
GameLoop::run()
{
    qDebug() << "init";

    std::vector<Player*> players = {player0, player1};

    BoardState state(board);
    for (Player* player : players) player->update(state);
    emit updateState(&state);

    int player_current = 0;
    while (!any_victory(state))
    {
        qDebug() << "loop for" << player_current;
        emit statusChanged(QString("requesting move for %1").arg(player_current == 0 ? "white" : "black"));

        const Move move = players[player_current]->getMove();
        qDebug() << "playing move";
        const bool valid = state.playMove(move);

        if (!valid)
        {
            qDebug() << "bad move";
            emit statusChanged(QString("bad move from %1").arg(player_current == 0 ? "white" : "black"));
            return;
        }

        for (Player* player : players) player->update(state);
        emit updateState(&state);

        player_current++;
        player_current %= state.board.borders.size();
    }

    const auto victories = state.checkVictories();
    qDebug() << QVector<bool>::fromStdVector(victories);
    emit statusChanged(QString("%1 wins!!").arg(victories[0] ? "white" : "black"));

    qDebug() << "finished";
}


