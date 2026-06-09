module;
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <utility>
#include <filesystem>

export module FileDataProvider;

import IDataProvider;
import TestData;
import DefinitelyNotAVector;

export class FileDataProvider : public IDataProvider
{
private:
    std::string filePath;

    bool IsTSPLIBFormat(std::ifstream& file) const
    {
        std::string first_line;
        std::getline(file, first_line);
        file.seekg(0); // Wracamy na poczatek

        return (first_line.find("NAME") != std::string::npos ||
                first_line.find("TYPE") != std::string::npos);
    }

    TestData ParseTSPLIB(std::ifstream& file) const
    {
        int size = 0;
        std::string line;

        while (std::getline(file, line))
        {
            size_t start = line.find_first_not_of(" \t\r\n");
            if (start == std::string::npos) continue;
            line = line.substr(start);

            if (line.find("DIMENSION") != std::string::npos)
            {
                size_t colon_pos = line.find(':');
                if (colon_pos != std::string::npos)
                {
                    std::string num_str = line.substr(colon_pos + 1);
                    size = std::stoi(num_str);
                }
            }
            else if (line.find("EDGE_WEIGHT_SECTION") != std::string::npos)
            {
                break;
            }
        }

        if (size == 0)
        {
            std::cerr << "TSPLIB: Nie znaleziono DIMENSION w pliku: " << filePath << std::endl;
            return { 0, DefinitelyNotAVector<DefinitelyNotAVector<int>>() };
        }

        DefinitelyNotAVector<DefinitelyNotAVector<int>> matrix(size);
        for (int i = 0; i < size; ++i)
        {
            matrix[i] = DefinitelyNotAVector<int>(size);
        }

        int total_elements = size * size;
        int count = 0;

        while (count < total_elements && std::getline(file, line))
        {
            if (line.find("EOF") != std::string::npos) break;

            std::istringstream iss(line);
            int val;
            while (iss >> val && count < total_elements)
            {
                int row = count / size;
                int col = count % size;
                matrix[row][col] = val;
                count++;
            }
        }

        if (count < total_elements)
        {
            std::cerr << "TSPLIB: Wczytano tylko " << count << "/" << total_elements
                      << " elementow macierzy z pliku: " << filePath << std::endl;
        }

        for (int i = 0; i < size; ++i)
        {
            matrix[i][i] = -1;
        }

        return { size, matrix };
    }

    TestData ParseSimple(std::ifstream& file) const
    {
        int size = 0;
        if (!(file >> size))
        {
            std::cerr << "Error reading size from file: " << filePath << std::endl;
            return { 0, DefinitelyNotAVector<DefinitelyNotAVector<int>>() };
        }

        DefinitelyNotAVector<DefinitelyNotAVector<int>> data(size);
        for (int i = 0; i < size; ++i)
        {
            data[i] = DefinitelyNotAVector<int>(size);
            for (int j = 0; j < size; ++j)
            {
                int val;
                if (file >> val)
                {
                    data[i][j] = val;
                }
                else
                {
                    std::cerr << "Error reading matrix element at [" << i << "][" << j << "]" << std::endl;
                }
            }
        }

        return { size, data };
    }

public:
    FileDataProvider(std::string filePath)
    {
        this->filePath = std::move(filePath);
    }

    TestData GetRequiredData() const override
    {
        std::ifstream file(filePath);
        if (!file.is_open())
        {
            std::cerr << "Could not open file: " << filePath << std::endl;
            return { 0, DefinitelyNotAVector<DefinitelyNotAVector<int>>() };
        }

        if (IsTSPLIBFormat(file))
        {
            return ParseTSPLIB(file);
        }
        else
        {
            return ParseSimple(file);
        }
    }

    [[nodiscard]] std::string GetFileName() const
    {
        std::filesystem::path p(filePath);
        return p.stem().string();
    }
};

export class TSPLIBFolderScanner
{
public:
    static DefinitelyNotAVector<std::string> ScanFolder(const std::string& folder_path)
    {
        DefinitelyNotAVector<std::string> files;

        if (!std::filesystem::exists(folder_path))
        {
            std::cerr << "Folder nie istnieje: " << folder_path << std::endl;
            return files;
        }

        for (const auto& entry : std::filesystem::directory_iterator(folder_path))
        {
            if (entry.is_regular_file())
            {
                std::string ext = entry.path().extension().string();
                if (ext == ".atsp" || ext == ".txt" || ext.empty())
                {
                    files.push_back(entry.path().string());
                }
            }
        }

        return files;
    }
};
