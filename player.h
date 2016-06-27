#pragma once

#include "board.h"

struct Player
{
    const Board& board;
    const std::string name;
    int player;

    Player(const Board& board, const std::string& name);
    virtual void update(const BoardState& state) = 0;
    virtual Move getMove() = 0;
};

