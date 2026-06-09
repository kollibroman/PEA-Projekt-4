module;

#include <chrono>
#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <sstream>
#include <limits>
#include <filesystem>

export module AlgorithmTester;

import IAtspAlgorithm;
import TestData;
import AlgorithmResult;
import DefinitelyNotAVector;
import GAConfig;
import GeneticAlgorithm;
import FileDataProvider;

export struct TestInstance
{
    std::string name;
    std::string file_path;
    int optimal_cost;
};

export namespace AlgorithmTester
{
    // Wyswietla macierz kosztow
    void DisplayMatrix(const TestData& data)
    {
        std::cout << "\n=== MACIERZ KOSZTOW (N = " << data.size << ") ===\n";
        for (int i = 0; i < data.size; ++i)
        {
            for (int j = 0; j < data.size; ++j)
            {
                std::cout << std::setw(5) << data.matrix[i][j] << " ";
            }
            std::cout << "\n";
        }
        std::cout << "===================================\n";
    }

    // Oblicza blad wzgledny procentowy
    double CalculateRelativeError(int found_cost, int optimal_cost)
    {
        if (optimal_cost == 0) return 0.0;
        return ((double)(found_cost - optimal_cost) / (double)optimal_cost) * 100.0;
    }

    // Laduje instancje testowe z folderu i pliku z rozwiazaniami
    DefinitelyNotAVector<TestInstance> LoadInstances(const std::string& solutions_file,
                                                     const std::string& data_folder)
    {
        DefinitelyNotAVector<TestInstance> instances;

        std::ifstream file(solutions_file);
        if (!file.is_open())
        {
            std::cerr << "Nie mozna otworzyc pliku solutions: " << solutions_file << std::endl;
            return instances;
        }

        std::string line;
        while (std::getline(file, line))
        {
            if (line.empty() || line[0] == '#') continue;

            size_t colon_pos = line.find(':');
            std::string name;
            int opt_cost = 0;

            if (colon_pos != std::string::npos)
            {
                name = line.substr(0, colon_pos);
                std::string val_str = line.substr(colon_pos + 1);

                size_t start = name.find_first_not_of(" \t");
                size_t end = name.find_last_not_of(" \t");
                if (start != std::string::npos)
                    name = name.substr(start, end - start + 1);

                start = val_str.find_first_not_of(" \t");
                end = val_str.find_last_not_of(" \t\r\n");
                if (start != std::string::npos)
                    val_str = val_str.substr(start, end - start + 1);

                try { opt_cost = std::stoi(val_str); }
                catch (...) { continue; }
            }
            else
            {
                std::istringstream iss(line);
                if (!(iss >> name >> opt_cost)) continue;
            }

            std::string file_path;
            DefinitelyNotAVector<std::string> extensions;
            extensions.push_back(".atsp");
            extensions.push_back(".txt");
            extensions.push_back("");

            for (const auto& ext : extensions)
            {
                std::string candidate = data_folder + "/" + name + ext;
                if (std::filesystem::exists(candidate))
                {
                    file_path = candidate;
                    break;
                }
            }

            if (!file_path.empty())
            {
                TestInstance inst;
                inst.name = name;
                inst.file_path = file_path;
                inst.optimal_cost = opt_cost;
                instances.push_back(inst);
            }
        }

        return instances;
    }

    // Uruchamia algorytm i mierzy czas
    double RunAndGetTime(const IAtspAlgorithm& algorithm, const TestData& data, AlgorithmResult& out_result)
    {
        auto start = std::chrono::high_resolution_clock::now();
        out_result = algorithm.SolveProblem(data);
        auto end = std::chrono::high_resolution_clock::now();

        std::chrono::duration<double, std::milli> duration = end - start;
        return duration.count();
    }

    // Wyswietla wynik pojedynczego uruchomienia algorytmu GA
    void RunAndPrintSingleResult(const IAtspAlgorithm& algorithm,
                                 const TestData& data,
                                 int optimal_cost = -1)
    {
        std::cout << "\n=== WYNIK ALGORYTMU GA ===\n";

        AlgorithmResult result;
        double time_ms = RunAndGetTime(algorithm, data, result);

        std::cout << "Rozmiar problemu (N): " << data.size << "\n";
        std::cout << "Czas wykonania:       " << std::fixed << std::setprecision(3) << time_ms << " ms\n";
        std::cout << "Najlepszy koszt:      " << result.cost << "\n";
        std::cout << "Sredni koszt populacji: " << std::fixed << std::setprecision(2) << result.average_cost << "\n";

        if (optimal_cost > 0)
        {
            double error_best = CalculateRelativeError(result.cost, optimal_cost);
            double error_avg = CalculateRelativeError((int)result.average_cost, optimal_cost);
            std::cout << "Optymalny koszt:      " << optimal_cost << "\n";
            std::cout << "Blad wzgledny (best): " << std::fixed << std::setprecision(2) << error_best << "%\n";
            std::cout << "Blad wzgledny (avg):  " << std::fixed << std::setprecision(2) << error_avg << "%\n";
        }

        std::cout << "Czas najlepszego:     " << std::fixed << std::setprecision(3) << result.best_found_time_ms << " ms\n";

        std::cout << "Znaleziona sciezka:   ";
        size_t path_length = result.path.size();
        for (size_t i = 0; i < path_length; ++i)
        {
            std::cout << result.path[i];
            if (i < path_length - 1)
            {
                std::cout << " -> ";
            }
        }
        std::cout << "\n===================================\n\n";
    }

