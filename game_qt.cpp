#include "game_qt.h"

GameLoop::GameLoop(Player* player0_, Player* player1_, QObject* parent) : board(player0_->board), QThread(parent), player0(player0_), player1(player1_), state(NULL)
{
    Q_ASSERT( player0 );
    Q_ASSERT( player1 );
    Q_ASSERT( &player0->board == &player1->board );
    Q_ASSERT( board.borders.size() == 2 );

    player0->player = 0;
    player1->player = 1;
}

GameLoop::~GameLoop()
{
    if (player0) delete player0;
    if (player1) delete player1;
    if (state) delete state;
}

void
GameLoop::updateState(const BoardState& state)
{
    emit stateUpdated(&state);
}

void
GameLoop::updateMessage(const std::string& message)
{
    emit messageUpdated(QString::fromStdString(message));
}

void
GameLoop::run()
{
    if (state) delete state;
    state = new BoardState(board);
    run_game_loop(board, *state, player0, player1, this);
}


