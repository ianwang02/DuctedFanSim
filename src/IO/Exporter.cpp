#include "IO/Exporter.h"
#include <fstream>

namespace IO
{
    bool FlowFieldCSVExporter::writeCSV(
        const std::string& filePath,
        const FlowField& field
    )
    {
        std::ofstream out(filePath);
        if (!out.is_open())
        {
            return false;
        }

        // Header
        out << "x,y,z,u,v,w\n";

        for (const auto& p : field.points)
        {
            out << p.x << "," << p.y << "," << p.z << ","
                << p.u << "," << p.v << "," << p.w << "\n";
        }

        return true;
    }
}
