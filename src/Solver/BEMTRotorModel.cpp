#include "Solver/BEMTRotorModel.h"
#include "Math/Interpolation.h"
#include <cmath>
#include <stdexcept>
#include <iostream>
#include <string>

// ------------------------------------------------------------
// Helper: thin-airfoil fallback if no database data available
// ------------------------------------------------------------
static void approximateAirfoilCoeffs(double alphaRad, double& Cl, double& Cd)
{
    const double twoPi = 2.0 * MathConstants::PI;
    Cl = twoPi * alphaRad;

    const double Cd0 = 0.01;   // profile drag at Cl ~ 0
    const double k = 0.02;   // induced/shape drag factor
    Cd = Cd0 + k * Cl * Cl;
}

// ------------------------------------------------------------
// Helper: Prandtl tip-loss factor
// ------------------------------------------------------------
static double computeTipLoss(unsigned int B, double R, double r, double phi)
{
    double sinPhi = std::sin(phi);
    if (R <= r || std::abs(sinPhi) < 1e-6)
    {
        return 1.0;
    }

    double f = (B / 2.0) * (R - r) / (r * sinPhi);
    double expTerm = std::exp(-f);
    double F = (2.0 / MathConstants::PI) * std::acos(expTerm);

    if (F < 1e-3) F = 1e-3;
    return F;
}

// ------------------------------------------------------------
// Main BEM solver
// ------------------------------------------------------------
BEMTRotorModel::Results BEMTRotorModel::solve(
    const Blade& blade,
    unsigned int bladeCount,
    const OperatingCondition& op,
    const AirfoilDatabase& db,
    double rpm
)
{
    Results res{};
    res.thrust = 0.0;
    res.torque = 0.0;
    res.power = 0.0;

    const auto& sections = blade.sections;
    if (sections.size() < 2)
    {
        throw std::runtime_error("BEMTRotorModel: blade must have at least 2 sections.");
    }

    // Basic constants from operating condition and geometry
    const double R = sections.back().r;
    const double rho = op.rho;
    const double mu = op.mu;
    const unsigned int B = bladeCount;
    const double Vinfty = op.V_infty;

    res.R = R;
    res.omega = rpm * (2.0 * MathConstants::PI / 60.0);  // rad/s
    res.U_tip = res.omega * R;

    const std::size_t N = sections.size();
    res.elements.reserve(N);

    // Radial spacing dr for each section (simple finite difference)
    std::vector<double> dr(N, 0.0);
    for (std::size_t i = 0; i < N; ++i)
    {
        double r = sections[i].r;
        if (i == 0)
        {
            dr[i] = sections[i + 1].r - r;
        }
        else if (i == N - 1)
        {
            dr[i] = r - sections[i - 1].r;
        }
        else
        {
            dr[i] = 0.5 * (sections[i + 1].r - sections[i - 1].r);
        }
    }

    // Loop over radial stations
    for (std::size_t i = 0; i < N; ++i)
    {
        const BladeSection& sec = sections[i];
        const double r = sec.r;
        const double c = sec.chord;
        const double dr_i = dr[i];
        const double theta = sec.twistDeg * MathConstants::PI / 180.0;
        const std::string& airfoilName = sec.airfoilName;

        // Initial guesses for induction factors
        double a = 0.1;
        double aP = 0.0;

        const int maxIter = 100;
        const double tol = 1e-4;

        double phi = 0.0;
        double alpha = 0.0;
        double alphaDeg = 0.0;
        double Cl = 0.0, Cd = 0.0;

        for (int iter = 0; iter < maxIter; ++iter)
        {
            // Local velocities
            double Vaxial = Vinfty * (1.0 - a);
            double Vtangential = res.omega * r * (1.0 + aP);

            phi = std::atan2(Vaxial, Vtangential);   // inflow angle
            alpha = theta - phi;                     // angle of attack
            alphaDeg = alpha * 180.0 / MathConstants::PI;

            // Get Cl, Cd: try database first, then fallback
            try
            {
                double Vrel = std::sqrt(Vaxial * Vaxial + Vtangential * Vtangential);
                double Re = (mu > 0.0) ? (rho * Vrel * c / mu) : 0.0;

                Cl = db.getCl(airfoilName, alphaDeg, Re, op.Mach);
                Cd = db.getCd(airfoilName, alphaDeg, Re, op.Mach);
            }
            catch (const std::exception&)
            {
                approximateAirfoilCoeffs(alpha, Cl, Cd);
            }

            // Normal & tangential force coefficients
            double Cn = Cl * std::cos(phi) + Cd * std::sin(phi);
            double Ct = Cl * std::sin(phi) - Cd * std::cos(phi);

            // Local solidity
            double sigma = (B * c) / (2.0 * MathConstants::PI * r);

            // Tip-loss factor
            double F = computeTipLoss(B, R, r, phi);

            if (sigma * Cn < 1e-6)
                break;

            // Axial induction update
            double aNew = 1.0 / ((4.0 * F * std::sin(phi) * std::sin(phi)) / (sigma * Cn) + 1.0);

            // Tangential induction update
            double aPNew = aP;
            if (std::abs(Ct) > 1e-6)
            {
                aPNew = 1.0 / ((4.0 * F * std::sin(phi) * std::cos(phi)) / (sigma * Ct) - 1.0);
            }

            // Relaxation
            const double relax = 0.3;
            aNew = a + relax * (aNew - a);
            aPNew = aP + relax * (aPNew - aP);

            if (std::abs(aNew - a) < tol && std::abs(aPNew - aP) < tol)
            {
                a = aNew;
                aP = aPNew;
                break;
            }

            a = aNew;
            aP = aPNew;
        }

        // Final velocities & forces
        double Vaxial = Vinfty * (1.0 - a);
        double Vtangential = res.omega * r * (1.0 + aP);
        double Vrel = std::sqrt(Vaxial * Vaxial + Vtangential * Vtangential);
        double q = 0.5 * rho * Vrel * Vrel;

        double Cn = Cl * std::cos(phi) + Cd * std::sin(phi);
        double Ct = Cl * std::sin(phi) - Cd * std::cos(phi);

        double dL = q * c * Cl * dr_i;
        double dD = q * c * Cd * dr_i;

        double dT = B * (dL * std::cos(phi) + dD * std::sin(phi));
        double dQ = B * (dL * std::sin(phi) - dD * std::cos(phi)) * r;

        res.thrust += dT;
        res.torque += dQ;

        ElementResult er;
        er.r = r;
        er.dr = dr_i;
        er.a = a;
        er.aPrime = aP;
        er.phi = phi;
        er.alphaDeg = alphaDeg;
        er.Cl = Cl;
        er.Cd = Cd;
        er.dT = dT;
        er.dQ = dQ;

        res.elements.push_back(er);
    }

    // Global performance quantities
    res.power = res.torque * res.omega;

    if (R > 0.0)
    {
        double A = MathConstants::PI * R * R;
        if (rho > 0.0 && A > 0.0 && res.U_tip > 0.0)
        {
            res.Ct = res.thrust / (rho * A * res.U_tip * res.U_tip);
            res.Cp = res.power / (rho * A * res.U_tip * res.U_tip * res.U_tip);
        }
    }

    if (Vinfty > 0.0 && res.power > 0.0)
    {
        res.eta = res.thrust * Vinfty / res.power;
    }
    else
    {
        res.eta = 0.0;
    }

    return res;
}
