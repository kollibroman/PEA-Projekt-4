module;
#include <random>
#include <chrono>
#include <algorithm>
#include <numeric>

export module GeneticAlgorithm;

import IAtspAlgorithm;
import TestData;
import AlgorithmResult;
import GAConfig;
import DefinitelyNotAVector;

export class GeneticAlgorithm : public IAtspAlgorithm {
private:
    GAConfig config_;

    struct Individual {
        DefinitelyNotAVector<int> path;
        int cost;
    };

    // Inicjalizacja
    static DefinitelyNotAVector<int> GenerateRandomPermutation(int n, std::mt19937& gen);
    int CalculatePathCost(const TestData& data, const DefinitelyNotAVector<int>& path) const;

    // Selekcja
    int TournamentSelection(const DefinitelyNotAVector<Individual>& population, std::mt19937& gen) const;
    int RouletteWheelSelection(const DefinitelyNotAVector<Individual>& population, std::mt19937& gen) const;
    int SelectParent(const DefinitelyNotAVector<Individual>& population, std::mt19937& gen) const;

    // Krzyżowanie
    DefinitelyNotAVector<int> OrderCrossover(const DefinitelyNotAVector<int>& parent1,
                                              const DefinitelyNotAVector<int>& parent2,
                                              std::mt19937& gen) const;
    DefinitelyNotAVector<int> PMXCrossover(const DefinitelyNotAVector<int>& parent1,
                                            const DefinitelyNotAVector<int>& parent2,
                                            std::mt19937& gen) const;
    DefinitelyNotAVector<int> Crossover(const DefinitelyNotAVector<int>& parent1,
                                         const DefinitelyNotAVector<int>& parent2,
                                         std::mt19937& gen) const;

    // Mutacja
    void SwapMutation(DefinitelyNotAVector<int>& path, std::mt19937& gen) const;
    void InvertMutation(DefinitelyNotAVector<int>& path, std::mt19937& gen) const;
    void Mutate(DefinitelyNotAVector<int>& path, std::mt19937& gen) const;

public:
    GeneticAlgorithm();
    explicit GeneticAlgorithm(const GAConfig& config);

    void SetConfig(const GAConfig& config);
    GAConfig GetConfig() const;

    AlgorithmResult SolveProblem(const TestData& data) const override;
};

// --- Implementacje publiczne ---

GeneticAlgorithm::GeneticAlgorithm() : config_() {}

GeneticAlgorithm::GeneticAlgorithm(const GAConfig& config) : config_(config) {}

void GeneticAlgorithm::SetConfig(const GAConfig& config) {
    config_ = config;
}

GAConfig GeneticAlgorithm::GetConfig() const {
    return config_;
}

// --- Inicjalizacja ---

DefinitelyNotAVector<int> GeneticAlgorithm::GenerateRandomPermutation(int n, std::mt19937& gen) {
    // Ścieżka: [0, losowa permutacja 1..n-1, 0] — łącznie n+1 elementów
    DefinitelyNotAVector<int> path;
    path.push_back(0);

    // Dodaj miasta 1..n-1
    for (int i = 1; i < n; ++i) {
        path.push_back(i);
    }

    // Fisher-Yates shuffle na pozycjach [1..n-1] (indeksy w path)
    for (int i = n - 1; i > 1; --i) {
        std::uniform_int_distribution<int> dist(1, i);
        int j = dist(gen);
        int tmp = path[i];
        path[i] = path[j];
        path[j] = tmp;
    }

    // Zamknij cykl
    path.push_back(0);

    return path;
}

int GeneticAlgorithm::CalculatePathCost(const TestData& data, const DefinitelyNotAVector<int>& path) const {
    int total_cost = 0;
    for (size_t i = 0; i + 1 < path.size(); ++i) {
        total_cost += data.matrix[path[i]][path[i + 1]];
    }
    return total_cost;
}

// --- Selekcja ---

