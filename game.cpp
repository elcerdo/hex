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

    std::vector<Player*> players = {player0, player1};

    BoardState state(board);
    for (Player* player : players) player->update(state);
    emit updateState(&state);

    int player_current = 0;
    while (!any_victory(state))
    {
        qDebug() << "loop for" << player_current;

        const Move move = players[player_current]->getMove();
        qDebug() << "playing move" << board.graph.valid(move);
        state.playMove(move);

        for (Player* player : players) player->update(state);
        emit updateState(&state);

        player_current++;
        player_current %= state.board.borders.size();
    }

    const auto victories = state.checkVictories();
    qDebug() << QVector<bool>::fromStdVector(victories);
    qDebug() << ( victories[0] ? "white" : "black" ) << "wins!!";
    qDebug() << "finished";
}


