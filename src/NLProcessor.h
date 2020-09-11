#pragma once

#include "BaseNL.h"

class NLProcessor
{
public:
    NLProcessor (const AudioProcessorValueTreeState& vts, size_t nChannels = 2);

    void prepare (double sampleRate, int samplesPerBlock);
    void processBlock (AudioBuffer<float>& buffer);

    float getLatencySamples() const noexcept;

private:
    std::atomic<float>* osParam = nullptr;
    std::atomic<float>* nlParam = nullptr;

    std::unique_ptr<BaseNL> nlProcs[6][2];
    std::unique_ptr<dsp::Oversampling<float>> oversample[3];

    const size_t nChannels;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NLProcessor)
};
