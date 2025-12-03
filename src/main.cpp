#include <iostream>
#include <filesystem>   // for current_path + creating output dirs

#include "Core/Config.h"
#include "Fan/DuctedFan.h"
#include "Solver/MomentumDiskModel.h"
#include "Solver/BEMTRotorModel.h"
#include "Aero/AirfoilDatabase.h"
#include "Flow/FlowFieldGenerator.h"
#include "IO/Exporter.h"

int main()
{
    // Show working directory so we know where relative paths point
    std::cout << "Working directory: "
        << std::filesystem::current_path().string() << "\n\n";

    std::cout << "Ducted Fan Simulation - BEM + Momentum Test\n";

    Config cfg;
    cfg.printSummary();

    // -----------------------------
    // Build a simple test ducted fan
    // -----------------------------
    DuctedFan fan;
    fan.rpm = cfg.rpm;
    fan.bladeCount = cfg.bladeCount;

    // Simple 5-station blade, r in meters
    fan.rotor.sections.push_back({ 0.2, 0.08, 25.0, "NACA2412" });
    fan.rotor.sections.push_back({ 0.4, 0.06, 18.0, "NACA2412" });
    fan.rotor.sections.push_back({ 0.6, 0.05, 12.0, "NACA2412" });
    fan.rotor.sections.push_back({ 0.8, 0.04,  8.0, "NACA2412" });
    fan.rotor.sections.push_back({ 1.0, 0.03,  5.0, "NACA2412" }); // tip at r = 1 m

    // Duct placeholder
    fan.duct.innerRadius = 1.0;
    fan.duct.outerRadius = 1.05;
    fan.duct.length = 0.5;
    fan.duct.nacaCode = "NACA0015";

    // Operating condition: hover (no freestream)
    cfg.opCond.V_infty = 0.0;

    // -----------------------------
    // Momentum disk model (sanity check)
    // -----------------------------
    MomentumDiskModel momModel;
    double testThrust = 100.0; // N
    auto momResults = momModel.solveWithThrust(fan, cfg.opCond, testThrust);

    std::cout << "\n=== Momentum Disk Model ===\n";
    std::cout << "Thrust (input): " << momResults.thrust << " N\n";
    std::cout << "Power (ideal):  " << momResults.power << " W\n";
    std::cout << "Vi (hover):     " << momResults.Vi_hover << " m/s\n";

    // -----------------------------
    // BEM rotor model
    // -----------------------------
    AirfoilDatabase airfoils;
    airfoils.loadFromDirectory(cfg.airfoilDataDir);


    BEMTRotorModel bem;
    auto bemResults = bem.solve(
        fan.rotor,
        fan.bladeCount,
        cfg.opCond,
        airfoils,
        fan.rpm
    );

    std::cout << "\n=== BEM Rotor Model ===\n";
    std::cout << "Thrust: " << bemResults.thrust << " N\n";
    std::cout << "Torque: " << bemResults.torque << " N*m\n";
    std::cout << "Power:  " << bemResults.power << " W\n";
    std::cout << "Ct:     " << bemResults.Ct << "\n";
    std::cout << "Cp:     " << bemResults.Cp << "\n";
    std::cout << "Eta:    " << bemResults.eta << "\n";
    std::cout << "R:      " << bemResults.R << " m\n";

    std::cout << "\nElement breakdown (r, a, a', alpha, dT, dQ):\n";
    for (const auto& e : bemResults.elements)
    {
        std::cout << "r=" << e.r
            << " a=" << e.a
            << " a'=" << e.aPrime
            << " alpha=" << e.alphaDeg
            << " dT=" << e.dT
            << " dQ=" << e.dQ
            << "\n";
    }

    // -----------------------------
    // Generate a simple flow field
    // -----------------------------
    // Ensure output directory exists (based on Config path)
    std::string flowFile = cfg.flowFieldOutputPath;   // e.g. "output/flowfield.csv"
    std::filesystem::path flowPath(flowFile);
    std::filesystem::path parentDir = flowPath.parent_path();

    if (!parentDir.empty())
    {
        std::error_code ec;
        std::filesystem::create_directories(parentDir, ec);
        if (ec)
        {
            std::cout << "\nWarning: could not create output directory '"
                << parentDir.string() << "': " << ec.message() << "\n";
        }
    }

    double rMax = bemResults.R * 1.5; // extend beyond tip a bit
    auto flow = FlowFieldGenerator::generateAxisymmetricField(
        bemResults,
        cfg.opCond,
        -1.0 * bemResults.R,  // xMin
        2.0 * bemResults.R,  // xMax
        40,                   // Nx
        rMax,
        20                    // Nr
    );

    if (IO::FlowFieldCSVExporter::writeCSV(flowFile, flow))
    {
        std::cout << "\nFlow field written to " << flowFile << "\n";
    }
    else
    {
        std::cout << "\nFailed to write flow field to " << flowFile << "\n";
    }

    std::cout << "\nSimulation complete.\n";
    return 0;
}
