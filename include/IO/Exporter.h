#pragma once
#include <string>
#include "Flow/FlowField.h"

namespace IO
{
    class FlowFieldCSVExporter
    {
    public:
        static bool writeCSV(
            const std::string& filePath,
            const FlowField& field
        );
    };
}
