#pragma once

#include "Tanh/BaseTanh.h"

template<class Base, size_t Num = 1, size_t Den = 1>
class StatefulNL : public Base
{
public:
    StatefulNL() = default;
    ~StatefulNL() {}

    void prepare (double sampleRate, int samplesPerBlock) override
    {
        // delay length is 200 samples at 44.1 kHz
        delaySamp = 200.0f * (sampleRate / 44100.0f);
        alpha = calcAlpha (Num * (float) sampleRate / Den);
        delay.prepare ({ sampleRate, (uint32) samplesPerBlock, 1 });
        delay.setDelay (delaySamp);
    }

    void processBlock (float* x, const int nSamples) override
    {
        constexpr float dd = 2.5f;

        for (int n = 0; n < nSamples; ++n)
        {
            x[n] -= delay.popSample (0);
            delay.pushSample (0, alpha * Base::customTanh (dd * x[n]) / dd);
        }
    }

protected:
    float calcAlpha (float fs)
    {
        return std::pow (0.001f, delaySamp / (t60 * fs));
    }

    const float t60 = 0.9f;
    float alpha = 0.0f;
    float z1 = 0.0f;

    float delaySamp = 1.0f;
    dsp::DelayLine<float, dsp::DelayLineInterpolationTypes::None> delay { 1 << 20 };

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StatefulNL)
};
