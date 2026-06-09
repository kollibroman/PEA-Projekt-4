export module IAtspAlgorithm;

import TestData;
import AlgorithmResult;

export class IAtspAlgorithm {
public:
    virtual ~IAtspAlgorithm() = default;

    virtual AlgorithmResult SolveProblem(const TestData& data) const = 0;
};
