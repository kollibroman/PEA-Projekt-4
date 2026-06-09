export module AlgorithmResult;

import DefinitelyNotAVector;

export struct AlgorithmResult
{
    int cost;
    DefinitelyNotAVector<int> path;
    double best_found_time_ms = 0.0;
    double average_cost = 0.0;
};
