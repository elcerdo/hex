#include <boost/python.hpp>
#include <iostream>
#include "board.h"

std::string
helloWorld()
{
    return "hello world";
}

int
countNodes(const Board::Graph& graph)
{
    return lemon::countNodes(graph);
}

int
countEdges(const Board::Graph& graph)
{
    return lemon::countEdges(graph);
}

static size_t import_count = 0;

BOOST_PYTHON_MODULE(libhex_uct_ext)
{
    using namespace std;
#if !defined(NDEBUG)
    cout << "!!!! DEBUG !!!!" << " ";
    cout << import_count << endl;
#endif

    import_count++;

    using namespace boost::python;

    def("helloWorld", helloWorld);
    def("countNodes", countNodes);
    def("countEdges", countEdges);

    class_<Move>("Move");

    class_<Board::Coord>("Coord", init<Board::Coord::first_type, Board::Coord::second_type>())
        .def_readonly("x", &Board::Coord::first)
        .def_readonly("y", &Board::Coord::second);

    class_<Board::Graph, boost::noncopyable>("Graph");

    class_<Board, boost::noncopyable>("Board", init<int>())
        .def("getNumberOfPlayers", &Board::getNumberOfPlayers)
        .def_readonly("graph", &Board::graph)
        .def(self_ns::str(self_ns::self));

    class_<BoardState>("BoardState", init<const Board&>())
        .def("getState", &BoardState::getState)
        .def("getNextPlayer", &BoardState::getNextPlayer)
        .def("getWinner", &BoardState::getWinner)
        .def(self_ns::str(self_ns::self));


    /*
        .add_property("x", [](const Move& move){ return move.first; } );

    class_<Board>("Board");
    */
}

