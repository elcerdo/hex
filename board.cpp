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

std::ostream&
operator<<(std::ostream& os, const Board::Coord& coord)
{
    return os << coord.first << "x" << coord.second;
}

std::ostream&
operator<<(std::ostream& os, const Board& board)
{
    os << "<Board ";
    os << "h=" << std::hex << hash_value(board) << std::dec << " ";
    os << "p=" << board.getNumberOfPlayers() << " ";
    os << "v=" << lemon::countNodes(board.graph) << " ";
    os << "e=" << lemon::countEdges(board.graph);
    os << ">";
    return os;
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

BoardState::BoardState(const Board& board_) : board(board_), board_hash(hash_value(board_)), states(board_.graph, board_.getNumberOfPlayers()), count(0), winner(-1), components(board_.graph, 0), union_find(components)
{
    for (int player=0; player<board.getNumberOfPlayers(); player++)
    {
        const Board::Border& border = board.borders[player];
        states[border.first] = player;
        states[border.second] = player;
        union_find.insert(border.first);
        union_find.insert(border.second);
    }
};

BoardState::BoardState(const BoardState& other) : board(other.board), board_hash(other.board_hash), states(other.board.graph), count(other.count), winner(other.winner), components(other.board.graph), union_find(components, other.union_find.items)
{
    lemon::mapCopy(board.graph, other.states, states);
    lemon::mapCopy(board.graph, other.components, components);
}

BoardState&
BoardState::operator=(const BoardState& other)
{
    assert( board_hash == other.board_hash );
    lemon::mapCopy(board.graph, other.states, states);
    lemon::mapCopy(board.graph, other.components, components);
    count = other.count;
    winner = other.winner;
    union_find.items = other.union_find.items;
    return *this;
}

Moves
BoardState::getAvailableMoves() const
{
    if (winner >= 0) return Moves();

    const int nplayers = board.getNumberOfPlayers();
    Moves moves;
    for (Board::Graph::NodeIt ni(board.graph); ni!=lemon::INVALID; ++ni)
        if (states[ni] == nplayers) moves.push_back(ni);
    return moves;
}

int
BoardState::checkWinnerBfs() const
{
    typedef lemon::FilterEdges<const Board::Graph, const SameStateMap> Subgraph;

    SameStateMap selector(board.graph, states);
    Subgraph subgraph(board.graph, selector);
    for (int player=0; player<board.getNumberOfPlayers(); player++)
    {
        const Board::Border& border = board.borders[player];
        const bool victory = lemon::bfs(subgraph).run(border.first, border.second);
        if (victory) return player;
    }

    return -1;
}

bool
BoardState::playMove(const Move& move)
{
    const int nplayers = board.getNumberOfPlayers();
    const int player = count%nplayers;
    if (!board.graph.valid(move)) return false;
    if (states[move] != nplayers) return false;
    states[move] = player;

    typedef Board::Graph::IncEdgeIt EdgeIt;
    union_find.insert(move);
    for (EdgeIt ei(board.graph, move); ei!=lemon::INVALID; ++ei)
    {
        const Board::Node node_uu = board.graph.u(ei);
        const Board::Node node_vv = board.graph.v(ei);
        assert( node_uu == move || node_vv == move );
        const Board::Node& node = node_uu == move ? node_vv : node_uu;
        if (states[node] != player) continue;
        union_find.join(move, node);
    }
    {
        const Board::Border& border = board.borders[player];
        if (union_find.find(border.first) == union_find.find(border.second))
            winner = player;
    }

    assert( winner == checkWinnerBfs() );
    count++;
    return true;
}

size_t
hash_value(const Board& board)
{
    size_t hash = 1234567890;
    boost::hash_combine(hash, lemon::countNodes(board.graph));
    for (Board::Graph::NodeIt ni(board.graph); ni!=lemon::INVALID; ++ni)
    {
        const Board::Coord& coord = board.coords[ni];
        boost::hash_combine(hash, coord.first);
        boost::hash_combine(hash, coord.second);
    }

    boost::hash_combine(hash, lemon::countEdges(board.graph));
    for (Board::Graph::EdgeIt ei(board.graph); ei!=lemon::INVALID; ++ei)
    {
        boost::hash_combine(hash, board.graph.id(board.graph.u(ei)));
        boost::hash_combine(hash, board.graph.id(board.graph.v(ei)));
    }

    boost::hash_combine(hash, board.borders.size());
    for (const Board::Border& border : board.borders)
    {
        boost::hash_combine(hash, board.graph.id(border.first));
        boost::hash_combine(hash, board.graph.id(border.second));
    }

    return hash;
}

size_t
hash_value(const BoardState& state)
{
    size_t hash = state.board_hash;
    boost::hash_combine(hash, state.count);
    for (Board::Graph::NodeIt ni(state.board.graph); ni!=lemon::INVALID; ++ni)
        boost::hash_combine(hash, state.states[ni]);
    return hash;
}

std::ostream&
operator<<(std::ostream& os, const BoardState& state)
{
    os << "<BoardState ";
    os << "b=" << std::hex << state.board_hash << std::dec << " ";
    os << "h=" << std::hex << hash_value(state) << std::dec << " ";
    os << "w=" << state.getWinner() << " ";

    const Moves& moves = state.getAvailableMoves();
    if (moves.size() < 10)
    {
        bool first = true;
        os << "a=[";
        for (const Move& move : moves)
        {
            if (!first) os << ",";
            os << state.board.coords[move];
            first = false;
        }
        os << "]";
    }
    else os << "a=" << moves.size();

    os << ">";
    return os;
}
