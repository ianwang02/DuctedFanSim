#pragma once
#include "Fan/Blade.h"
#include "Fan/Duct.h"

struct DuctedFan
{
    Blade rotor;
    Duct duct;
    unsigned int bladeCount;
    double rpm;
};
