#pragma once

#include <iostream>

double
get_double_time();

struct clock_it
{
    clock_it(const double& delta);

    double delta;
};

std::ostream&
operator<<(std::ostream& os, const clock_it& clock_it);

