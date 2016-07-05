#include "player_uct.h"

#include "loop_uct.h"
#include <iomanip>

PlayerUct::PlayerUct(const Board& board, const double uct_constant, const double crunch_time_, const int prune_skip_, const size_t seed, std::ostream& os_) : Player(board, "uct"), re(seed), graph_data(uct_constant), state(board), crunch_time(crunch_time_), prune_skip(prune_skip_), prune_count(0), os(os_)
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

    os << graph_data << std::endl;
    const bool should_prune = ( prune_skip > 1 ? prune_count%prune_skip == 0 : true );
    crunch_it_baby(graph_data, re, hashed_state, crunch_time, prune_count > 0 && should_prune, os);
    os << graph_data << std::endl;

    assert( graph_data.contains(hashed_state) );
    const GraphData::Node& root = graph_data.get_node(hashed_state);
    graph_data.print_from_root(os, root, "** ", 1);
    const std::pair<Move, GraphData::UctData>& best_move = graph_data.get_best_move(root, re);
    const Board::Coord coord = state.board.coords[best_move.first];
    const double proba = 100*static_cast<double>(best_move.second.parent_score)/best_move.second.count;
    os << "best move is " << coord << " with proba " << std::setprecision(0) << std::fixed << proba << "%" << std::endl;

    prune_count++;

    return best_move.first;
}

