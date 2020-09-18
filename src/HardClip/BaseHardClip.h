#pragma once

#include "../BaseNL.h"

class BaseHardClip : public BaseNL
{
public:
    BaseHardClip() = default;
    virtual ~BaseHardClip() {}

    virtual inline float hardClip (float x) noexcept
    {
        return jlimit (-1.0f, 1.0f, x);
    }

    void processBlock (float* x, const int nSamples) override
    {
        for (int n = 0; n < nSamples; ++n)
            x[n] = hardClip (x[n]);
    }

protected:
    /** First antiderivative of hard clipper */
    inline float hardClipAD1 (float x) const noexcept
    {
        bool inRange = std::abs(x) <= 1.0f;

        return inRange ?
            x * x / 2.0f :
            x * signum (x) - 0.5f;
    }

    /** Second antiderivative of hard clipper */
    inline float hardClipAD2 (float x) const noexcept
    {
        bool inRange = std::abs(x) <= 1.0f;

        return inRange ?
            x * x * x / 3.0f :
            (x * x / 2.0f - (1.0f / 6.0f)) * signum (x);
    }

private:
    /** Signum function to determine the sign of the input. */
    template <typename T> inline int signum (T val) const noexcept
    {
        return (T (0) < val) - (val < T (0));
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BaseHardClip)
};
