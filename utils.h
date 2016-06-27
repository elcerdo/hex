#pragma once

#include <iostream>
#include <random>

typedef std::mt19937 RandomEngine;

double
get_double_time();

struct clock_it
{
    clock_it(const double& delta);

    double delta;
};

std::ostream&
operator<<(std::ostream& os, const clock_it& clock_it);

