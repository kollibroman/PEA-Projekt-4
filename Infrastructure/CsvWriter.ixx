module;

#include <fstream>
#include <string>
#include <iomanip>

export module CsvWriter;

import DefinitelyNotAVector;

export class CsvWriter
{
private:
    std::string filename;
    std::ofstream file;
    bool header_written;

public:
    CsvWriter(const std::string& filepath)
        : filename(filepath), header_written(false)
    {
        file.open(filename);
    }

    ~CsvWriter()
    {
        if (file.is_open())
            file.close();
    }

    bool IsOpen() const
    {
        return file.is_open();
    }
    
    void WriteHeader(const DefinitelyNotAVector<std::string>& headers)
    {
        if (!file.is_open() || header_written)
            return;

        for (size_t i = 0; i < headers.size(); ++i)
        {
            file << headers[i];
            if (i < headers.size() - 1)
                file << ",";
        }
        file << "\n";
        header_written = true;
    }
    
    template<typename... Args>
    void WriteRow(Args... args)
    {
        if (!file.is_open())
            return;

        WriteRowHelper(args...);
        file << "\n";
    }
    
    void Flush()
    {
        if (file.is_open())
            file.flush();
    }

private:
    template<typename T>
    void WriteRowHelper(T value)
    {
        file << value;
    }

    template<typename T, typename... Rest>
    void WriteRowHelper(T first, Rest... rest)
    {
        file << first << ",";
        WriteRowHelper(rest...);
    }
};

// Writer do wynikow eksperymentow SA
export class SAResultsWriter
{
private:
    CsvWriter writer;

public:
    SAResultsWriter(const std::string& filename)
        : writer(filename)
    {
        DefinitelyNotAVector<std::string> headers;
        headers.push_back("Instance");
        headers.push_back("N");
        headers.push_back("Optimal");
        headers.push_back("Found");
        headers.push_back("Error_pct");
        headers.push_back("Time_ms");
        headers.push_back("CoolingScheme");
        headers.push_back("InitialSolution");
        headers.push_back("InitialTemp");
        headers.push_back("CoolingRate");
        headers.push_back("EpochLength");

        writer.WriteHeader(headers);
    }

    void WriteResult(const std::string& instance, int n, int optimal, int found,
                     double error_pct, double time_ms, const std::string& cooling,
                     const std::string& init_sol, double init_temp, double cooling_rate,
                     int epoch_length)
    {
        writer.WriteRow(instance, ",", n, ",", optimal, ",", found, ",",
                       std::fixed, std::setprecision(2), error_pct, ",",
                       std::fixed, std::setprecision(1), time_ms, ",",
                       cooling, ",", init_sol, ",",
                       std::fixed, std::setprecision(1), init_temp, ",",
                       std::fixed, std::setprecision(4), cooling_rate, ",",
                       epoch_length);
    }

    void Flush()
    {
        writer.Flush();
    }

    bool IsOpen() const
    {
        return writer.IsOpen();
    }
};
