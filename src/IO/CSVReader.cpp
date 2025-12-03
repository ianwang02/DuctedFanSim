#include "IO/CSVReader.h"
#include <fstream>
#include <sstream>

namespace IO
{
    bool CSVReader::readCSV(
        const std::string& filePath,
        std::vector<std::vector<std::string>>& rows
    )
    {
        rows.clear();
        std::ifstream file(filePath);
        if (!file.is_open())
        {
            return false;
        }

        std::string line;
        while (std::getline(file, line))
        {
            std::vector<std::string> tokens;
            std::stringstream ss(line);
            std::string cell;

            while (std::getline(ss, cell, ','))
            {
                tokens.push_back(cell);
            }

            rows.push_back(tokens);
        }

        return true;
    }
}
