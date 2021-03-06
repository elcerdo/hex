#include "board.h"
#include "graph_uct.h"
#include "loop_uct.h"
#include <iostream>

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

    { // check board state hash
        const BoardState state_copy(state);
        BoardState state_assign(board);
        state_assign = state;
        fatal_error( hash_value(state) == hash_value(state_assign), "board state assign operator hash mismatch" );
        fatal_error( hash_value(state) == hash_value(state_copy), "board state copy constructor hash mismatch" );
    }

    { // check board hash
        Board board_other(3);
        fatal_error( hash_value(board) == hash_value(board_other), "board hash mismatch" );

        BoardState state_other(board_other);
        BoardState state_origin(board);
        state_origin = state_other;
        fatal_error( hash_value(state) == hash_value(state_other), "board state initial hash mismatch" );
        fatal_error( hash_value(state) == hash_value(state_origin), "board state initial hash mismatch" );

        {
            const Move move = board.graph.nodeFromId(3);
            state_other.playMove(move);
            state_origin.playMove(move);
            fatal_error( hash_value(state_origin) == hash_value(state_other), "board state origin move hash mismatch" );
        }

        {
            const Move move = board_other.graph.nodeFromId(2);
            state_other.playMove(move);
            state_origin.playMove(move);
            fatal_error( hash_value(state_origin) == hash_value(state_other), "board state other move hash mismatch" );
        }
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
        graph_data.print_from_root(cout, root, "", 1);
    }
    cout << endl << endl;

    { // test crunch_it_baby
        GraphData graph_data(.5);
        const GraphData::Node root = graph_data.get_or_create_node(state, re).second;
        cout << graph_data << endl;

        crunch_it_baby(graph_data, re, state, 3, false, std::cout);

        cout << graph_data << endl;
        graph_data.print_from_root(cout, root, "", 1);

        const std::pair<Move, GraphData::UctData> best_move = graph_data.get_best_move(root, re);
        cout << "best move is " << board.coords[best_move.first] << endl;
    }
    cout << endl << endl;

    { // test union_find
        typedef Board::Graph::NodeMap<int> Components;
        typedef lemon::UnionFind<Components> UnionFind;

        Components components(board.graph, 0);
        UnionFind union_find(components);

        for (const Board::Border& border : board.borders)
            cout << "insert " << union_find.insert(border.first) << " " << union_find.insert(border.second) << endl;

        for (const Board::Border& border : board.borders)
        {
            cout << "find ";
            cout << components[border.first] << "/" << union_find.find(border.first) << "(" << union_find.size(border.first)  << ") ";
            cout << components[border.second] << "/" << union_find.find(border.second) << "(" << union_find.size(border.second)  << ")" << endl;
        }

        const Board::Node& aa = board.graph.nodeFromId(2);
        const Board::Node& bb = board.graph.nodeFromId(3);
        cout << "aa " << board.coords[aa] << " bb " << board.coords[bb] << endl;
        cout << "insert " << union_find.insert(aa) << " " << union_find.insert(bb) << endl;
        cout << "join " << union_find.join(aa, bb) << endl;
        cout << "join " << union_find.join(bb, aa) << endl;
        cout << "join " << union_find.join(aa, board.borders[1].second) << endl;

        cout << "find ";
        cout << components[aa] << "/" << union_find.find(aa) << "(" << union_find.size(aa) << ") ";
        cout << components[bb] << "/" << union_find.find(bb) << "(" << union_find.size(bb) << ")" << endl;
        for (const Board::Border& border : board.borders)
        {
            cout << "find ";
            cout << components[border.first] << "/" << union_find.find(border.first) << "(" << union_find.size(border.first)  << ") ";
            cout << components[border.second] << "/" << union_find.find(border.second) << "(" << union_find.size(border.second)  << ")" << endl;
        }

        cout << "join " << union_find.join(bb, board.borders[1].first) << endl;

        cout << "find ";
        cout << components[aa] << "/" << union_find.find(aa) << "(" << union_find.size(aa) << ") ";
        cout << components[bb] << "/" << union_find.find(bb) << "(" << union_find.size(bb) << ")" << endl;
        for (const Board::Border& border : board.borders)
        {
            cout << "find ";
            cout << components[border.first] << "/" << union_find.find(border.first) << "(" << union_find.size(border.first)  << ") ";
            cout << components[border.second] << "/" << union_find.find(border.second) << "(" << union_find.size(border.second)  << ")" << endl;
        }
    }

    return 0;
}

