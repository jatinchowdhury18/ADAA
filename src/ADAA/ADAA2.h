#pragma once

#include "ADAA1.h"

class ADAA2 : public ADAA1
{
public:
    ADAA2() = default;
    virtual ~ADAA2() {}

    void prepare() override
    {
        ADAA1::prepare();
        x2 = 0.0;
        ad2_x1 = 0.0;
        d2 = 0.0;
    }

    inline double process (double x) noexcept override
    {
        bool illCondition = std::abs (x - x2) < ADAAConst::TOL;
        double d1 = calcD1 (x);

        double y = illCondition ?
            fallback (x) :
            (2.0 / (x - x2)) * (d1 - d2);

        // update state
        d2 = d1;
        x2 = x1;
        x1 = x;
        
        return y;
    }

protected:
    virtual inline double nlFunc_AD2 (double x) const noexcept { return func_AD2 (x); }

    double x2 = 0.0;
    double ad2_x1 = 0.0;
    double d2 = 0.0;

private:
    inline double calcD1 (double x0) noexcept
    {
        bool illCondition = std::abs (x0 - x1) < ADAAConst::TOL;
        double ad2_x0 = nlFunc_AD2 (x0);

        double y = illCondition ?
            nlFunc_AD1 (0.5 * (x0 + x1)) :
            (ad2_x0 - ad2_x1) / (x0 - x1);

        ad2_x1 = ad2_x0;
        return y;
    }

    inline double fallback (double x) noexcept
    {
        double xBar = 0.5 * (x + x2);
        double delta = xBar - x;

        bool illCondition = std::abs (delta) < ADAAConst::TOL;

        return illCondition ?
            nlFunc (0.5 * (xBar + x)) :
            (2.0 / delta) * (nlFunc_AD1 (xBar) + (nlFunc_AD2 (x) - nlFunc_AD2 (xBar)) / delta);
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ADAA2)
};
