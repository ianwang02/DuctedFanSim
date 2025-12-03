#pragma once
#include "Flow/FlowField.h"
#include "Solver/BEMTRotorModel.h"
#include "Core/OperatingCondition.h"
#include "Math/Constants.h"

class FlowFieldGenerator
{
public:
    // Simple axisymmetric field using BEM thrust + momentum theory
    static FlowField generateAxisymmetricField(
        const BEMTRotorModel::Results& bem,
        const OperatingCondition& op,
        double xMin, double xMax, int Nx,
        double rMax, int Nr
    );
};
