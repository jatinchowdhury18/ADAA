#include "FFTViewer.h"

namespace
{
    constexpr float fs = 44100.0f;
    constexpr float freq = 2244.5f;

    constexpr int fftOrder = 16;
    constexpr int size = 1 << fftOrder;
    constexpr float fLow = 1000.0f;
    constexpr float fHigh = 22000.0f;
}

FFTViewer::FFTViewer (AudioProcessorValueTreeState& vts) :
    BaseViewer (vts),
    dryBuffer (createSine (freq, fs, size)),
    forwardFFT (fftOrder)
{
    fftBuffer.reset (new float[2*size]);
}

void FFTViewer::processBuffer()
{
    wetBuffer.makeCopyOf (dryBuffer);
    float inputGain = Decibels::decibelsToGain (gainDBParam->load());
    wetBuffer.applyGain (inputGain);
    nlProc.prepare (fs, size);
    nlProc.processBlock (wetBuffer);

    FloatVectorOperations::copy (fftBuffer.get(), wetBuffer.getReadPointer (0), size);
    forwardFFT.performFrequencyOnlyForwardTransform (fftBuffer.get());
}

float FFTViewer::getMagnitudeForX (float x)
{
    int idx = (int) jmap (x / (float) getWidth(), 0.0f, (float) size / 2.0f);
    return fftBuffer[idx];
}

void FFTViewer::updateCurve()
{
    processBuffer();

    curvePath.clear();
    bool started = false;
    for (float xPos = 0.0f; xPos < (float) getWidth(); xPos += 1.0f)
    {
        auto traceMag = Decibels::gainToDecibels (getMagnitudeForX (xPos));
        auto traceY = getHeight() - (2 * traceMag);

        if (! started)
        {
            curvePath.startNewSubPath (xPos, traceY);
            started = true;
        }
        else
        {
            curvePath.lineTo (xPos, traceY);
        }
    }

    MessageManagerLock mml;
    repaint();
}
