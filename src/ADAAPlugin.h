#pragma once

#include <JuceHeader.h>
#include "NLProcessor.h"

class ADAAPlugin : public chowdsp::PluginBase<ADAAPlugin>
{
public:
    ADAAPlugin();

    static void addParameters (Parameters& params);
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock (AudioBuffer<float>& buffer) override;

    AudioProcessorEditor* createEditor() override;

    AudioProcessorValueTreeState& getVTS() { return vts; }
    AudioProcessLoadMeasurer& getLoadMeasurer() { return loadMeasurer; }

private:
    std::atomic<float>* gainDBParam = nullptr;
    chowdsp::GainProcessor gainProc;

    NLProcessor nlProc { vts, 2 };

    AudioProcessLoadMeasurer loadMeasurer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ADAAPlugin)
};
