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
    std::atomic<float>* adaaParam = nullptr;
    std::atomic<float>* nlParam = nullptr;

    using NLSet = std::vector<std::vector<std::unique_ptr<BaseNL>>>;
    std::vector<NLSet> nlProcs;

    std::unique_ptr<dsp::Oversampling<float>> oversample[3];

    const size_t nChannels;

    double mySampleRate = 44100.0;
    int mySamplesPerBlock = 128;
    int prevOS = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NLProcessor)
};
