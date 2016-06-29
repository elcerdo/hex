#include "board.h"

#include <boost/unordered_map.hpp>
#include <lemon/bfs.h>
#include <lemon/adaptors.h>
#include <lemon/maps.h>

Board::Board(const int size) : graph(), coords(graph)
{
    typedef boost::unordered_map<Coord,Node> Nodes;

    Nodes nodes;
    for (int ii=0; ii<size; ii++)
        for (int jj=0; jj<size; jj++)
        {
            const Coord coord = std::make_pair(ii,jj);
            const Node node = graph.addNode();
            coords[node] = coord;
            nodes[coord] = node;
        }

    for (int ii=0; ii<size; ii++)
        for (int jj=0; jj<size-1; jj++)
        {
            const Coord coord_aa(ii,jj);
            const Coord coord_bb(ii,jj+1);
            assert( nodes.find(coord_aa) != nodes.end() );
            assert( nodes.find(coord_bb) != nodes.end() );
            graph.addEdge(nodes.find(coord_aa)->second, nodes.find(coord_bb)->second);
        }

    for (int jj=0; jj<size; jj++)
        for (int ii=0; ii<size-1; ii++)
        {
            const Coord coord_aa(ii,jj);
            const Coord coord_bb(ii+1,jj);
            assert( nodes.find(coord_aa) != nodes.end() );
            assert( nodes.find(coord_bb) != nodes.end() );
            graph.addEdge(nodes.find(coord_aa)->second, nodes.find(coord_bb)->second);
        }

    for (int ii=1; ii<size; ii++)
        for (int jj=1; jj<size; jj++)
        {
            const Coord coord_aa = std::make_pair(ii,jj-1);
            const Coord coord_bb = std::make_pair(ii-1,jj);
            assert( nodes.find(coord_aa) != nodes.end() );
            assert( nodes.find(coord_bb) != nodes.end() );
            graph.addEdge(nodes.find(coord_aa)->second, nodes.find(coord_bb)->second);
        }

    const int playable_size = nodes.size();
    assert( playable_size == graph.maxNodeId()+1 );

    {
        const Border border = std::make_pair(graph.addNode(), graph.addNode());
        assert( graph.id(border.first) >= playable_size );
        assert( graph.id(border.second) >= playable_size );
        coords[border.first] = std::make_pair(size/2, -2);
        coords[border.second] = std::make_pair(size/2, size+1);
        borders.push_back(border);

        for (int kk=0; kk<size; kk++)
        {
            const Coord coord_first(kk, 0);
            const Coord coord_second(kk, size-1);
            graph.addEdge(border.first, nodes.find(coord_first)->second);
            graph.addEdge(border.second, nodes.find(coord_second)->second);
        }
    }

    {
        const Border border = std::make_pair(graph.addNode(), graph.addNode());
        assert( graph.id(border.first) >= playable_size );
        assert( graph.id(border.second) >= playable_size );
        coords[border.first] = std::make_pair(-2, size/2);
        coords[border.second] = std::make_pair(size+1, size/2);
        borders.push_back(border);

        for (int kk=0; kk<size; kk++)
        {
            const Coord coord_first(0, kk);
            const Coord coord_second(size-1, kk);
            graph.addEdge(border.first, nodes.find(coord_first)->second);
            graph.addEdge(border.second, nodes.find(coord_second)->second);
        }
    }
}

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

SameStateMap::SameStateMap(const Board::Graph& graph_, const SameStateMap::InputMap& input_map_) : graph(graph_), input_map(input_map_)
{
}

SameStateMap::Value
SameStateMap::operator[](const SameStateMap::Key& key) const
{
    return input_map[graph.u(key)] == input_map[graph.v(key)];
}

BoardState::BoardState(const Board& board_) : board(board_), states(board_.graph, board.getNumberOfPlayers()), count(0), victories()
{
    for (int player=0; player<board.getNumberOfPlayers(); player++)
    {
        const Board::Border& border = board.borders[player];
        states[border.first] = player;
        states[border.second] = player;
        victories.push_back(false);
    }
};

BoardState::BoardState(const BoardState& other) : board(other.board), states(other.board.graph), count(other.count), victories(other.victories)
{
    lemon::mapCopy(board.graph, other.states, states);
}

BoardState&
BoardState::operator=(const BoardState& other)
{
    assert( &board == &other.board );
    lemon::mapCopy(board.graph, other.states, states);
    count = other.count;
    victories = other.victories;
    return *this;
}

Moves
BoardState::getAvailableMoves() const
{
    if (getWinner() >= 0) return Moves();

    const int nplayers = board.getNumberOfPlayers();
    Moves moves;
    for (Board::Graph::NodeIt ni(board.graph); ni!=lemon::INVALID; ++ni)
        if (states[ni] == nplayers) moves.push_back(ni);
    return moves;
}

int
BoardState::getWinner() const
{
    for (int player=0; player<victories.size(); player++)
        if (victories[player]) return player;
    return -1;
}

BoardState::Victories
BoardState::checkVictories() const
{
    typedef lemon::FilterEdges<const Board::Graph, const SameStateMap> Subgraph;

    Victories victories;

    SameStateMap selector(board.graph, states);
    Subgraph subgraph(board.graph, selector);
    for (int player=0; player<board.getNumberOfPlayers(); player++)
    {
        const Board::Border& border = board.borders[player];
        const bool victory = lemon::bfs(subgraph).run(border.first, border.second);
        victories.push_back(victory);
    }

    return victories;
}

bool
BoardState::playMove(const Move& move)
{
    const int nplayers = board.getNumberOfPlayers();
    const int player = count%nplayers;
    if (!board.graph.valid(move)) return false;
    if (states[move] != nplayers) return false;
    states[move] = player;
    count++;
    victories = checkVictories();
    return true;
}

size_t
hash_value(const BoardState& state)
{
    size_t hash = reinterpret_cast<size_t>(&state.board);
    boost::hash_combine(hash, state.count);
    for (Board::Graph::NodeIt ni(state.board.graph); ni!=lemon::INVALID; ++ni)
        boost::hash_combine(hash, state.states[ni]);
    return hash;
}

