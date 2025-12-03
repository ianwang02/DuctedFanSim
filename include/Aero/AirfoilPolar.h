#pragma once
#include <string>
#include <vector>

struct AirfoilPolar
{
    std::string airfoilName;       // e.g. "NACA2412"
    double Re;                     // Reynolds number for this polar
    double Mach;                   // Mach number (if applicable, else 0)

    std::vector<double> alphaDeg;  // angle of attack [deg]
    std::vector<double> Cl;        // lift coefficient
    std::vector<double> Cd;        // drag coefficient
    std::vector<double> Cm;        // moment coefficient (optional)

    AirfoilPolar()
        : Re(0.0), Mach(0.0)
    {
    }
};
