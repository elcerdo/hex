#include "loop_uct.h"

#include <cassert>
#include "utils.h"

void
play_one_sequence(GraphData& graph_data, RandomEngine& re, const HashedPair<BoardState>& hashed_state_start)
{
    GraphData::Nodes nodes;

    // go down till hitting the bottom of the tree
    GraphData::Node current_node = graph_data.get_node(hashed_state_start);
    nodes.push_back(current_node);
    {
        bool new_node_created = false;
        do
        {
            if (graph_data.available_moves(current_node) > 0)
            {
                BoardState state = graph_data.get_state(current_node);

                const Move& move = graph_data.pop_random_available_move(current_node);
                std::tie(new_node_created, current_node) = graph_data.get_or_create_child(state, move, re); // non-const !!!
            }
            else
            {
                const GraphData::Node next_node = graph_data.get_best_child(current_node, re);
                if (!graph_data.valid(next_node)) break;
                current_node = next_node;
            }
            nodes.push_back(current_node);
        } while (!new_node_created);
    }

    // get state from cache
    BoardState state = graph_data.get_state(current_node);

    // play random until end of game
    int winner = state.getWinner();

    /*
    // generic loop
    while ( winner < 0 )
    {
        const Moves& possible_moves = state.getAvailableMoves();
        assert( possible_moves.size() != 0 );
        const bool valid = state.playMove(possible_moves[ re()%possible_moves.size() ]);
        assert( valid );
        winner = state.getWinner();
    }
    */

    // hex specific
    Moves available_moves = state.getAvailableMoves();
    std::random_shuffle(available_moves.begin(), available_moves.end(), [&re](const size_t size){ return re()%size; });
    while ( winner < 0 && !available_moves.empty() )
    {
        const Move selected_move = available_moves.back();
        const bool valid = state.playMove(selected_move);
        assert( valid );
        winner = state.getWinner();
        available_moves.pop_back();
    }

    assert( state.getWinner() >= 0 );

    // propagate mc value backward
    graph_data.back_propagate_winner(nodes, state);
}

void
crunch_it_baby(GraphData& graph_data, RandomEngine& re, const HashedPair<BoardState>& hashed_state, const double duration_max, const bool prune)
{
    const double start_time = get_double_time();

    if (!graph_data.contains(hashed_state))
    {
        std::cout << "WARNING!!! unknown game state" << std::endl;
        graph_data.get_or_create_node(hashed_state, re);
    }

    if (prune) {
        const double prunning_start_time = get_double_time();

        const GraphData::Node& root = graph_data.get_node(hashed_state);
        graph_data.prune_below(root);

        const double prunning_end_time = get_double_time();
        std::cout << "prunning took " << clock_it(prunning_end_time-prunning_start_time) << std::endl;
    }

    int mc_count = 0;
    const double mc_start_time = get_double_time();
    double current_time = mc_start_time;
    while (current_time - start_time < duration_max)
    {
        play_one_sequence(graph_data, re, hashed_state);
        mc_count++;
        current_time = get_double_time();
    }

    double game_per_second = 0;
    if (current_time != mc_start_time) game_per_second = mc_count/(current_time-mc_start_time);
    std::cout << "simulated " << mc_count << " games in " << clock_it(current_time-mc_start_time) << " (" << game_per_second << "games/s)" << std::endl;
}
