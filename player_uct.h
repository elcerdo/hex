#pragma once

#include "board.h"
#include "player.h"
#include "graph_uct.h"
#include "utils.h"

struct PlayerUct : public Player
{
    GraphData graph_data;
    HashedPair<BoardState> hashed_state;
    RandomEngine re;

    PlayerUct(const Board& board, const double uct_constant, const size_t seed);
    void update(const BoardState& state) override;
    Move getMove() override;
};


