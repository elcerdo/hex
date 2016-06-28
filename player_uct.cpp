#include "player_uct.h"

#include "loop_uct.h"

PlayerUct::PlayerUct(const Board& board, const double uct_constant, const size_t seed) : Player(board, "uct"), re(seed), graph_data(uct_constant), hashed_state(BoardState(board))
{
}

void
PlayerUct::update(const BoardState& state)
{
    hashed_state = make_hashed_pair(state);
}

Move
PlayerUct::getMove()
{
    crunch_it_baby(graph_data, re, hashed_state, 1);

    assert( graph_data.contains(hashed_state) );
    const GraphData::Node& root = graph_data.get_node(hashed_state);
    std::cout << "choosing best move" << std::endl;
    graph_data.print_from_root(std::cout, root, "** ", 1);
    const Move& best_move = graph_data.get_best_move(root, re);

    return best_move;
}

