#pragma once

#include <lemon/smart_graph.h>

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
};

typedef Board::Node Move;
typedef std::vector<Move> Moves;

struct BoardState
{
    typedef Board::Graph::NodeMap<int> States;
    typedef std::vector<bool> Victories;

    const Board& board;
    States states;

    BoardState(const Board& board);
    BoardState(const BoardState& other);
    BoardState& operator=(const BoardState& other);

    inline int getNextPlayer() const { return count%board.getNumberOfPlayers(); }
    int getWinner() const;
    Moves getAvailableMoves() const;
    Victories checkVictories() const;
    bool playMove(const Move& move);

    protected:
        int count;

        friend size_t hash_value(const BoardState& state);
};


