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
    os << "<BoardState " << std::hex << hashed_state.hash << std::dec << " " << hashed_state.value.getWinner() << " ";

    bool first = true;
    os << "[";
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

    return 0;
}

