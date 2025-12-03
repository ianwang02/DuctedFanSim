#pragma once
#include <string>
#include "Core/OperatingCondition.h"

// Config: simulation settings and file paths.
// For now we hardcode values and later we can add file parsing.

class Config
{
public:
    // Paths to data directories
    std::string airfoilDataDir;
    std::string nasaDataDir;

    // STL file paths (optional)
    std::string ductSTLPath;
    std::string rotorSTLPath;

    // Output files
    std::string flowFieldOutputPath;
    std::string performanceOutputPath;

    // Operating condition
    OperatingCondition opCond;

    // Basic rotor/duct settings (can be refined later)
    double rpm;
    unsigned int bladeCount;

    Config();

    // (Later) load from JSON/TXT file or command-line
    bool loadFromFile(const std::string& filePath);

    // For now: just print summary to console
    void printSummary() const;
};
