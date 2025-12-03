#pragma once
#include <string>
#include <vector>

namespace IO
{
    // Very simple CSV reader: splits lines by commas.
    // We will improve as needed.
    class CSVReader
    {
    public:
        static bool readCSV(
            const std::string& filePath,
            std::vector<std::vector<std::string>>& rows
        );
    }
    ;
}
