#pragma once

#include "BaseTanh.h"

namespace LUT
{
    constexpr float minVal = -10.0f;
    constexpr float maxVal = 10.0f;
}

template<size_t N>
class TanhLUT : public BaseTanh
{
public:
    TanhLUT() = default;

    inline float customTanh (float x) noexcept override
    {
        return tanhLut.processSample (x);
    }

private:
    dsp::LookupTableTransform<float> tanhLut { [] (float x) { return std::tanh (x); },
        LUT::minVal, LUT::maxVal, N };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TanhLUT)
};
