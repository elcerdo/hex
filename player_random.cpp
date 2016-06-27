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
    std::vector<Move> moves;

    typedef Board::Graph::NodeIt NodeIt;
    for (NodeIt ni(board.graph); ni!=lemon::INVALID; ++ni)
    {
        if (state.states[ni] != board.borders.size()) continue;
        moves.push_back(ni);
    }

    return moves[ re()%moves.size() ];
}

