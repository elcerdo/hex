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
    int prune_skip;
    int prune_count;
    std::ostream& os;

    PlayerUct(const Board& board, const double uct_constant, const double crunch_time, const int prune_skip, const size_t seed, std::ostream& os=std::cout);
    void update(const BoardState& state) override;
    Move getMove() override;
};


