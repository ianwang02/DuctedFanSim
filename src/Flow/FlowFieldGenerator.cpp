#include "Flow/FlowFieldGenerator.h"
#include <cmath>

FlowField FlowFieldGenerator::generateAxisymmetricField(
    const BEMTRotorModel::Results& bem,
    const OperatingCondition& op,
    double xMin, double xMax, int Nx,
    double rMax, int Nr
)
{
    FlowField field;
    field.points.reserve(static_cast<std::size_t>(Nx * Nr * 4));

    const double R = bem.R;
    const double rho = op.rho;

    // Use global momentum theory to estimate induced velocity (hover-like)
    double A = MathConstants::PI * R * R;
    double Vi = 0.0;
    if (rho > 0.0 && A > 0.0)
    {
        Vi = std::sqrt(std::max(0.0, bem.thrust) / (2.0 * rho * A));
    }

    double Vinfty = op.V_infty;

    // Spatial steps
    double dx = (Nx > 1) ? (xMax - xMin) / (Nx - 1) : 0.0;
    double dr = (Nr > 1) ? (rMax) / (Nr - 1) : 0.0;

    for (int ix = 0; ix < Nx; ++ix)
    {
        double x = xMin + ix * dx;

        // Simple axial profile factor f(x): 0 upstream, 1 at disk, 2 downstream
        double f = 0.0;
        if (x < 0.0)
        {
            f = 0.0;
        }
        else if (x >= 0.0 && x <= 0.2 * R)
        {
            f = x / (0.2 * R); // ramp from 0 to 1
        }
        else
        {
            f = 1.0 + std::min((x - 0.2 * R) / (0.8 * R), 1.0); // approach 2
        }

        for (int ir = 0; ir < Nr; ++ir)
        {
            double r = ir * dr;

            // For each radius, place points around the circumference (4 points)
            for (int k = 0; k < 4; ++k)
            {
                double angle = (MathConstants::TWO_PI / 4.0) * k;
                double y = r * std::cos(angle);
                double z = r * std::sin(angle);

                FlowPoint p;
                p.x = x;
                p.y = y;
                p.z = z;

                // Axial velocity: freestream + induced
                double uInduced = Vi * f;
                p.u = Vinfty + uInduced;

                // No swirl yet (v, w = 0); we can add later
                p.v = 0.0;
                p.w = 0.0;

                field.points.push_back(p);
            }
        }
    }

    return field;
}
