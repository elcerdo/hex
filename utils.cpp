#include "utils.h"

#include <cmath>

double
get_double_time()
{
#if defined(OPENMP_FOUND)
    return omp_get_wtime();
#else
    timespec foo;
    clock_gettime(CLOCK_REALTIME, &foo);

    return foo.tv_sec + foo.tv_nsec*1e-9;
#endif
}

clock_it::clock_it(const double& delta) : delta(delta)
{
}

std::ostream&
operator<<(std::ostream& os, const clock_it& clock_it)
{
    const std::streamsize old_precision = os.precision();
    const std::ostream::fmtflags old_flags = os.flags();
    os.precision(0);
    os << std::fixed;

    if (clock_it.delta < 1e-6)
    {
        const double ns(clock_it.delta*1e9);
        os << ns << "ns";
        os.precision(old_precision);
        os.flags(old_flags);
        return os;
    }

    if (clock_it.delta < 1e-3)
    {
        const double us(clock_it.delta*1e6);
        os << us << "us";
        os.precision(old_precision);
        os.flags(old_flags);
        return os;
    }

    const double ms(clock_it.delta*1e3);
    os << ms << "ms";
    os.precision(old_precision);
    os.flags(old_flags);
    return os;
}

