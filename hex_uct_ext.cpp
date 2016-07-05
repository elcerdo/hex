#include <boost/python.hpp>
#include <iostream>
#include "board.h"
#include "game.h"
#include "player_uct.h"
#include "player_random.h"

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

struct GamePython : public Status
{
    boost::python::object state_callback;
    boost::python::object message_callback;

    GamePython(boost::python::object& status);

    void updateState(const BoardState& state) override;
    void updateMessage(const std::string& message) override;

    int run(const Board& board);
};

GamePython::GamePython(boost::python::object& status) : state_callback(), message_callback()
{
    state_callback = status.attr("updateState");
    message_callback = status.attr("updateMessage");
}

void
GamePython::updateState(const BoardState& state)
{
    state_callback(state);
}

void
GamePython::updateMessage(const std::string& message)
{
    message_callback(message);
}

int
GamePython::run(const Board& board)
{
    Player* player0 = new PlayerRandom(board, 1234);
    Player* player1 = new PlayerUct(board, 1, 3, 4, 567890);

    BoardState state(board);
    const int winner = run_game_loop(board, state, player0, player1, this);

    delete player0;
    delete player1;

    return winner;
}

#if !defined(NDEBUG)
static size_t import_count = 0;
#endif

BOOST_PYTHON_MODULE(libhex_uct_ext)
{
    using namespace std;
#if !defined(NDEBUG)
    cout << "!!!! DEBUG !!!!" << " ";
    cout << import_count << endl;
    import_count++;
#endif


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

    class_<GamePython>("Game", init<boost::python::object&>())
        .def("run", &GamePython::run);

    /*
        .add_property("x", [](const Move& move){ return move.first; } );

    class_<Board>("Board");
    */
}

