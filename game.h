#pragma once

#include "player.h"
#include "board.h"

struct Status
{
    virtual void updateState(const BoardState& state) = 0;
    virtual void updateMessage(const std::string& message) = 0;
};

int run_game_loop(const Board& board, BoardState& state, Player* player0, Player* player1, Status* status);


