#pragma once

#include "../BaseNL.h"
#include "Li2.hpp"

class BaseTanh : public BaseNL
{
public:
    BaseTanh() = default;
    virtual ~BaseTanh() {}

    virtual inline float customTanh (float x) noexcept
    {
        return std::tanh (x);
    }

    void processBlock (float* x, const int nSamples) override
    {
        for (int n = 0; n < nSamples; ++n)
            x[n] = customTanh (x[n]);
    }

protected:
    /** First antiderivative of tanh */
    inline float tanhAD1 (float x) const noexcept
    {
        return std::log (std::cosh (x));
    }

    /** Second antiderivative of tanh */
    inline float tanhAD2 (float x) const noexcept
    {
        const auto expVal = std::exp(-2 * x);
        return 0.5f * (x * (x + 2 * std::log (expVal + 1)) - polylogarithm::Li2 (1.0f + expVal));
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BaseTanh)
};
