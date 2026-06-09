module;

#include <random>

export module RandomDataGenerator;

import IDataProvider;
import TestData;
import DefinitelyNotAVector;

export class RandomDataGenerator : public IDataProvider
{
    int size_;
    int min_cost_;
    int max_cost_;

public:
    explicit RandomDataGenerator(int size, int min_cost = 1, int max_cost = 100)
        : size_(size), min_cost_(min_cost), max_cost_(max_cost) {}

    TestData GetRequiredData() const override
    {
        TestData data;
        data.size = size_;
        
        std::random_device rd;  
        std::mt19937 gen(rd()); 
        std::uniform_int_distribution<int> dist(min_cost_, max_cost_); 
        
        data.matrix = DefinitelyNotAVector<DefinitelyNotAVector<int>>(size_);

        for (int i = 0; i < size_; ++i)
        {
            data.matrix[i] = DefinitelyNotAVector<int>(size_);
            
            for (int j = 0; j < size_; ++j)
            {
                if (i == j)
                {
                    data.matrix[i][j] = -1;
                }
                else
                {
                    data.matrix[i][j] = dist(gen);
                }
            }
        }

        return data; 
    }
};
