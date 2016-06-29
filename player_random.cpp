#include "player_random.h"

PlayerRandom::PlayerRandom(const Board& board, const size_t seed) : Player(board, "random"), state(board), re(seed)
{
}

void
PlayerRandom::update(const BoardState& state_)
{
    state = state_;
}

Move
PlayerRandom::getMove()
{
    const Moves moves = state.getAvailableMoves();
    return moves[ re()%moves.size() ];
}

