#pragma once

#include <lemon/smart_graph.h>

struct Board
{
    typedef std::pair<int,int> Coord;

    typedef lemon::SmartGraph Graph;
    typedef Graph::NodeMap<Coord> Coords;
    typedef Graph::Node Node;
    typedef std::pair<Node,Node> Border;

    int size;
    Graph graph;
    Coords coords;

    Board(const int size=11);
    Board(const Board& board) = delete;
    Board& operator=(const Board& board) = delete;

    inline int get_number_of_players() const { return borders.size(); }
    inline const Border& get_border(int player) const { return borders[player]; }

    protected:
        typedef std::vector<Border> Borders;

        Borders borders;
};

typedef Board::Node Move;
typedef std::vector<Move> Moves;

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
    Moves getAvailableMoves() const;
    Victories checkVictories() const;
    bool playMove(const Move& move);
};

size_t hash_value(const BoardState& state);

