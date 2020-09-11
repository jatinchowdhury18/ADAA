#pragma once

#include <JuceHeader.h>

class BaseNL
{
public:
    BaseNL() = default;
    virtual ~BaseNL() {}

    virtual void prepare (double, int) {};
    virtual void processBlock (float*, const int) {};

private:

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BaseNL)
};
