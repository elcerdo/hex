#include "game.h"

#include <QDebug>

GameLoop::GameLoop(Player* player0_, Player* player1_, QObject* parent) : board(player0_->board), QThread(parent), player0(player0_), player1(player1_)
{
    Q_ASSERT( player0 );
    Q_ASSERT( player1 );
    Q_ASSERT( &player0->board == &player1->board );

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
    BoardState state(board);

    std::vector<Player*> players = {player0, player1};

    int player_current = 0;
    while (!any_victory(state))
    {
        qDebug() << "loop for" << player_current;
        for (Player* player : players) player->update(state);
        emit updateState(&state);

        const Move move = players[player_current]->getMove();

        player_current++;
        player_current %= 2;
    }

    qDebug() << "finished";
}


