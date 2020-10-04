#pragma once

#include <JuceHeader.h>

class StandardNL
{
public:
    StandardNL() = default;
    virtual ~StandardNL() {}
    
    virtual void initialise() {}
    virtual void prepare() {}
    virtual inline double process (double x) noexcept
    {
        return func (x);
    }

    virtual double func (double) const noexcept = 0;
    virtual double func_AD1 (double) const noexcept = 0;
    virtual double func_AD2 (double) const noexcept = 0;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StandardNL)
};
