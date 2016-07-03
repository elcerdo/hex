#pragma once

#include <lemon/smart_graph.h>
#include "unionfind.h"

struct Board
{
    typedef lemon::SmartGraph Graph;
    typedef Graph::Node Node;

    typedef std::pair<int,int> Coord;
    typedef std::pair<Node,Node> Border;

    typedef Graph::NodeMap<Coord> Coords;
    typedef std::vector<Border> Borders;

    Graph graph;
    Coords coords;
    Borders borders;

    Board(const int size=11);
    Board(const Board& board) = delete;
    Board& operator=(const Board& board) = delete;

    inline int getNumberOfPlayers() const { return borders.size(); }

    friend size_t hash_value(const Board& board);
};

typedef Board::Node Move;
typedef std::vector<Move> Moves;

struct BoardState
{
    const Board& board;

    BoardState(const Board& board);
    BoardState(const BoardState& other);
    BoardState& operator=(const BoardState& other);

    inline int getState(const Board::Node& node) const { return states[node]; }
    inline int getNextPlayer() const { return count%board.getNumberOfPlayers(); }
    inline int getWinner() const { return winner; }
    Moves getAvailableMoves() const;
    bool playMove(const Move& move);

    protected:
        typedef Board::Graph::NodeMap<int> States;
        typedef Board::Graph::NodeMap<int> Components;
        typedef lemon::UnionFind<Components> UnionFind;

        States states;
        int count;
        int winner;

        Components components;
        UnionFind union_find;

        int checkWinnerBfs() const;

    friend size_t hash_value(const BoardState& state);
};


