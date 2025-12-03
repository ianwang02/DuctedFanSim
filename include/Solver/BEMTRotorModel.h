#pragma once
#include <vector>
#include "Fan/Blade.h"
#include "Core/OperatingCondition.h"
#include "Aero/AirfoilDatabase.h"
#include "Math/Constants.h"

class BEMTRotorModel
{
public:
    struct ElementResult
    {
        double r;          // radial position [m]
        double dr;         // radial width of this element [m]
        double a;          // axial induction factor
        double aPrime;     // tangential induction factor
        double phi;        // inflow angle [rad]
        double alphaDeg;   // angle of attack [deg]
        double Cl;         // lift coefficient
        double Cd;         // drag coefficient
        double dT;         // thrust contribution [N]
        double dQ;         // torque contribution [N*m]
    };

    struct Results
    {
        double thrust;     // total thrust [N]
        double torque;     // total torque [N*m]
        double power;      // total power [W]
        double Ct;         // thrust coefficient
        double Cp;         // power coefficient
        double eta;        // propulsive efficiency (using Vinfty)
        double R;          // tip radius [m]
        double omega;      // angular speed [rad/s]
        double U_tip;      // tip speed [m/s]

        std::vector<ElementResult> elements;
    };

    // Note: rpm is now an explicit argument
    Results solve(
        const Blade& blade,
        unsigned int bladeCount,
        const OperatingCondition& op,
        const AirfoilDatabase& db,
        double rpm
    );
};