    // Eksperyment 3.0: zaleznosc czasu i bledu od rozmiaru instancji
    void RunSizeExperiment(const GAConfig& base_config,
                           const std::string& data_folder,
                           const std::string& solutions_file,
                           int runs)
    {
        auto instances = LoadInstances(solutions_file, data_folder);

        if (instances.empty())
        {
            std::cout << "Nie znaleziono instancji. Sprawdz sciezki.\n";
            return;
        }

        std::cout << "\n=== EKSPERYMENT: Zaleznosc od rozmiaru instancji ===\n";
        std::cout << "Znaleziono " << instances.size() << " instancji.\n";
        std::cout << "Uruchomien per instancja: " << runs << "\n";
        std::cout << "Konfiguracja: pop=" << base_config.population_size
                  << ", mut=" << base_config.mutation_rate
                  << ", cross=" << base_config.crossover_rate
                  << ", czas=" << base_config.time_limit_seconds << "s\n\n";

        GeneticAlgorithm ga(base_config);

        std::ofstream csv("wyniki_size.csv");
        csv << "Instance,N,OPT,AvgBestCost,AvgAvgCost,AvgError_best_pct,AvgError_avg_pct,AvgTime_ms,AvgBestFoundTime_ms\n";

        for (size_t idx = 0; idx < instances.size(); ++idx)
        {
            const auto& inst = instances[idx];
            std::cout << "[" << (idx + 1) << "/" << instances.size() << "] " << inst.name
                      << " (N=?, OPT=" << inst.optimal_cost << ") ... ";

            FileDataProvider fdp(inst.file_path);
            TestData data = fdp.GetRequiredData();

            if (data.size == 0)
            {
                std::cout << "BLAD: Nie udalo sie wczytac!\n";
                continue;
            }

            double total_best_cost = 0.0;
            double total_avg_cost = 0.0;
            double total_error_best = 0.0;
            double total_error_avg = 0.0;
            double total_time = 0.0;
            double total_best_found_time = 0.0;

            for (int r = 0; r < runs; ++r)
            {
                AlgorithmResult result;
                double time_ms = RunAndGetTime(ga, data, result);

                double error_best = CalculateRelativeError(result.cost, inst.optimal_cost);
                double error_avg = CalculateRelativeError((int)result.average_cost, inst.optimal_cost);

                total_best_cost += result.cost;
                total_avg_cost += result.average_cost;
                total_error_best += error_best;
                total_error_avg += error_avg;
                total_time += time_ms;
                total_best_found_time += result.best_found_time_ms;
            }

            double avg_best_cost = total_best_cost / runs;
            double avg_avg_cost = total_avg_cost / runs;
            double avg_error_best = total_error_best / runs;
            double avg_error_avg = total_error_avg / runs;
            double avg_time = total_time / runs;
            double avg_best_found_time = total_best_found_time / runs;

            std::cout << "N=" << data.size
                      << " | blad_best=" << std::fixed << std::setprecision(2) << avg_error_best << "%"
                      << " | blad_avg=" << std::setprecision(2) << avg_error_avg << "%"
                      << " | czas=" << std::setprecision(0) << avg_time << "ms\n";

            csv << inst.name << "," << data.size << "," << inst.optimal_cost << ","
                << std::fixed << std::setprecision(2) << avg_best_cost << ","
                << std::setprecision(2) << avg_avg_cost << ","
                << std::setprecision(2) << avg_error_best << ","
                << std::setprecision(2) << avg_error_avg << ","
                << std::setprecision(1) << avg_time << ","
                << std::setprecision(1) << avg_best_found_time << "\n";
            csv.flush();
        }

        csv.close();
        std::cout << "\n=== Wyniki zapisane do: wyniki_size.csv ===\n";
    }

