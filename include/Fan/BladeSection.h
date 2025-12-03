#pragma once
#include <string> 

struct BladeSection
{
    double r;        // radial position
    double chord;    // chord length
    double twistDeg; // twist angle [deg]
    std::string airfoilName;
};
