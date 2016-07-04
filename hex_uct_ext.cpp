#include <boost/python.hpp>
#include <iostream>
#include "board.h"

std::string
hello_world()
{
    return "hello world";
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

    def("hello_world", hello_world);
    class_<Move>("Move");
    /*
        .add_property("x", [](const Move& move){ return move.first; } );

    class_<Board>("Board");
    */
}

