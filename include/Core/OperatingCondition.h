#pragma once

// Operating conditions for the ducted fan simulation.
// Later we can add constructors that set default values or read from Config.

struct OperatingCondition
{
    double rho;        // air density [kg/m^3]
    double mu;         // dynamic viscosity [Pa*s]
    double p_ambient;  // ambient pressure [Pa]
    double T_ambient;  // ambient temperature [K]
    double V_infty;    // freestream velocity [m/s]
    double Mach;       // Mach number (optional, approximate)

    OperatingCondition()
        : rho(1.225),
        mu(1.7894e-5),
        p_ambient(101325.0),
        T_ambient(288.15),
        V_infty(0.0),
        Mach(0.0)
    {
    }
};
