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

struct SameStateMap
{
    typedef Board::Graph::Edge Key;
    typedef bool Value;

    typedef Board::Graph::NodeMap<int> InputMap;

    const Board::Graph& graph;
    const InputMap& input_map;

    SameStateMap(const Board::Graph& graph, const InputMap& input_map);

    Value operator[](const Key& key) const;
};

typedef Board::Node Move;

struct BoardState
{
    typedef Board::Graph::NodeMap<int> States;
    typedef std::vector<bool> Victories;

    const Board& board;
    States states;
    int count;

    BoardState(const Board& board);
    BoardState(const BoardState& other);
    BoardState& operator=(const BoardState& other);

    int getWinner() const;
    std::vector<Move> getPossibleMoves() const;
    Victories checkVictories() const;
    bool playMove(const Move& move);
};

size_t hash_value(const BoardState& state);

