#include "player_uct.h"

#include "loop_uct.h"
#include <iomanip>

PlayerUct::PlayerUct(const Board& board, const double uct_constant, const double crunch_time_, const bool prune_, const size_t seed) : Player(board, "uct"), re(seed), graph_data(uct_constant), state(board), crunch_time(crunch_time_), prune(prune_)
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
    crunch_it_baby(graph_data, re, hashed_state, crunch_time, prune);
    std::cout << graph_data << std::endl;

    assert( graph_data.contains(hashed_state) );
    const GraphData::Node& root = graph_data.get_node(hashed_state);
    graph_data.print_from_root(std::cout, root, "** ", 1);
    const std::pair<Move, GraphData::UctData>& best_move = graph_data.get_best_move(root, re);
    const Board::Coord coord = state.board.coords[best_move.first];
    const double proba = 100*static_cast<double>(best_move.second.parent_score)/best_move.second.count;
    std::cout << "best move is " << coord.first << "x" << coord.second << " with proba " << std::setprecision(0) << std::fixed << proba << "%" << std::endl;

    return best_move.first;
}

