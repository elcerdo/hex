#pragma once

#include <lemon/smart_graph.h>

struct Board
{
    typedef std::pair<int,int> Coord;

    typedef lemon::SmartGraph Graph;
    typedef Graph::NodeMap<Coord> Coords;
    typedef Graph::Node Node;
    typedef std::pair<Node,Node> Border;
    typedef std::vector<Border> Borders;

    int size;
    Graph graph;
    Coords coords;
    Borders borders;

    Board(const int size=11);
};

