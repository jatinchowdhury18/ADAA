#include "NLViewer.h"

namespace
{
    constexpr float fs = 44100.0f;
    constexpr float freq = 50.0f;
    constexpr int size = 4096;
}

NLViewer::NLViewer (AudioProcessorValueTreeState& vts) :
    BaseViewer (vts),
    dryBuffer (createSine (freq, fs, size))
{
    nlParam = vts.getRawParameterValue ("nl");
}

void NLViewer::processBuffer()
{
    wetBuffer.makeCopyOf (dryBuffer);
    float inputGain = Decibels::decibelsToGain (gainDBParam->load());
    wetBuffer.applyGain (inputGain);
    nlProc.prepare (fs, size);
    nlProc.processBlock (wetBuffer);

    delayDryBuffer.makeCopyOf (dryBuffer);
    delay.setDelay (nlProc.getLatencySamples());
    delay.prepare ({ fs, (uint32) size, 1 });
    dsp::AudioBlock<float> delayBlock { delayDryBuffer };
    delay.process (dsp::ProcessContextReplacing<float> { delayBlock });

    // different gain range for waveguide
    if (nlParam->load() == 2.0f)
    {
        wetBuffer.applyGain (0.1f);
        delayDryBuffer.applyGain (1.5f);
    }
}

void NLViewer::updateCurve()
{
    processBuffer();

    const auto yFactor = 0.45f;
    const float gain = 1.0f;

    curvePath.clear();
    bool started = false;
    for (int n = size / 2; n <  size; ++n)
    {
        auto x = delayDryBuffer.getSample (0, n);
        auto y = wetBuffer.getSample (0, n);

        auto xDraw = (x +  (gain / 1.0f)) * (float) getWidth() / (gain * 2);
        auto yDraw = (float) getHeight() * (0.5f - yFactor * y);

        if (! started)
        {
            curvePath.startNewSubPath (xDraw, yDraw);
            started = true;
        }
        else
        {
            curvePath.lineTo (xDraw, yDraw);
        }
    }

    MessageManagerLock mml;
    repaint();
}
