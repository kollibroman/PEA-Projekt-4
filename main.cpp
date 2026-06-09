#include <iostream>
#include <string>
#include <iomanip>
#include <sstream>

import GeneticAlgorithm;
import NN;
import AlgorithmTester;
import FileDataProvider;
import RandomDataGenerator;
import TestData;
import AlgorithmResult;
import DefinitelyNotAVector;
import GAConfig;

void DisplayConfig(const GAConfig& config)
{
    std::cout << "\n=== AKTUALNA KONFIGURACJA GA ===\n";
    std::cout << "Rozmiar populacji:       " << config.population_size << "\n";
    std::cout << "Wspolczynnik mutacji:    " << config.mutation_rate << "\n";
    std::cout << "Wspolczynnik krzyzowania:" << config.crossover_rate << "\n";
    std::cout << "Limit czasu:             " << config.time_limit_seconds << " s\n";
    std::cout << "Metoda krzyzowania:      ";
    if (config.crossover_method == CrossoverMethod::OX)
        std::cout << "OX (Order Crossover)\n";
    else
        std::cout << "PMX (Partially Mapped Crossover)\n";
    std::cout << "Metoda mutacji:          ";
    if (config.mutation_method == MutationMethod::Swap)
        std::cout << "Swap\n";
    else
        std::cout << "Invert (2-opt)\n";
    std::cout << "Metoda selekcji:         ";
    if (config.selection_method == SelectionMethod::Tournament)
        std::cout << "Tournament (rozmiar turnieju: " << config.tournament_size << ")\n";
    else
        std::cout << "Roulette Wheel\n";
    std::cout << "================================\n\n";
}

