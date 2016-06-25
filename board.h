#pragma once

#include <lemon/smart_graph.h>
#include <lemon/adaptors.h>
#include <lemon/maps.h>
#include <lemon/bfs.h>

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

struct SelectorMap
{
    typedef Board::Graph::Node Key;
    typedef bool Value;

    typedef Board::Graph::NodeMap<int> InputMap;

    const InputMap& input_map;
    int selection;

    SelectorMap(const InputMap& input_map, const int& selection);

    Value operator[](const Key& key) const;
};

struct BoardState
{
    typedef Board::Graph::NodeMap<int> States;

    typedef lemon::FilterNodes<const Board::Graph, const SelectorMap> Subgraph;
    typedef lemon::Bfs<Subgraph> Bfs;
    typedef Board::Graph::NodeMap<Board::Graph::Node> PredMap;
    typedef std::vector<bool> Victories;

    const Board& board;
    States states;

    PredMap pred_map;
    Victories victories;

    BoardState(const Board& board);
    bool checkVictories();
};

