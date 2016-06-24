#include "board.h"

#include <boost/unordered_map.hpp>

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
            const Coord coord_aa = std::make_pair(ii,jj);
            const Coord coord_bb = std::make_pair(ii,jj+1);
            assert( nodes.find(coord_aa) != nodes.end() );
            assert( nodes.find(coord_bb) != nodes.end() );
            graph.addEdge(nodes.find(coord_aa)->second, nodes.find(coord_bb)->second);
        }

    for (int jj=0; jj<size; jj++)
        for (int ii=0; ii<size-1; ii++)
        {
            const Coord coord_aa = std::make_pair(ii,jj);
            const Coord coord_bb = std::make_pair(ii+1,jj);
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
}

