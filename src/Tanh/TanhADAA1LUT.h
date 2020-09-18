#pragma once

#include "TanhADAA1.h"
#include "TanhLUT.h"

template<size_t N>
class TanhADAA1LUT : public TanhADAA1
{
public:
    TanhADAA1LUT() = default;

    inline float customTanh (float x) noexcept override
    {
        bool illCondition = std::abs (x - x1) < ADAA::TOL;
        float tanhAD1_x = tanhAD1Lut (x);

        float y = illCondition ?
            tanhLut (0.5f * (x + x1)) :             // fallback for ill-conditioned input
            (tanhAD1_x - tanhAD1_x1) / (x - x1);    // normal mode

        // update state
        tanhAD1_x1 = tanhAD1_x;
        x1 = x;
        
        return y;
    }

private:
    dsp::LookupTableTransform<float> tanhLut { [] (float x) { return std::tanh (x); },
        LUT::minVal, LUT::maxVal, N };

    dsp::LookupTableTransform<float> tanhAD1Lut { [=] (float x) { return tanhAD1 (x); },
        2 * LUT::minVal, 2 * LUT::maxVal, N };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TanhADAA1LUT)
};
