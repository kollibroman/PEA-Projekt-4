module;

#include <string>
#include <sstream>

export module CsvParser;

import TestData;
import DefinitelyNotAVector;

export class Parser
{
public:
    Parser();

    static DefinitelyNotAVector<int> ParseDataIntoVector(const std::string& rawData)
    {
        DefinitelyNotAVector<int> data;
        std::istringstream iss(rawData);
        int token;

        while (iss >> token)
        {
            data.push_back(token);
        }

        return data;
    }
};