#pragma once

#include "TanhADAA1.h"

class TanhADAA2 : public BaseTanh
{
public:
    TanhADAA2() = default;

    void prepare (double, int) override
    {
        x1 = 0.0f;
        x2 = 0.0f;
        tanhAD2_x1 = 0.0f;
        d2 = 0.0f;
    }

    inline float calcD1 (float x0, float _x1) noexcept
    {
        bool illCondition = std::abs (x0 - _x1) < ADAA::TOL;
        float tanhAD2_x0 = tanhAD2 (x0);

        float y = illCondition ?
            tanhAD1 (0.5f * (x0 + _x1)) :
            (tanhAD2_x0 - tanhAD2_x1) / (x0 - _x1);

        tanhAD2_x1 = tanhAD2_x0;
        return y;
    }

    inline float fallback (float x) noexcept
    {
        float xBar = 0.5f * (x + x2);
        float delta = xBar - x;

        bool illCondition = std::abs (delta) < 6000 * ADAA::TOL;

        return illCondition ?
            std::tanh (0.5f * (xBar + x)) :
            (2.0f / delta) * (tanhAD1 (xBar) + (tanhAD2 (x) - tanhAD2 (xBar)) / delta);
    }

    inline float customTanh (float x) noexcept override
    {
        bool illCondition = std::abs (x - x2) < 6000 * ADAA::TOL;
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

protected:
    float x1 = 0.0f, x2 = 0.0f;
    float tanhAD2_x1 = 0.0f;
    float d2 = 0.0f;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TanhADAA2)
};
