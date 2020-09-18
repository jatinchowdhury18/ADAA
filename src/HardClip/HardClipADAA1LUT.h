#pragma once

#include "HardClipADAA1.h"
#include "HardClipLUT.h"

template<size_t N>
class HardClipADAA1LUT : public HardClipADAA1
{
public:
    HardClipADAA1LUT() = default;

    inline float hardClip (float x) noexcept override
    {
        bool illCondition = std::abs (x - x1) < ADAA::TOL;
        float hcAD1_x = hardClipAD1Lut (x);

        float y = illCondition ?
            hardClipLut (0.5f * (x + x1)) :     // fallback for ill-conditioned input
            (hcAD1_x - hcAD1_x1) / (x - x1);    // normal mode

        // update state
        hcAD1_x1 = hcAD1_x;
        x1 = x;
        
        return y;
    }

private:
    dsp::LookupTableTransform<float> hardClipLut { [=] (float x) { return BaseHardClip::hardClip (x); },
        LUT::minVal, LUT::maxVal, N };

    dsp::LookupTableTransform<float> hardClipAD1Lut { [=] (float x) { return hardClipAD1 (x); },
        2 * LUT::minVal, 2 * LUT::maxVal, N };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HardClipADAA1LUT)
};
