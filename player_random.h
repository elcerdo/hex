#pragma once

#include "player.h"
#include <random>

struct PlayerRandom : public Player
{
    BoardState state;
    std::default_random_engine re;

    PlayerRandom(const Board& board, const size_t seed);
    void update(const BoardState& state) override;
    Move getMove() override;
};

