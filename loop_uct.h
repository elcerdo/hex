#pragma once

#include "graph_uct.h"
#include "utils.h"
#include "hashed.h"

void
crunch_it_baby(GraphData& graph_data, RandomEngine& re, const HashedPair<BoardState>& hashed_state, const double duration_max, const bool prune, std::ostream& os);

void
play_one_sequence(GraphData& graph_data, RandomEngine& re, const HashedPair<BoardState>& hashed_state_start);

