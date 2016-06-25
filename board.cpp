#include "board.h"

#include <boost/unordered_map.hpp>

Board::Board(const int size) : size(size), graph(), coords(graph)
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

    {
        const Border border = std::make_pair(graph.addNode(), graph.addNode());
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

SelectorMap::SelectorMap(const SelectorMap::InputMap& input_map, const int& selection) : input_map(input_map), selection(selection)
{
}

SelectorMap::Value
SelectorMap::operator[](const SelectorMap::Key& key) const
{
    return input_map[key] == selection;
}

#include <iostream>
using namespace std;

BoardState::BoardState(const Board& board_) : board(board_), states(board_.graph, board.borders.size()), pred_map(board_.graph, lemon::INVALID)
{
    for (int player=0; player<board.borders.size(); player++)
    {
        states[board.borders[player].first] = player;
        states[board.borders[player].second]= player;
    }
};

bool
BoardState::checkVictories()
{
    bool any_victory = false;

    victories.clear();
    for (int player=0; player<board.borders.size(); player++)
    {
        SelectorMap selector(states, player);
        Subgraph subgraph(board.graph, selector);

        const bool victory = Bfs(subgraph)
            .run(board.borders[player].first, board.borders[player].second);

        any_victory |= victory;
        victories.push_back(victory);
    }


    return any_victory;
}
