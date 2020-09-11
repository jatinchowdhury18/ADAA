#pragma once

#include "BaseViewer.h"

class NLViewer : public BaseViewer
{
public:
    NLViewer (AudioProcessorValueTreeState& vts);

    void processBuffer() override;
    void updateCurve() override;

private:
    const AudioBuffer<float> dryBuffer;
    AudioBuffer<float> wetBuffer;
    AudioBuffer<float> delayDryBuffer;
    dsp::DelayLine<float, dsp::DelayLineInterpolationTypes::Lagrange3rd> delay { 512 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NLViewer)
};
