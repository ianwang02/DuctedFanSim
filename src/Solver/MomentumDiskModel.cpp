#include "Solver/MomentumDiskModel.h"
#include "Math/Constants.h"
#include <cmath>
#include <stdexcept>
#include <iostream>

// ---------------------------
// Helper: Disk Area
// ---------------------------
double MomentumDiskModel::computeDiskArea(double radius) const
{
    return MathConstants::PI * radius * radius;
}

// -----------------------------------------------------------
// Helper: Approximate thrust when only RPM is known
// -----------------------------------------------------------
double MomentumDiskModel::estimateThrustFromRPM(
    const DuctedFan& fan,
    const OperatingCondition& op
) const
{
    // Very rough estimate for initial testing:
    // T ≈ K * rho * (RPM)^2 * R^4
    // Choose K small to avoid unrealistic forces

    const double R = fan.rotor.sections.back().r;
    const double rpm = fan.rpm;
    const double rho = op.rho;

    double K = 1e-10;  // can be tuned later or refined with BEM
    return K * rho * rpm * rpm * std::pow(R, 4.0);
}

// -----------------------------------------------------------
// Solve using thrust (ideal momentum theory)
// -----------------------------------------------------------
MomentumDiskModel::Results MomentumDiskModel::solveWithThrust(
    const DuctedFan& fan,
    const OperatingCondition& op,
    double thrust
)
{
    Results res{};
    const double R = fan.rotor.sections.back().r;
    res.diskArea = computeDiskArea(R);

    // Hover induced velocity
    res.Vi_hover = std::sqrt(thrust / (2.0 * op.rho * res.diskArea));

    // Forward flight induced velocity
    double Vinfty = op.V_infty;

    res.Vi_forward = 0.0;
    if (Vinfty > 0.0)
    {
        res.Vi_forward =
            (-Vinfty + std::sqrt(Vinfty * Vinfty + 4.0 * res.Vi_hover * res.Vi_hover)) / 2.0;
    }

    // Select correct induced velocity (hover or forward flight)
    res.Vi = (Vinfty > 0.0) ? res.Vi_forward : res.Vi_hover;

    // Power
    res.power = thrust * (Vinfty + res.Vi);

    // Mass flow
    res.massFlow = op.rho * res.diskArea * (Vinfty + res.Vi);

    // Coefficients (using classical definitions)
    double omega = fan.rpm * (2.0 * MathConstants::PI / 60.0);
    double U_tip = omega * R;

    res.Ct = thrust / (op.rho * res.diskArea * U_tip * U_tip);
    res.Cp = res.power / (op.rho * res.diskArea * std::pow(U_tip, 3));

    // Ideal propulsive efficiency
    if (Vinfty + res.Vi != 0.0)
        res.eta = Vinfty / (Vinfty + res.Vi);
    else
        res.eta = 0.0;

    res.thrust = thrust;
    return res;
}

// -----------------------------------------------------------
// Solve using RPM only (estimate thrust, then compute)
// -----------------------------------------------------------
MomentumDiskModel::Results MomentumDiskModel::solveWithRPM(
    const DuctedFan& fan,
    const OperatingCondition& op
)
{
    double estThrust = estimateThrustFromRPM(fan, op);
    return solveWithThrust(fan, op, estThrust);
}