int GeneticAlgorithm::TournamentSelection(const DefinitelyNotAVector<Individual>& population, std::mt19937& gen) const {
    int pop_size = static_cast<int>(population.size());
    int t_size = config_.tournament_size;
    if (t_size > pop_size) t_size = pop_size;

    std::uniform_int_distribution<int> dist(0, pop_size - 1);

    int best_idx = dist(gen);
    int best_cost = population[best_idx].cost;

    for (int i = 1; i < t_size; ++i) {
        int idx = dist(gen);
        if (population[idx].cost < best_cost) {
            best_cost = population[idx].cost;
            best_idx = idx;
        }
    }

    return best_idx;
}

int GeneticAlgorithm::RouletteWheelSelection(const DefinitelyNotAVector<Individual>& population, std::mt19937& gen) const {
    int pop_size = static_cast<int>(population.size());

    // Znajdź max_cost
    int max_cost = population[0].cost;
    for (int i = 1; i < pop_size; ++i) {
        if (population[i].cost > max_cost) {
            max_cost = population[i].cost;
        }
    }

    // Oblicz total fitness: fitness_i = max_cost - cost_i + 1
    double total_fitness = 0.0;
    for (int i = 0; i < pop_size; ++i) {
        total_fitness += static_cast<double>(max_cost - population[i].cost + 1);
    }

    // Losuj wartość w [0, total_fitness)
    std::uniform_real_distribution<double> dist(0.0, total_fitness);
    double rand_val = dist(gen);

    // Kumulatywna suma
    double cumulative = 0.0;
    for (int i = 0; i < pop_size; ++i) {
        cumulative += static_cast<double>(max_cost - population[i].cost + 1);
        if (rand_val < cumulative) {
            return i;
        }
    }

    return pop_size - 1;
}

int GeneticAlgorithm::SelectParent(const DefinitelyNotAVector<Individual>& population, std::mt19937& gen) const {
    if (config_.selection_method == SelectionMethod::Tournament) {
        return TournamentSelection(population, gen);
    }
    return RouletteWheelSelection(population, gen);
}

// --- Krzyżowanie ---

DefinitelyNotAVector<int> GeneticAlgorithm::OrderCrossover(const DefinitelyNotAVector<int>& parent1,
                                                            const DefinitelyNotAVector<int>& parent2,
                                                            std::mt19937& gen) const {
    // path ma n+1 elementów: [0, cities..., 0]. Wewnętrzna część to indeksy [1..n-1].
    int path_len = static_cast<int>(parent1.size());
    int n = path_len - 1; // liczba miast
    int inner_size = n - 1; // liczba miast w wewnętrznej części (bez miasta 0)

    // Wybierz dwa punkty cięcia w wewnętrznej części [1, n-1] (indeksy w path)
    std::uniform_int_distribution<int> dist(1, n - 1);
    int i = dist(gen);
    int j = dist(gen);
    if (i > j) {
        int tmp = i; i = j; j = tmp;
    }

    // Utwórz child z -1 (marker pustych pozycji)
    DefinitelyNotAVector<int> child(path_len, -1);
    child[0] = 0;
    child[n] = 0;

    // Tablica oznaczająca, które miasta już trafiły do child
    DefinitelyNotAVector<bool> in_child(n, false);
    in_child[0] = true; // miasto 0 jest na stałe

    // Kopiuj segment [i..j] z parent1
    for (int k = i; k <= j; ++k) {
        child[k] = parent1[k];
        in_child[parent1[k]] = true;
    }

    // Uzupełnij z parent2 w porządku cyklicznym (zaczynając od j+1)
    // Zbierz miasta z parent2 w kolejności cyklicznej, pomijając te już w child
    DefinitelyNotAVector<int> remaining_cities;
    for (int count = 0; count < inner_size; ++count) {
        int pos = ((j + 1 - 1) + count) % inner_size + 1; // cyklicznie po pozycjach [1..n-1]
        int city = parent2[pos];
        if (!in_child[city]) {
            remaining_cities.push_back(city);
        }
    }

    // Wstaw remaining_cities na puste pozycje w child (cyklicznie od j+1)
    int r_idx = 0;
    for (int count = 0; count < inner_size; ++count) {
        int pos = ((j + 1 - 1) + count) % inner_size + 1;
        if (child[pos] == -1) {
            child[pos] = remaining_cities[r_idx];
            r_idx++;
        }
    }

    return child;
}

