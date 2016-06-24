#pragma once

#include <lemon/smart_graph.h>

struct Board
{
    typedef std::pair<int,int> Coord;

    typedef lemon::SmartGraph Graph;
    typedef Graph::NodeMap<Coord> Coords;
    typedef Graph::Node Node;

    Graph graph;
    Coords coords;

    Board(const int size=11);
};

