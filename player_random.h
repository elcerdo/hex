#pragma once

#include "player.h"
#include "utils.h"

struct PlayerRandom : public Player
{
    BoardState state;
    RandomEngine re;

    PlayerRandom(const Board& board, const size_t seed);
    void update(const BoardState& state) override;
    Move getMove() override;
};

