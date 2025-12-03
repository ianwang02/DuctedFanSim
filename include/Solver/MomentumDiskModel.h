#pragma once
#include "Core/OperatingCondition.h"
#include "Fan/DuctedFan.h"

class MomentumDiskModel
{
public:
    struct Results
    {
        double diskArea;     // m^2
        double Vi_hover;     // induced velocity in hover (m/s)
        double Vi_forward;   // induced velocity in forward flight
        double Vi;           // final induced velocity used
        double thrust;       // Newtons
        double power;        // Watts
        double Ct;           // thrust coefficient
        double Cp;           // power coefficient
        double eta;          // ideal efficiency
        double massFlow;     // mass flow rate (kg/s)
    };

    // Solve using known thrust (preferred)
    Results solveWithThrust(
        const DuctedFan& fan,
        const OperatingCondition& op,
        double thrust // user-specified thrust [N]
    );

    // Solve using RPM + basic rotor solidity estimate
    Results solveWithRPM(
        const DuctedFan& fan,
        const OperatingCondition& op
    );

private:
    double computeDiskArea(double radius) const;
    double estimateThrustFromRPM(
        const DuctedFan& fan,
        const OperatingCondition& op
    ) const;
};
