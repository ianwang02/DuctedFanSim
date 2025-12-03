#pragma once
#include <string>

// Simple placeholder for duct geometry
struct Duct
{
    std::string nacaCode;  // identified NACA airfoil code (later)
    double length;         // duct length
    double innerRadius;    // inner radius at mid-span
    double outerRadius;    // outer radius at mid-span
};
