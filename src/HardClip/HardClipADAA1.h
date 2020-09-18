#pragma once

#include "BaseHardClip.h"

namespace HC_ADAA
{
    constexpr float TOL = (float) 1.0e-4;
}

class HardClipADAA1 : public BaseHardClip
{
public:
    HardClipADAA1() = default;

    void prepare (double, int) override
    {
        x1 = 0.0f;
        hcAD1_x1 = 0.0f;
    }

    inline float hardClip (float x) noexcept override
    {
        bool illCondition = std::abs (x - x1) < HC_ADAA::TOL;
        float hcAD1_x = hardClipAD1 (x);

        float y = illCondition ?
            BaseHardClip::hardClip (0.5f * (x + x1)) :  // fallback for ill-conditioned input
            (hcAD1_x - hcAD1_x1) / (x - x1);            // normal mode

        // update state
        hcAD1_x1 = hcAD1_x;
        x1 = x;
        
        return y;
    }

protected:
    float x1 = 0.0f;
    float hcAD1_x1 = 0.0f;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HardClipADAA1)
};
