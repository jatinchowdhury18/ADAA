#pragma once

#include "TanhADAA2.h"
#include "TanhLUT.h"

template<size_t N>
class TanhADAA2LUT : public TanhADAA2
{
public:
    TanhADAA2LUT() = default;

    inline float calcD1 (float x0, float _x1) noexcept
    {
        bool illCondition = std::abs (x0 - _x1) < ADAA::TOL;
        float tanhAD2_x0 = tanhAD2Lut (x0);

        float y = illCondition ?
            tanhAD1Lut (0.5f * (x0 + _x1)) :
            (tanhAD2_x0 - tanhAD2_x1) / (x0 - _x1);

        tanhAD2_x1 = tanhAD2_x0;
        return y;
    }

    inline float fallback (float x) noexcept
    {
        float xBar = 0.5f * (x + x2);
        float delta = xBar - x;

        bool illCondition = std::abs (delta) < 10000 * ADAA::TOL;

        return illCondition ?
            tanhLut (0.5f * (xBar + x)) :
            (2.0f / delta) * (tanhAD1Lut (xBar) + (tanhAD2Lut (x) - tanhAD2Lut (xBar)) / delta);
    }

    inline float customTanh (float x) noexcept override
    {
        bool illCondition = std::abs (x - x2) < 10000 * ADAA::TOL;
        float d1 = calcD1 (x, x1);

        float y = illCondition ?
            fallback (x) :
            (2.0f / (x - x2)) * (d1 - d2);

        // update state
        d2 = d1;
        x2 = x1;
        x1 = x;
        
        return y;
    }

private:
    dsp::LookupTableTransform<float> tanhLut { [] (float x) { return std::tanh (x); },
        LUT::minVal, LUT::maxVal, N };

    dsp::LookupTableTransform<float> tanhAD1Lut { [=] (float x) { return tanhAD1 (x); },
        2 * LUT::minVal, 2 * LUT::maxVal, 4 * N };

    dsp::LookupTableTransform<float> tanhAD2Lut { [=] (float x) { return tanhAD2 (x); },
        4 * LUT::minVal, 4 * LUT::maxVal, 8 * N };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TanhADAA2LUT)
};
