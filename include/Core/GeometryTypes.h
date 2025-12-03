#pragma once
#include "Math/Vector3.h"

// Basic geometric building blocks

struct Point3D
{
    double x;
    double y;
    double z;

    Point3D() : x(0.0), y(0.0), z(0.0) {}
    Point3D(double x_, double y_, double z_) : x(x_), y(y_), z(z_) {}
};

struct Triangle
{
    Point3D v0;
    Point3D v1;
    Point3D v2;
};