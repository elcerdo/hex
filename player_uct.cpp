#include "player_uct.h"

#include <cassert>
#include "utils.h"

PlayerUct::PlayerUct(const Board& board, const double uct_constant, const size_t seed) : Player(board, "uct"), re(seed), graph_data(uct_constant), hashed_state(BoardState(board))
{
}

void
PlayerUct::update(const BoardState& state)
{
    hashed_state = make_hashed_pair(state);
}

#include "loop_uct.h"

void
crunch_it_baby(GraphData& graph_data, GraphData::Rng& re, const HashedPair<BoardState>& hashed_state, const double duration_max)
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
            graph_data.get_or_create_node(hashed_state);
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

