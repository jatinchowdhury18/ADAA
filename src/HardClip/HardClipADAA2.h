#pragma once

#include "HardClipADAA1.h"

class HardClipADAA2 : public BaseHardClip
{
public:
    HardClipADAA2() = default;

    void prepare (double, int) override
    {
        x1 = 0.0f;
        x2 = 0.0f;
        hcAD2_x1 = 0.0f;
        d2 = 0.0f;
    }

    inline float calcD1 (float x0) noexcept
    {
        bool illCondition = std::abs (x0 - x1) < HC_ADAA::TOL;
        float hcAD2_x0 = hardClipAD2 (x0);

        float y = illCondition ?
            hardClipAD1 (0.5f * (x0 + x1)) :
            (hcAD2_x0 - hcAD2_x1) / (x0 - x1);

        hcAD2_x1 = hcAD2_x0;
        return y;
    }

    inline float fallback (float x) noexcept
    {
        float xBar = 0.5f * (x + x2);
        float delta = xBar - x;

        bool illCondition = std::abs (delta) < 6000.0f * HC_ADAA::TOL;

        return illCondition ?
            BaseHardClip::hardClip (0.5f * (xBar + x)) :
            (2.0f / delta) * (hardClipAD1 (xBar) + (hardClipAD2 (x) - hardClipAD2 (xBar)) / delta);
    }

    inline float hardClip (float x) noexcept override
    {
        bool illCondition = std::abs (x - x2) < 6000.0f * ADAA::TOL;
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

protected:
    float x1 = 0.0f, x2 = 0.0f;
    float hcAD2_x1 = 0.0f;
    float d2 = 0.0f;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HardClipADAA2)
};
