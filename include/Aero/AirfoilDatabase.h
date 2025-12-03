#pragma once
#include <string>
#include <map>
#include <vector>
#include "Aero/AirfoilPolar.h"

class AirfoilDatabase
{
public:
    AirfoilDatabase() = default;

    // Load all polars from a directory (later: iterate over files)
    bool loadFromDirectory(const std::string& directoryPath);

    // Add a single polar (e.g., loaded from one file)
    void addPolar(const AirfoilPolar& polar);

    // Query Cl, Cd, Cm for given airfoil, alpha [deg], Re, Mach (approximate)
    double getCl(const std::string& airfoilName, double alphaDeg, double Re, double Mach) const;
    double getCd(const std::string& airfoilName, double alphaDeg, double Re, double Mach) const;
    double getCm(const std::string& airfoilName, double alphaDeg, double Re, double Mach) const;

private:
    // For each airfoil name, store a list of polars at different Re/Mach
    std::map<std::string, std::vector<AirfoilPolar>> database;

    const AirfoilPolar* findClosestPolar(
        const std::string& airfoilName,
        double Re,
        double Mach
    ) const;
};
