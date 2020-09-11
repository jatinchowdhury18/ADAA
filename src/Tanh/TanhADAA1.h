#pragma once

#include "BaseTanh.h"

namespace ADAA
{
    constexpr float TOL = (float) 1.0e-6;
}

class TanhADAA1 : public BaseTanh
{
public:
    TanhADAA1() = default;

    void prepare (double, int) override
    {
        x1 = 0.0f;
        tanhAD1_x1 = 0.0f;
    }

    inline float customTanh (float x) noexcept override
    {
        bool illCondition = std::abs (x - x1) < ADAA::TOL;
        float tanhAD1_x = tanhAD1 (x);

        float y = illCondition ?
            std::tanh (0.5f * (x +x1)) :          // fallback for ill-conditioned input
            (tanhAD1_x - tanhAD1_x1) / (x - x1);  // normal mode

        // update state
        tanhAD1_x1 = tanhAD1_x;
        x1 = x;
        
        return y;
    }

protected:
    float x1 = 0.0f;
    float tanhAD1_x1 = 0.0f;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TanhADAA1)
};
