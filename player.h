#pragma once

#include "board.h"

struct Player
{
    const Board& board;
    int player;

    Player(const Board& board);
    virtual void update(const BoardState& state) = 0;
    virtual Move getMove() = 0;
};

