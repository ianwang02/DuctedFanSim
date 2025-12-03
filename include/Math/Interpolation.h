#pragma once
#include <vector>

namespace MathUtils
{
    // Simple 1D linear interpolation: y(x) given arrays xTable, yTable
    double linearInterpolate(
        const std::vector<double>& xTable,
        const std::vector<double>& yTable,
        double x
    );

    // (Later) 2D interpolation, etc.
}