int main()
{
    TestData current_data;
    GAConfig config;
    GeneticAlgorithm ga(config);

    int choice;
    do
    {
        std::cout << "\n======== MENU (ATSP - Algorytm Genetyczny) ========\n";
        std::cout << " 1. Wczytaj dane z pliku i wyswietl\n";
        std::cout << " 2. Ustaw kryterium stopu (czas)\n";
        std::cout << " 3. Ustaw rozmiar populacji poczatkowej\n";
        std::cout << " 4. Ustaw wspolczynnik mutacji\n";
        std::cout << " 5. Ustaw wspolczynnik krzyzowania\n";
        std::cout << " 6. Wybierz metode krzyzowania\n";
        std::cout << " 7. Wybierz metode mutacji\n";
        std::cout << " 8. Wybierz metode selekcji\n";
        std::cout << " 9. Uruchom algorytm i wyswietl wyniki\n";
        std::cout << "---\n";
        std::cout << "10. [EXP] Zaleznosc od rozmiaru instancji\n";
        std::cout << "11. [EXP] Wplyw rozmiaru populacji\n";
        std::cout << "12. [EXP] Wplyw metody mutacji\n";
        std::cout << "---\n";
        std::cout << " 0. Wyjscie\n";
        std::cout << "====================================================\n";
        std::cout << "Wybor: ";

        if (!(std::cin >> choice))
        {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            choice = -1;
        }

        switch (choice)
        {
        case 1:
        {
            std::string path;
            std::cout << "Podaj sciezke do pliku (.atsp lub prosty format): ";
            std::cin >> path;
            FileDataProvider fdp(path);
            current_data = fdp.GetRequiredData();
            if (current_data.size > 0)
            {
                std::cout << "Wczytano pomyslnie (N = " << current_data.size << ").\n";
                AlgorithmTester::DisplayMatrix(current_data);
            }
            else
                std::cout << "Blad wczytywania pliku.\n";
            break;
        }
        case 2:
        {
            double time_s;
            std::cout << "Podaj maksymalny czas dzialania [s]: ";
            std::cin >> time_s;
            if (std::cin.fail() || time_s <= 0)
            {
                std::cin.clear();
                std::cin.ignore(10000, '\n');
                std::cout << "Bledna wartosc. Czas musi byc > 0.\n";
            }
            else
            {
                config.time_limit_seconds = time_s;
                ga.SetConfig(config);
                std::cout << "Ustawiono limit czasu: " << time_s << " s\n";
            }
            break;
        }
        case 3:
        {
            int pop_size;
            std::cout << "Podaj rozmiar populacji poczatkowej: ";
            std::cin >> pop_size;
            if (std::cin.fail() || pop_size < 2)
            {
                std::cin.clear();
                std::cin.ignore(10000, '\n');
                std::cout << "Bledna wartosc. Rozmiar populacji musi byc >= 2.\n";
            }
            else
            {
                config.population_size = pop_size;
                ga.SetConfig(config);
                std::cout << "Ustawiono rozmiar populacji: " << pop_size << "\n";
            }
            break;
        }
        case 4:
        {
            double mut_rate;
            std::cout << "Podaj wspolczynnik mutacji [0, 1]: ";
            std::cin >> mut_rate;
            if (std::cin.fail() || mut_rate < 0.0 || mut_rate > 1.0)
            {
                std::cin.clear();
                std::cin.ignore(10000, '\n');
                std::cout << "Bledna wartosc. Wspolczynnik mutacji musi byc w zakresie [0, 1].\n";
            }
            else
            {
                config.mutation_rate = mut_rate;
                ga.SetConfig(config);
                std::cout << "Ustawiono wspolczynnik mutacji: " << mut_rate << "\n";
            }
            break;
        }
        case 5:
        {
            double cross_rate;
            std::cout << "Podaj wspolczynnik krzyzowania [0, 1]: ";
            std::cin >> cross_rate;
            if (std::cin.fail() || cross_rate < 0.0 || cross_rate > 1.0)
            {
                std::cin.clear();
                std::cin.ignore(10000, '\n');
                std::cout << "Bledna wartosc. Wspolczynnik krzyzowania musi byc w zakresie [0, 1].\n";
            }
            else
            {
                config.crossover_rate = cross_rate;
                ga.SetConfig(config);
                std::cout << "Ustawiono wspolczynnik krzyzowania: " << cross_rate << "\n";
            }
            break;
        }
        case 6:
        {
            int method;
            std::cout << "Wybierz metode krzyzowania:\n";
            std::cout << "  1. OX (Order Crossover)\n";
            std::cout << "  2. PMX (Partially Mapped Crossover)\n";
            std::cout << "Wybor: ";
            std::cin >> method;
            if (method == 1)
            {
                config.crossover_method = CrossoverMethod::OX;
                ga.SetConfig(config);
                std::cout << "Ustawiono metode krzyzowania: OX\n";
            }
            else if (method == 2)
            {
                config.crossover_method = CrossoverMethod::PMX;
                ga.SetConfig(config);
                std::cout << "Ustawiono metode krzyzowania: PMX\n";
            }
            else
            {
                std::cout << "Bledny wybor.\n";
            }
            break;
        }
        case 7:
        {
            int method;
            std::cout << "Wybierz metode mutacji:\n";
            std::cout << "  1. Swap\n";
            std::cout << "  2. Invert (2-opt)\n";
            std::cout << "Wybor: ";
            std::cin >> method;
            if (method == 1)
            {
                config.mutation_method = MutationMethod::Swap;
                ga.SetConfig(config);
                std::cout << "Ustawiono metode mutacji: Swap\n";
            }
            else if (method == 2)
            {
                config.mutation_method = MutationMethod::Invert;
                ga.SetConfig(config);
                std::cout << "Ustawiono metode mutacji: Invert\n";
            }
            else
            {
                std::cout << "Bledny wybor.\n";
            }
            break;
        }
        case 8:
        {
            int method;
            std::cout << "Wybierz metode selekcji:\n";
            std::cout << "  1. Tournament\n";
            std::cout << "  2. Roulette Wheel\n";
            std::cout << "Wybor: ";
            std::cin >> method;
            if (method == 1)
            {
                config.selection_method = SelectionMethod::Tournament;
                int t_size;
                std::cout << "Podaj rozmiar turnieju (domyslnie " << config.tournament_size << "): ";
                std::cin >> t_size;
                if (!std::cin.fail() && t_size >= 2)
                {
                    config.tournament_size = t_size;
                }
                else
                {
                    std::cin.clear();
                    std::cin.ignore(10000, '\n');
                    std::cout << "Zostawiono domyslny rozmiar turnieju: " << config.tournament_size << "\n";
                }
                ga.SetConfig(config);
                std::cout << "Ustawiono metode selekcji: Tournament (rozmiar: " << config.tournament_size << ")\n";
            }
            else if (method == 2)
            {
                config.selection_method = SelectionMethod::RouletteWheel;
                ga.SetConfig(config);
                std::cout << "Ustawiono metode selekcji: Roulette Wheel\n";
            }
            else
            {
                std::cout << "Bledny wybor.\n";
            }
            break;
        }
        case 9:
        {
            if (current_data.size > 0)
            {
                int optimal_cost = 0;
                std::cout << "Podaj optymalny koszt (lub 0 aby pominac): ";
                std::cin >> optimal_cost;
                if (std::cin.fail())
                {
                    std::cin.clear();
                    std::cin.ignore(10000, '\n');
                    optimal_cost = 0;
                }

                ga.SetConfig(config);
                DisplayConfig(config);
                AlgorithmTester::RunAndPrintSingleResult(ga, current_data, optimal_cost);
            }
            else
                std::cout << "Brak danych. Najpierw wczytaj dane.\n";
            break;
        }
        case 10:
        {
            std::string data_folder, solutions_file;
            int runs;
            std::cout << "Podaj sciezke do folderu z plikami .atsp: ";
            std::cin >> data_folder;
            std::cout << "Podaj sciezke do pliku solutions: ";
            std::cin >> solutions_file;
            std::cout << "Ile uruchomien per instancja (domyslnie 10): ";
            std::cin >> runs;
            if (std::cin.fail() || runs <= 0)
            {
                std::cin.clear();
                std::cin.ignore(10000, '\n');
                runs = 10;
            }

            DisplayConfig(config);
            AlgorithmTester::RunSizeExperiment(config, data_folder, solutions_file, runs);
            break;
        }
        case 11:
        {
            std::string data_folder, solutions_file;
            int runs;
            DefinitelyNotAVector<int> pop_sizes;

            std::cout << "Podaj rozmiary populacji (oddzielone przecinkami, np. 50,100,200): ";
            std::cin.ignore(10000, '\n');
            std::string pop_input;
            std::getline(std::cin, pop_input);

            std::istringstream iss(pop_input);
            std::string token;
            while (std::getline(iss, token, ','))
            {
                try
                {
                    int val = std::stoi(token);
                    if (val >= 2) pop_sizes.push_back(val);
                }
                catch (...)
                {
                    // ignoruj bledne wartosci
                }
            }

            if (pop_sizes.empty())
            {
                std::cout << "Nie podano prawidlowych rozmiarow populacji.\n";
                break;
            }

            std::cout << "Podaj sciezke do folderu z plikami .atsp: ";
            std::cin >> data_folder;
            std::cout << "Podaj sciezke do pliku solutions: ";
            std::cin >> solutions_file;
            std::cout << "Ile uruchomien per konfiguracja (domyslnie 10): ";
            std::cin >> runs;
            if (std::cin.fail() || runs <= 0)
            {
                std::cin.clear();
                std::cin.ignore(10000, '\n');
                runs = 10;
            }

            DisplayConfig(config);
            AlgorithmTester::RunPopulationSizeExperiment(config, pop_sizes, data_folder, solutions_file, runs);
            break;
        }
        case 12:
        {
            std::string data_folder, solutions_file;
            int runs;
            std::cout << "Podaj sciezke do folderu z plikami .atsp: ";
            std::cin >> data_folder;
            std::cout << "Podaj sciezke do pliku solutions: ";
            std::cin >> solutions_file;
            std::cout << "Ile uruchomien per konfiguracja (domyslnie 10): ";
            std::cin >> runs;
            if (std::cin.fail() || runs <= 0)
            {
                std::cin.clear();
                std::cin.ignore(10000, '\n');
                runs = 10;
            }

            DisplayConfig(config);
            AlgorithmTester::RunMutationMethodExperiment(config, data_folder, solutions_file, runs);
            break;
        }
        case 0:
            std::cout << "Koniec programu.\n";
            break;
        default:
            std::cout << "Bledny wybor.\n";
        }
    } while (choice != 0);

    return 0;
}
