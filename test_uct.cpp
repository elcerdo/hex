#include "board.h"
#include "graph_uct.h"
#include "player_uct.h"
#include <iostream>

std::ostream&
display(std::ostream& os, const Move& move, const Board& board)
{
    const Board::Coord& coord = board.coords[move];
    return os << coord.first << "x" << coord.second;
}

std::ostream&
operator<<(std::ostream& os, const HashedPair<BoardState>& hashed_state)
{
    using std::endl;
    os << "[" << std::hex << hashed_state.hash << std::dec << " " << hashed_state.value.getWinner() << " " << hashed_state.value.getAvailableMoves().size() << "]";
    return os;
}

using std::endl;
using std::cout;

int main(int argc, char* argv[])
{
    RandomEngine re(123);
    cout << std::hex << re() << " " << re() << " " << re() << std::dec << std::endl;

    Board board(3);
    BoardState state(board);
    cout << state << endl;

    return 0;
}

