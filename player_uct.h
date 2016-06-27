#pragma once

#include "board.h"
#include "player.h"
#include "graph_uct.h"
#include <random>

struct PlayerUct : public Player
{
    std::default_random_engine re;
    GraphData graph_data;
    HashedPair<BoardState> hashed_state;

    PlayerUct(const Board& board, const double uct_constant, const size_t seed);
    void update(const BoardState& state) override;
    Move getMove() override;
};


