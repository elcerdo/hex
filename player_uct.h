#pragma once

#include "board.h"
#include "player.h"
#include "graph_uct.h"
#include "utils.h"

struct PlayerUct : public Player
{
    GraphData graph_data;
    BoardState state;
    RandomEngine re;
    double crunch_time;
    bool prune;

    PlayerUct(const Board& board, const double uct_constant, const double crunch_time, const bool prune, const size_t seed);
    void update(const BoardState& state) override;
    Move getMove() override;
};


