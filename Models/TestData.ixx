export module TestData;

import DefinitelyNotAVector;

export struct TestData
{
    int size;
    DefinitelyNotAVector<DefinitelyNotAVector<int>> matrix;
};
