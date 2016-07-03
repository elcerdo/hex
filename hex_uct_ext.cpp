#include <boost/python.hpp>
#include <iostream>

std::string
hello_world()
{
    return "hello world";
}

BOOST_PYTHON_MODULE(libhex_uct_ext)
{
#if !defined(NDEBUG)
    std::cout << "!!!! DEBUG !!!!" << std::endl;
#endif
    using namespace boost::python;
    def("hello_world", hello_world);
}

