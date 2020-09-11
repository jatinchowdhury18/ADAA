#pragma once

#include "BaseViewer.h"

class FFTViewer : public BaseViewer
{
public:
    FFTViewer (AudioProcessorValueTreeState& vts);

    void processBuffer() override;
    void updateCurve() override;

private:
    const AudioBuffer<float> dryBuffer;
    AudioBuffer<float> wetBuffer;

    float getMagnitudeForX (float x);
    dsp::FFT forwardFFT;

    std::unique_ptr<float[]> fftBuffer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FFTViewer)
};
