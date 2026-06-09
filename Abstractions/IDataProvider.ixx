export module IDataProvider;

import TestData;

export class IDataProvider
{
public:
    virtual ~IDataProvider() = default;

    virtual TestData GetRequiredData() const = 0;
};
