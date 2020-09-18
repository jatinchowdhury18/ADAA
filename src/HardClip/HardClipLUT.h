#pragma once

#include "BaseHardClip.h"

namespace HC_LUT
{
    constexpr float minVal = -10.0f;
    constexpr float maxVal = 10.0f;
}

template<size_t N>
class HardClipLUT : public BaseHardClip
{
public:
    HardClipLUT() = default;

    inline float hardClip (float x) noexcept override
    {
        return hardClipLUT.processSample (x);
    }

private:
    dsp::LookupTableTransform<float> hardClipLUT { [=] (float x) { return BaseHardClip::hardClip (x); },
        HC_LUT::minVal, HC_LUT::maxVal, N };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HardClipLUT)
};
