module;
#include <limits>
export module NN;

import IAtspAlgorithm;
import TestData;
import AlgorithmResult;
import DefinitelyNotAVector;

export class NN : public IAtspAlgorithm
{
public:
    AlgorithmResult SolveProblem(const TestData& data) const override
    {
        int n = data.size;
        AlgorithmResult result;
        result.cost = 0;

        if (n < 2) return result;

        DefinitelyNotAVector<bool> visited(n, false);
        result.path = DefinitelyNotAVector<int>(n + 1);

        int current_city = 0;
        visited[current_city] = true;
        result.path[0] = current_city;

        for (int step = 1; step < n; ++step)
        {
            int min_cost = std::numeric_limits<int>::max();
            int best_next_city = -1;

            for (int j = 0; j < n; ++j)
            {
                if (!visited[j] && data.matrix[current_city][j] != -1 && data.matrix[current_city][j] < min_cost)
                {
                    min_cost = data.matrix[current_city][j];
                    best_next_city = j;
                }
            }
            
            if (best_next_city != -1)
            {
                visited[best_next_city] = true;
                result.cost += min_cost;
                current_city = best_next_city;
                result.path[step] = current_city;
            }
        }
        
        result.cost += data.matrix[current_city][0];
        result.path[n] = 0;

        return result;
    }
};
