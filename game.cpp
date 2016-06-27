#include "game.h"

#include <QDebug>
#include "hashed.h"

GameLoop::GameLoop(Player* player0_, Player* player1_, QObject* parent) : board(player0_->board), QThread(parent), player0(player0_), player1(player1_)
{
    Q_ASSERT( player0 );
    Q_ASSERT( player1 );
    Q_ASSERT( &player0->board == &player1->board );
    Q_ASSERT( board.borders.size() == 2 );

    player0->player = 0;
    player1->player = 1;
}

void
GameLoop::run()
{
    static const std::vector<QString> colors = {"white", "black"};

    qDebug() << "init";

    std::vector<Player*> players = {player0, player1};

    BoardState state(board);
    qDebug() << "hash" << make_hashed_pair(state).hash;
    for (Player* player : players) player->update(state);
    emit updateState(&state);

    int player_current = 0;
    int player_winner = state.getWinner();
    while (player_winner < 0)
    {
        qDebug() << "loop for" << player_current;
        emit statusChanged(QString("requesting move for %1").arg(colors[player_current]));

        const Move move = players[player_current]->getMove();
        qDebug() << "playing move";
        const bool valid = state.playMove(move);

        if (!valid)
        {
            qDebug() << "bad move";
            emit statusChanged(QString("bad move from %1").arg(colors[player_current]));
            player_winner = 1-player_current;
            break;
        }

        qDebug() << "hash" << make_hashed_pair(state).hash;
        for (Player* player : players) player->update(state);
        emit updateState(&state);

        player_winner = state.getWinner();

        player_current++;
        player_current %= state.board.borders.size();
    }

    emit statusChanged(QString("%1 wins!!").arg(colors[player_winner]));

    qDebug() << "finished";
}