    // Eksperyment 3.5: wplyw rozmiaru populacji
    void RunPopulationSizeExperiment(const GAConfig& base_config,
                                     const DefinitelyNotAVector<int>& pop_sizes,
                                     const std::string& data_folder,
                                     const std::string& solutions_file,
                                     int runs)
    {
        auto instances = LoadInstances(solutions_file, data_folder);

        if (instances.empty())
        {
            std::cout << "Nie znaleziono instancji. Sprawdz sciezki.\n";
            return;
        }

        std::cout << "\n=== EKSPERYMENT: Wplyw rozmiaru populacji ===\n";
        std::cout << "Znaleziono " << instances.size() << " instancji.\n";
        std::cout << "Rozmiary populacji do testowania: ";
        for (size_t i = 0; i < pop_sizes.size(); ++i)
        {
            std::cout << pop_sizes[i];
            if (i < pop_sizes.size() - 1) std::cout << ", ";
        }
        std::cout << "\nUruchomien per konfiguracja: " << runs << "\n\n";

        std::ofstream csv("wyniki_populacja.csv");
        csv << "Instance,N,OPT,PopSize,AvgBestCost,AvgError_pct,AvgTime_ms\n";

        for (size_t p = 0; p < pop_sizes.size(); ++p)
        {
            int pop_size = pop_sizes[p];
            GAConfig config = base_config;
            config.population_size = pop_size;

            GeneticAlgorithm ga(config);

            std::cout << "--- Rozmiar populacji: " << pop_size << " ---\n";

            for (size_t idx = 0; idx < instances.size(); ++idx)
            {
                const auto& inst = instances[idx];
                std::cout << "  [" << (idx + 1) << "/" << instances.size() << "] " << inst.name << " ... ";

                FileDataProvider fdp(inst.file_path);
                TestData data = fdp.GetRequiredData();

                if (data.size == 0)
                {
                    std::cout << "BLAD!\n";
                    continue;
                }

                double total_best_cost = 0.0;
                double total_error = 0.0;
                double total_time = 0.0;

                for (int r = 0; r < runs; ++r)
                {
                    AlgorithmResult result;
                    double time_ms = RunAndGetTime(ga, data, result);

                    double error = CalculateRelativeError(result.cost, inst.optimal_cost);
                    total_best_cost += result.cost;
                    total_error += error;
                    total_time += time_ms;
                }

                double avg_best_cost = total_best_cost / runs;
                double avg_error = total_error / runs;
                double avg_time = total_time / runs;

                std::cout << "blad=" << std::fixed << std::setprecision(2) << avg_error << "%"
                          << " | czas=" << std::setprecision(0) << avg_time << "ms\n";

                csv << inst.name << "," << data.size << "," << inst.optimal_cost << ","
                    << pop_size << ","
                    << std::fixed << std::setprecision(2) << avg_best_cost << ","
                    << std::setprecision(2) << avg_error << ","
                    << std::setprecision(1) << avg_time << "\n";
                csv.flush();
            }
        }

        csv.close();
        std::cout << "\n=== Wyniki zapisane do: wyniki_populacja.csv ===\n";
    }

    // Eksperyment 4.0: wplyw metody mutacji (Swap vs Invert)
    void RunMutationMethodExperiment(const GAConfig& base_config,
                                     const std::string& data_folder,
                                     const std::string& solutions_file,
                                     int runs)
    {
        auto instances = LoadInstances(solutions_file, data_folder);

        if (instances.empty())
        {
            std::cout << "Nie znaleziono instancji. Sprawdz sciezki.\n";
            return;
        }

        std::cout << "\n=== EKSPERYMENT: Wplyw metody mutacji (Swap vs Invert) ===\n";
        std::cout << "Znaleziono " << instances.size() << " instancji.\n";
        std::cout << "Uruchomien per konfiguracja: " << runs << "\n\n";

        std::ofstream csv("wyniki_mutacja.csv");
        csv << "Instance,N,OPT,MutationMethod,AvgBestCost,AvgError_pct,AvgTime_ms\n";

        // Dwie metody mutacji do porownania
        MutationMethod methods[] = { MutationMethod::Swap, MutationMethod::Invert };
        const char* method_names[] = { "Swap", "Invert" };

        for (int m = 0; m < 2; ++m)
        {
            GAConfig config = base_config;
            config.mutation_method = methods[m];

            GeneticAlgorithm ga(config);

            std::cout << "--- Metoda mutacji: " << method_names[m] << " ---\n";

            for (size_t idx = 0; idx < instances.size(); ++idx)
            {
                const auto& inst = instances[idx];
                std::cout << "  [" << (idx + 1) << "/" << instances.size() << "] " << inst.name << " ... ";

                FileDataProvider fdp(inst.file_path);
                TestData data = fdp.GetRequiredData();

                if (data.size == 0)
                {
                    std::cout << "BLAD!\n";
                    continue;
                }

                double total_best_cost = 0.0;
                double total_error = 0.0;
                double total_time = 0.0;

                for (int r = 0; r < runs; ++r)
                {
                    AlgorithmResult result;
                    double time_ms = RunAndGetTime(ga, data, result);

                    double error = CalculateRelativeError(result.cost, inst.optimal_cost);
                    total_best_cost += result.cost;
                    total_error += error;
                    total_time += time_ms;
                }

                double avg_best_cost = total_best_cost / runs;
                double avg_error = total_error / runs;
                double avg_time = total_time / runs;

                std::cout << "blad=" << std::fixed << std::setprecision(2) << avg_error << "%"
                          << " | czas=" << std::setprecision(0) << avg_time << "ms\n";

                csv << inst.name << "," << data.size << "," << inst.optimal_cost << ","
                    << method_names[m] << ","
                    << std::fixed << std::setprecision(2) << avg_best_cost << ","
                    << std::setprecision(2) << avg_error << ","
                    << std::setprecision(1) << avg_time << "\n";
                csv.flush();
            }
        }

        csv.close();
        std::cout << "\n=== Wyniki zapisane do: wyniki_mutacja.csv ===\n";
    }
}
