#pragma once

#include "HardClipADAA2.h"

template<size_t N>
class HardClipADAA2LUT : public HardClipADAA2
{
public:
    HardClipADAA2LUT() = default;

    inline float calcD1 (float x0) noexcept
    {
        bool illCondition = std::abs (x0 - x1) < HC_ADAA::TOL;
        float hcAD2_x0 = hcAD2Lut (x0);

        float y = illCondition ?
            hcAD1Lut (0.5f * (x0 + x1)) :
            (hcAD2_x0 - hcAD2_x1) / (x0 - x1);

        hcAD2_x1 = hcAD2_x0;
        return y;
    }

    inline float fallback (float x) noexcept
    {
        float xBar = 0.5f * (x + x2);
        float delta = xBar - x;

        bool illCondition = std::abs (delta) < 10000.0f * HC_ADAA::TOL;

        return illCondition ?
            hcLut (0.5f * (xBar + x)) :
            (2.0f / delta) * (hcAD1Lut (xBar) + (hcAD2Lut (x) - hcAD2Lut (xBar)) / delta);
    }

    inline float hardClip (float x) noexcept override
    {
        bool illCondition = std::abs (x - x2) < 10000.0f * ADAA::TOL;
        float d1 = calcD1 (x);

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
    dsp::LookupTableTransform<float> hcLut { [=] (float x) { return BaseHardClip::hardClip (x); },
        LUT::minVal, LUT::maxVal, N };

    dsp::LookupTableTransform<float> hcAD1Lut { [=] (float x) { return hardClipAD1 (x); },
        2 * LUT::minVal, 2 * LUT::maxVal, 4 * N };

    dsp::LookupTableTransform<float> hcAD2Lut { [=] (float x) { return hardClipAD2 (x); },
        4 * LUT::minVal, 4 * LUT::maxVal, 8 * N };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HardClipADAA2LUT)
};
