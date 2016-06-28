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
    while ( winner < 0 )
    {
        const Moves& possible_moves = state.getAvailableMoves();
        assert( possible_moves.size() != 0 );
        const bool valid = state.playMove(possible_moves[ re()%possible_moves.size() ]);
        assert( valid );
        winner = state.getWinner();
    }
    assert( state.getWinner() >= 0 );

    // propagate mc value backward
    graph_data.back_propagate_winner(nodes, state);
}

void
crunch_it_baby(GraphData& graph_data, RandomEngine& re, const HashedPair<BoardState>& hashed_state, const double duration_max)
{
    const double start_time = get_double_time();
    double current_time = start_time;

    /*
#if defined(REPORTING)
    Report report;

    const double duration_start_time = current_time;
    report.timestamp = current_time;
    report.turn = game.turn;

    graph_data.get_sizes(report.pre_prunning_nodes, report.pre_prunning_arcs, report.pre_prunning_parallel_arcs);
    std::cout << "n=" << report.pre_prunning_nodes << " ";
    std::cout << "e=" << report.pre_prunning_arcs << " ";
    std::cout << "pe=" << report.pre_prunning_parallel_arcs << std::endl;
#endif
*/

    const double prunning_start_time = get_double_time();
    {
        if (!graph_data.contains(hashed_state))
        {
            std::cout << "WARNING!!! unknown game state" << std::endl;
            graph_data.get_or_create_node(hashed_state, re);
        }

        const GraphData::Node& root = graph_data.get_node(hashed_state);
        graph_data.prune_below(root);

    }
    const double prunning_end_time = get_double_time();
    std::cout << "prunning took " << clock_it(prunning_end_time-prunning_start_time) << std::endl;

    /*
#if defined(REPORTING)
    graph_data.get_sizes(report.post_prunning_nodes, report.post_prunning_arcs, report.post_prunning_parallel_arcs);
    std::cout << "n=" << report.post_prunning_nodes << " ";
    std::cout << "e=" << report.post_prunning_arcs << " ";
    std::cout << "pe=" << report.post_prunning_parallel_arcs << std::endl;

    report.prunning_duration = prunning_end_time-prunning_start_time;
#endif
*/

    int mc_count = 0;
    current_time = get_double_time();
    const double mc_start_time = current_time;

    /*
#if defined(TIMING)
    double accum_descent = 0;
    double accum_mc = 0;
    double accum_update = 0;
#endif
*/

    std::cout << "monte carlo" <<std::endl;
    while (current_time - start_time < duration_max)
    {
        /*
#if defined(VERBOSEMC)
        std::cout << "/////////////////////////// simulating game " << mc_count << std::endl;
#endif
*/

        play_one_sequence(graph_data, re, hashed_state);


        mc_count++;
        current_time = get_double_time();
    }


    /*
#if defined(TIMING)
    const double& total_accum = accum_descent+accum_mc+accum_update;
    if (total_accum>0)
    {
        std::cout << "descent " << (100*accum_descent/total_accum) << "% ";
        std::cout << "mc " << (100*accum_mc/total_accum) << "% ";
        std::cout << "update " << (100*accum_update/total_accum) << "%" << std::endl;
    }
#endif
*/

    double game_per_second = 0;
    if (current_time != mc_start_time) game_per_second = mc_count/(current_time-mc_start_time);
    std::cout << "simulated " << mc_count << " games in " << clock_it(current_time-mc_start_time) << " (" << game_per_second << "games/s)" << std::endl;

    /*
#if defined(REPORTING)
    graph_data.get_sizes(report.post_mc_nodes, report.post_mc_arcs, report.post_mc_parallel_arcs);
    std::cout << "n=" << report.post_mc_nodes << " ";
    std::cout << "e=" << report.post_mc_arcs << " ";
    std::cout << "pe=" << report.post_mc_parallel_arcs << std::endl;

    report.duration = current_time-duration_start_time;

    return report;
#endif
*/

}