DefinitelyNotAVector<int> GeneticAlgorithm::PMXCrossover(const DefinitelyNotAVector<int>& parent1,
                                                          const DefinitelyNotAVector<int>& parent2,
                                                          std::mt19937& gen) const {
    int path_len = static_cast<int>(parent1.size());
    int n = path_len - 1; // liczba miast

    // Wybierz dwa punkty cięcia w [1, n-1]
    std::uniform_int_distribution<int> dist(1, n - 1);
    int i = dist(gen);
    int j = dist(gen);
    if (i > j) {
        int tmp = i; i = j; j = tmp;
    }

    // Utwórz child wypełniony -1
    DefinitelyNotAVector<int> child(path_len, -1);
    child[0] = 0;
    child[n] = 0;

    // Kopiuj segment z parent1 do child
    for (int k = i; k <= j; ++k) {
        child[k] = parent1[k];
    }

    // Dla każdego elementu parent2[k] w segmencie, jeśli nie jest w child — znajdź pozycję
    for (int k = i; k <= j; ++k) {
        int city = parent2[k];
        // Sprawdź czy city jest już gdzieś w segmencie child
        bool found = false;
        for (int m = i; m <= j; ++m) {
            if (child[m] == city) {
                found = true;
                break;
            }
        }
        if (found) continue;

        // Śledź łańcuch mapowań: parent1[k] -> szukaj pozycji w parent2
        int mapped = parent1[k];
        int pos = -1;
        // Znajdź pozycję mapped w parent2
        for (int m = 1; m < n; ++m) {
            if (parent2[m] == mapped) {
                pos = m;
                break;
            }
        }
        // Jeśli pos jest w segmencie, kontynuuj łańcuch
        while (pos >= i && pos <= j) {
            mapped = parent1[pos];
            pos = -1;
            for (int m = 1; m < n; ++m) {
                if (parent2[m] == mapped) {
                    pos = m;
                    break;
                }
            }
        }
        // Wstaw city na znalezioną pozycję
        if (pos != -1) {
            child[pos] = city;
        }
    }

    // Uzupełnij pozostałe puste pozycje z parent2
    for (int k = 1; k < n; ++k) {
        if (child[k] == -1) {
            child[k] = parent2[k];
        }
    }

    return child;
}

DefinitelyNotAVector<int> GeneticAlgorithm::Crossover(const DefinitelyNotAVector<int>& parent1,
                                                       const DefinitelyNotAVector<int>& parent2,
                                                       std::mt19937& gen) const {
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    if (dist(gen) < config_.crossover_rate) {
        if (config_.crossover_method == CrossoverMethod::OX) {
            return OrderCrossover(parent1, parent2, gen);
        }
        return PMXCrossover(parent1, parent2, gen);
    }

    // Brak krzyżowania — zwróć kopię parent1
    DefinitelyNotAVector<int> copy = parent1;
    return copy;
}

// --- Mutacja ---

void GeneticAlgorithm::SwapMutation(DefinitelyNotAVector<int>& path, std::mt19937& gen) const {
    int last_idx = static_cast<int>(path.size()) - 2; // path.size()-2 = ostatni wewnętrzny indeks
    if (last_idx < 2) return; // potrzeba co najmniej 2 wewnętrznych pozycji [1, last_idx]

    std::uniform_int_distribution<int> dist(1, last_idx);
    int i = dist(gen);
    int j = dist(gen);
    while (j == i) {
        j = dist(gen);
    }

    int tmp = path[i];
    path[i] = path[j];
    path[j] = tmp;
}

void GeneticAlgorithm::InvertMutation(DefinitelyNotAVector<int>& path, std::mt19937& gen) const {
    int last_idx = static_cast<int>(path.size()) - 2;
    if (last_idx < 2) return;

    std::uniform_int_distribution<int> dist(1, last_idx);
    int i = dist(gen);
    int j = dist(gen);
    while (j == i) {
        j = dist(gen);
    }
    if (i > j) {
        int tmp = i; i = j; j = tmp;
    }

    // Odwróć segment path[i..j]
    int left = i, right = j;
    while (left < right) {
        int tmp = path[left];
        path[left] = path[right];
        path[right] = tmp;
        left++;
        right--;
    }
}

