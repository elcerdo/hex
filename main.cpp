/*
#include "hashed.h"
#include <boost/unordered_set.hpp>

struct A
{
    int x;
    float y;

    bool operator==(const A& other) const
    {
        return x == other.x && y == other.y;
    };
};

size_t
hash_value(const A& a)
{
    size_t hash = 0xaf058c2e;
    boost::hash_combine(hash, a.x);
    boost::hash_combine(hash, a.y);
    return hash;
}
*/

#include "viewer.h"
#include <iostream>
#include <QApplication>

using std::cout;
using std::endl;

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    Board board(11);
    cout << lemon::countNodes(board.graph) << "/" << lemon::countEdges(board.graph) << endl;

    Viewer viewer(board);
    viewer.resize(800, 600);
    viewer.show();

    return app.exec();
}

