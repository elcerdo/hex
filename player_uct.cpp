#include "player_uct.h"

#include "loop_uct.h"

PlayerUct::PlayerUct(const Board& board, const double uct_constant, const double crunch_time_, const size_t seed) : Player(board, "uct"), re(seed), graph_data(uct_constant), state(board), crunch_time(crunch_time_)
{
}

void
PlayerUct::update(const BoardState& state_)
{
    state = state_;
}

Move
PlayerUct::getMove()
{
    const HashedPair<BoardState> hashed_state(state);

    std::cout << graph_data << std::endl;
    crunch_it_baby(graph_data, re, hashed_state, crunch_time);
    std::cout << graph_data << std::endl;

    assert( graph_data.contains(hashed_state) );
    const GraphData::Node& root = graph_data.get_node(hashed_state);
    graph_data.print_from_root(std::cout, root, "** ", 1);
    const Move best_move = graph_data.get_best_move(root, re);
    const Board::Coord coord = state.board.coords[best_move];
    std::cout << "best move " << coord.first << "x" << coord.second << std::endl;

    return best_move;
}

