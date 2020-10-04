#pragma once

#include "TanhNL.h"

template<class Type, int ADAAOrder = 0>
class NLWaveguide : public TanhNL<Type>
{
public:
    NLWaveguide() = default;

    void prepare (double sampleRate, int samplesPerBlock) override
    {
        TanhNL<Type>::prepare (sampleRate, samplesPerBlock);

        float delaySamp = 200.0f * (sampleRate / 44100.0f); // delay length is 200 samples at 44.1 kHz
        delaySamp -= getADAADelaySamp(); // correct delay line length for ADAA
        alpha = calcAlpha (delaySamp / sampleRate);

        delay.prepare ({ sampleRate, (uint32) samplesPerBlock, 1 });
        delay.setDelay (delaySamp);
    }

    void processBlock (float* x, const int nSamples) override
    {
        constexpr float dd = 2.5f;
        for (int n = 0; n < nSamples; ++n)
        {
            x[n] -= delay.popSample (0);
            delay.pushSample (0, alpha * (float) TanhNL<Type>::process ((double) (dd * x[n])) / dd);
        }
    }

private:
    float calcAlpha (float delayTime) const noexcept
    {
        constexpr float t60 = 0.9f;
        return std::pow (0.001f, delayTime / t60);
    }

    constexpr float getADAADelaySamp() { return 0.5f * (float) ADAAOrder; }

    float alpha = 0.0f;
    dsp::DelayLine<float, dsp::DelayLineInterpolationTypes::Linear> delay { 1 << 20 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NLWaveguide)
};
