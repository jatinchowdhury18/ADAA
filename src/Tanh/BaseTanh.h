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

    template<typename T>
    inline T dilog_approx (T x) const noexcept
    {
        auto powVal = std::pow (x, (T) 1.5);
        powVal = std::abs (powVal) < (T) 1.0e-15 ? (T) 1.0e-15 : powVal;
        return (T) -1 * std::log (powVal);
    }

    /** Second antiderivative of tanh */
    inline float tanhAD2 (float x) const noexcept
    {
        const auto expVal = std::exp (-2 * x);
        return 0.5f * (x * (x + 2 * std::log (expVal + 1)) - (float) polylogarithm::Li2 (1.0 + (double) expVal));
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BaseTanh)
};
