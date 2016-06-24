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

#include <iostream>

using std::cout;
using std::endl;

int main(int argc, char* argv[])
{
    typedef boost::unordered_set<A> Prout;
    Prout prout;
    A xx = {1,1.2};
    A yy = {2,3.};
    cout << (xx== yy) << endl;
    prout.insert(xx);
    cout << prout.size() << endl;
    return 0;
}

