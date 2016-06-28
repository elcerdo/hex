#include "board.h"
#include "graph_uct.h"
#include "loop_uct.h"
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

void
fatal_error(const bool& assert_value, const std::string& message)
{
    if (assert_value) return;
    std::cerr << "FATAL ERROR " << message << std::endl;
    std::exit(42);
}

using std::endl;
using std::cout;

int main(int argc, char* argv[])
{
    RandomEngine re(1234567890);
    cout << std::hex << re() << " " << re() << " " << re() << std::dec << std::endl;
    cout << endl << endl;

    Board board(3);
    BoardState state(board);
    cout << state << endl;

    { // check copy state
        const BoardState state_assign = state;
        const BoardState state_copy(state);
        fatal_error( hash_value(state) == hash_value(state_assign), "assign operator hash mismatch" );
        fatal_error( hash_value(state) == hash_value(state_copy), "copy constructor hash mismatch" );
    }

    { // check associativity
        const double time_start = get_double_time();

        Moves available_moves = state.getAvailableMoves();
        assert( available_moves.size() >= 4 );
        for (int kk=0; kk<1000; kk++)
        {
            std::random_shuffle(available_moves.begin(), available_moves.end(), [&re](const size_t size) { return re()%size; });

            BoardState state_aa = state;
            state_aa.playMove(available_moves[0]);
            state_aa.playMove(available_moves[1]);
            state_aa.playMove(available_moves[2]);
            state_aa.playMove(available_moves[3]);

            BoardState state_bb = state;
            state_bb.playMove(available_moves[2]);
            state_bb.playMove(available_moves[1]);
            state_bb.playMove(available_moves[0]);
            state_bb.playMove(available_moves[3]);

            BoardState state_cc = state;
            state_cc.playMove(available_moves[0]);
            state_cc.playMove(available_moves[3]);
            state_cc.playMove(available_moves[2]);
            state_cc.playMove(available_moves[1]);

            BoardState state_dd = state;
            state_dd.playMove(available_moves[2]);
            state_dd.playMove(available_moves[3]);
            state_dd.playMove(available_moves[0]);
            state_dd.playMove(available_moves[1]);

            fatal_error( hash_value(state_aa) == hash_value(state_bb), "associativity mismatch" );
            fatal_error( hash_value(state_aa) == hash_value(state_cc), "associativity mismatch" );
            fatal_error( hash_value(state_aa) == hash_value(state_dd), "associativity mismatch" );
        }

        const double time_end = get_double_time();
        cout << "associativity tests took " << clock_it(time_end-time_start) << endl;
    }
    cout << endl << endl;

    { // check play_one_sequence
        GraphData graph_data(.5);
        const GraphData::Node root = graph_data.get_or_create_node(state, re).second;
        cout << graph_data << endl;
        graph_data.print_from_root(cout, root);

        for (int kk=0; kk<10; kk++)
        {
            cout << "-----------" << endl;
            play_one_sequence(graph_data, re, state);
            cout << graph_data << endl;
            graph_data.print_from_root(cout, root);
        }
        cout << "-----------" << endl;

        {
            const int kk_max = 2000;

            const double time_start = get_double_time();
            for (int kk=0; kk<kk_max; kk++)
                play_one_sequence(graph_data, re, state);
            const double time_end = get_double_time();

            const double game_per_second = kk_max/(time_end-time_start);
            cout << "computed " << kk_max << " games in " << clock_it(time_end-time_start) << " (" << game_per_second << "games/s)" << endl;

        }

        cout << "-----------" << endl;
        cout << graph_data << endl;
        graph_data.print_from_root(cout, root,"" ,1);
    }

    return 0;
}

