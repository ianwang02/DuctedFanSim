#pragma once
#include <vector>

struct FlowPoint
{
    double x; // position [m]
    double y;
    double z;

    double u; // velocity components [m/s]
    double v;
    double w;
};

struct FlowField
{
    std::vector<FlowPoint> points;
};
