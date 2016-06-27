#pragma once

void
play_one_sequence(GraphData& graph_data, GraphData::Rng& re, const HashedPair<BoardState>& hashed_state_start)
{
    GraphData::Nodes nodes;

    // go down till hitting the bottom of the tree

    /*
#if defined(VERBOSEMC)
    std::cout << graph_data << std::endl;
    std::cout << "going down" << std::endl;
#endif
*/

    const GraphData::Node& start_node = graph_data.get_node(hashed_state_start);

    GraphData::Node current_node = start_node;

    nodes.push_back(current_node);

    {
        bool new_node_created = false;
        do
        {
            if (graph_data.is_leaf(current_node))
            {
                BoardState state = graph_data.get_state(current_node);

                const Move& move = graph_data.get_random_available_move(current_node, re);
                std::tie(new_node_created, current_node) = graph_data.get_or_create_child(state, move); // non-const !!!
            }
            else
            {
                current_node = graph_data.get_best_child(current_node, re);
            }

            nodes.push_back(current_node);
        } while (!new_node_created);
    }

    // get state from cache
    BoardState state = graph_data.get_state(current_node);

    /*
#if defined(VERBOSEMC)
    std::cout << graph_data << std::endl;
    std::cout << "down " << nodes.size() << " nodes to " << state << std::endl;
#endif

#if defined(TIMING)
    const double& time_end_descent = get_double_time();
    accum_descent += time_end_descent-time_start_descent;
#endif
*/

    // play random until end of game
    /*
#if defined(VERBOSEMC)
    std::cout << "mc til the end" << std::endl;
#endif

#if defined(TIMING)
    const double& time_start_mc = get_double_time();
#endif
*/

    int winner = state.getWinner();
    while ( winner < 0 )
    {
        const auto& possible_moves = state.getPossibleMoves();
        assert( possible_moves.size() != 0 );
        state.playMove(possible_moves[ re()%possible_moves.size() ]);
        winner = state.getWinner();
    }

   /*
#if defined(TIMING)
    const double& time_end_mc = get_double_time();
    accum_mc += time_end_mc-time_start_mc;
#endif

    // propagate mc value backward
#if defined(VERBOSEMC)
    std::cout << graph_data << std::endl;
    std::cout << "backpropagate mc result" << std::endl;
#endif

#if defined(TIMING)
    const double& time_start_update = get_double_time();
#endif
*/

    graph_data.back_propagate_winner(nodes, state);
/*
#if defined(TIMING)
    const double& time_end_update = get_double_time();
    accum_update += time_end_update-time_start_update;
#endif
*/
}