void GeneticAlgorithm::Mutate(DefinitelyNotAVector<int>& path, std::mt19937& gen) const {
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    if (dist(gen) < config_.mutation_rate) {
        if (config_.mutation_method == MutationMethod::Swap) {
            SwapMutation(path, gen);
        } else {
            InvertMutation(path, gen);
        }
    }
}

// --- Główna pętla ewolucyjna ---

AlgorithmResult GeneticAlgorithm::SolveProblem(const TestData& data) const {
    AlgorithmResult result;
    result.cost = 0;
    result.best_found_time_ms = 0.0;
    result.average_cost = 0.0;

    // Edge case: za mało miast
    if (data.size < 2) {
        return result;
    }

    int n = data.size;

    // Inicjalizacja generatora losowego
    std::random_device rd;
    std::mt19937 gen(rd());

    int pop_size = config_.population_size;
    if (pop_size < 2) pop_size = 2;

    // 1. Utwórz populację początkową
    DefinitelyNotAVector<Individual> population;
    for (int i = 0; i < pop_size; ++i) {
        Individual ind;
        ind.path = GenerateRandomPermutation(n, gen);
        ind.cost = CalculatePathCost(data, ind.path);
        population.push_back(std::move(ind));
    }

    // 2. Znajdź najlepszego (global_best)
    Individual global_best;
    global_best.path = population[0].path;
    global_best.cost = population[0].cost;
    for (int i = 1; i < pop_size; ++i) {
        if (population[i].cost < global_best.cost) {
            global_best.path = population[i].path;
            global_best.cost = population[i].cost;
        }
    }

    // 3. Start timer
    auto start_time = std::chrono::high_resolution_clock::now();
    double best_found_time_ms = 0.0;
    double time_limit_ms = config_.time_limit_seconds * 1000.0;

    // 4. Główna pętla
    while (true) {
        auto current_time = std::chrono::high_resolution_clock::now();
        double elapsed_ms = std::chrono::duration<double, std::milli>(current_time - start_time).count();
        if (elapsed_ms >= time_limit_ms) break;

        // a. Nowa populacja
        DefinitelyNotAVector<Individual> new_population;

        // b. Elityzm — kopiuj global_best
        Individual elite;
        elite.path = global_best.path;
        elite.cost = global_best.cost;
        new_population.push_back(std::move(elite));

        // c. Generuj potomków
        while (static_cast<int>(new_population.size()) < pop_size) {
            int parent1_idx = SelectParent(population, gen);
            int parent2_idx = SelectParent(population, gen);

            DefinitelyNotAVector<int> child_path = Crossover(
                population[parent1_idx].path,
                population[parent2_idx].path,
                gen
            );

            Mutate(child_path, gen);

            Individual child;
            child.cost = CalculatePathCost(data, child_path);
            child.path = std::move(child_path);
            new_population.push_back(std::move(child));
        }

        // d. Zastąp populację
        population = std::move(new_population);

        // e. Aktualizuj global_best
        for (int i = 0; i < static_cast<int>(population.size()); ++i) {
            if (population[i].cost < global_best.cost) {
                global_best.path = population[i].path;
                global_best.cost = population[i].cost;

                auto found_time = std::chrono::high_resolution_clock::now();
                best_found_time_ms = std::chrono::duration<double, std::milli>(found_time - start_time).count();
            }
        }
    }

    // 5. Średni koszt z ostatniej populacji
    double total_cost = 0.0;
    for (int i = 0; i < static_cast<int>(population.size()); ++i) {
        total_cost += static_cast<double>(population[i].cost);
    }
    double average_cost = total_cost / static_cast<double>(population.size());

    // 6. Zwróć wynik
    result.cost = global_best.cost;
    result.path = global_best.path;
    result.best_found_time_ms = best_found_time_ms;
    result.average_cost = average_cost;

    return result;
}
