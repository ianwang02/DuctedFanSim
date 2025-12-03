#include "Core/Config.h"
#include <iostream>

Config::Config()
    : airfoilDataDir("data/airfoils"),
    nasaDataDir("data/nasa"),
    ductSTLPath(""),
    rotorSTLPath(""),
    flowFieldOutputPath("output/flowfield.csv"),
    performanceOutputPath("output/performance.txt"),
    rpm(5000.0),
    bladeCount(3)
{
    // OperatingCondition already has sensible defaults
    opCond.V_infty = 0.0;
}

bool Config::loadFromFile(const std::string& /*filePath*/)
{
    // TODO: Implement file reading (e.g., JSON, INI, or simple key=value text).
    // For now, we just return false to indicate "not implemented".
    return false;
}

void Config::printSummary() const
{
    std::cout << "=== Simulation Configuration ===\n";
    std::cout << "Airfoil data directory: " << airfoilDataDir << "\n";
    std::cout << "NASA data directory   : " << nasaDataDir << "\n";
    std::cout << "Duct STL path         : " << ductSTLPath << "\n";
    std::cout << "Rotor STL path        : " << rotorSTLPath << "\n";
    std::cout << "Output flow field     : " << flowFieldOutputPath << "\n";
    std::cout << "Output performance    : " << performanceOutputPath << "\n";
    std::cout << "RPM                   : " << rpm << "\n";
    std::cout << "Blade count           : " << bladeCount << "\n";
    std::cout << "Operating condition:\n";
    std::cout << "  rho        = " << opCond.rho << " kg/m^3\n";
    std::cout << "  mu         = " << opCond.mu << " Pa*s\n";
    std::cout << "  V_infty    = " << opCond.V_infty << " m/s\n";
    std::cout << "  p_ambient  = " << opCond.p_ambient << " Pa\n";
    std::cout << "  T_ambient  = " << opCond.T_ambient << " K\n";
    std::cout << "  Mach       = " << opCond.Mach << "\n";
    std::cout << "================================\n";
}
