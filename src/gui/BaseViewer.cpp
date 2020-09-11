#include "BaseViewer.h"

BaseViewer::BaseViewer (AudioProcessorValueTreeState& vts) :
    vts (vts),
    nlProc (vts, 1)
{
    gainDBParam = vts.getRawParameterValue ("gain_db");
    vts.addParameterListener ("gain_db", this);
    vts.addParameterListener ("os", this);
    vts.addParameterListener ("nl", this);

    setColour (backgroundColourID, Colours::white);
    setColour (pathColourID, Colours::blue);
}

BaseViewer::~BaseViewer()
{
    vts.removeParameterListener ("gain_db", this);
    vts.removeParameterListener ("os", this);
    vts.removeParameterListener ("nl", this);
}

void BaseViewer::parameterChanged (const String& paramID, float value)
{ 
    MessageManager::callAsync ([=] { updateCurve(); });
}

void BaseViewer::paint (Graphics& g)
{
    g.fillAll (findColour (backgroundColourID));

    g.setColour (findColour (pathColourID));
    g.strokePath (curvePath, PathStrokeType (2.0f, PathStrokeType::JointStyle::curved));
}

void BaseViewer::resized()
{ 
    updateCurve();
}

AudioBuffer<float> BaseViewer::createSine (float freq, float fs, int lengthSamples)
{
    AudioBuffer<float> sineBuffer (1, lengthSamples);

    const float phi = MathConstants<float>::twoPi * freq / fs;
    for (int n = 0; n < lengthSamples; ++n)
        sineBuffer.setSample (0, n, std::sin (phi  * (float) n));

    return sineBuffer;
}
