#include "board.h"
#include "graph_uct.h"
#include "player_uct.h"
#include <iostream>

struct move_coord
{
    const Board::Coord foo;

    move_coord(const Board& board, const Move& move) : foo(board.coords[move])
    {
    }
};

std::ostream&
operator<<(std::ostream& os, const move_coord& foo)
{
    return os << foo.foo.first << "x" << foo.foo.second;
}

std::ostream&
operator<<(std::ostream& os, const HashedPair<BoardState>& hashed_state)
{
    using std::endl;
    os << "<BoardState h=" << std::hex << hashed_state.hash << std::dec << " w=" << hashed_state.value.getWinner() << " ";

    bool first = true;
    os << "a=[";
    for (const Move& move : hashed_state.value.getAvailableMoves())
    {
        if (!first) os << ",";
        os << move_coord(hashed_state.value.board, move);
        first = false;
    }
    os << "]";

    os << ">";
    return os;
}

using std::endl;
using std::cout;

int main(int argc, char* argv[])
{
    RandomEngine re(1234567890);
    cout << std::hex << re() << " " << re() << " " << re() << std::dec << std::endl;

    Board board(3);
    BoardState state(board);
    cout << state << endl;
    { // check copy state
        const BoardState state_assign = state;
        const BoardState state_copy(state);
        assert( make_hashed_pair(state) == make_hashed_pair(state_assign) );
        assert( make_hashed_pair(state) == make_hashed_pair(state_copy) );
    }

    GraphData graph_data(.1);

    const GraphData::Node root = graph_data.get_or_create_node(state, re).second;
    cout << graph_data << endl;
    graph_data.print_from_root(cout, root);

    return 0;
}

