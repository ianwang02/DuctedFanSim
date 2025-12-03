#include "Math/Interpolation.h"
#include <stdexcept>

namespace MathUtils
{
    double linearInterpolate(
        const std::vector<double>& xTable,
        const std::vector<double>& yTable,
        double x
    )
    {
        if (xTable.size() != yTable.size() || xTable.empty())
        {
            throw std::runtime_error("Interpolation tables are invalid.");
        }

        // If x is outside the table, clamp to ends for now
        if (x <= xTable.front())
        {
            return yTable.front();
        }
        if (x >= xTable.back())
        {
            return yTable.back();
        }

        // Find interval [x_i, x_{i+1}] containing x
        for (std::size_t i = 0; i < xTable.size() - 1; ++i)
        {
            if (x >= xTable[i] && x <= xTable[i + 1])
            {
                double x0 = xTable[i];
                double x1 = xTable[i + 1];
                double y0 = yTable[i];
                double y1 = yTable[i + 1];

                double t = (x - x0) / (x1 - x0);
                return y0 + t * (y1 - y0);
            }
        }

        // Should never reach here, but just in case:
        return yTable.back();
    }
}
