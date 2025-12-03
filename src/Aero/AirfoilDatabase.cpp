#include "Aero/AirfoilDatabase.h"
#include "Math/Interpolation.h"
#include "IO/CSVReader.h"
#include <stdexcept>
#include <filesystem>   // C++17
#include <limits>

namespace fs = std::filesystem;

void AirfoilDatabase::addPolar(const AirfoilPolar& polar)
{
    database[polar.airfoilName].push_back(polar);
}

bool AirfoilDatabase::loadFromDirectory(const std::string& directoryPath)
{
    // TODO: Decide on file naming convention.
    // For now, we just try to parse every .csv as a single polar.
    try
    {
        for (const auto& entry : fs::directory_iterator(directoryPath))
        {
            if (!entry.is_regular_file())
                continue;

            if (entry.path().extension() == ".csv")
            {
                std::vector<std::vector<std::string>> rows;
                if (!IO::CSVReader::readCSV(entry.path().string(), rows))
                {
                    continue; // skip unreadable files
                }

                // TODO: parse header to get airfoilName, Re, Mach, etc.
                // For now, just skip actual parsing.
            }
        }
    }
    catch (...)
    {
        return false;
    }

    return true;
}

const AirfoilPolar* AirfoilDatabase::findClosestPolar(
    const std::string& airfoilName,
    double Re,
    double Mach
) const
{
    auto it = database.find(airfoilName);
    if (it == database.end() || it->second.empty())
    {
        return nullptr;
    }

    const std::vector<AirfoilPolar>& polars = it->second;

    // Very simple: choose the polar with minimal |Re_polar - Re|
    double bestScore = std::numeric_limits<double>::max();
    const AirfoilPolar* bestPolar = nullptr;

    for (const auto& polar : polars)
    {
        double dRe = polar.Re - Re;
        double dMach = polar.Mach - Mach;
        double score = dRe * dRe + dMach * dMach;

        if (score < bestScore)
        {
            bestScore = score;
            bestPolar = &polar;
        }
    }

    return bestPolar;
}

double AirfoilDatabase::getCl(const std::string& airfoilName, double alphaDeg, double Re, double Mach) const
{
    const AirfoilPolar* polar = findClosestPolar(airfoilName, Re, Mach);
    if (!polar)
    {
        throw std::runtime_error("No polar data for airfoil: " + airfoilName);
    }
    return MathUtils::linearInterpolate(polar->alphaDeg, polar->Cl, alphaDeg);
}

double AirfoilDatabase::getCd(const std::string& airfoilName, double alphaDeg, double Re, double Mach) const
{
    const AirfoilPolar* polar = findClosestPolar(airfoilName, Re, Mach);
    if (!polar)
    {
        throw std::runtime_error("No polar data for airfoil: " + airfoilName);
    }
    return MathUtils::linearInterpolate(polar->alphaDeg, polar->Cd, alphaDeg);
}

double AirfoilDatabase::getCm(const std::string& airfoilName, double alphaDeg, double Re, double Mach) const
{
    const AirfoilPolar* polar = findClosestPolar(airfoilName, Re, Mach);
    if (!polar)
    {
        throw std::runtime_error("No polar data for airfoil: " + airfoilName);
    }
    return MathUtils::linearInterpolate(polar->alphaDeg, polar->Cm, alphaDeg);
}
