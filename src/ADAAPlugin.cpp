#include "ADAAPlugin.h"
#include "gui/NLViewer.h"
#include "gui/FFTViewer.h"
#include "gui/CPUMeter.h"

ADAAPlugin::ADAAPlugin()
{
    gainDBParam = vts.getRawParameterValue ("gain_db");
}

void ADAAPlugin::addParameters (Parameters& params)
{
    StringArray osChoices { "2x", "4x", "8x" };
    StringArray nlChoices { "Tanh", "Tanh ADAA1", "Tanh ADAA2", "Tanh LUT", "Tanh ADAA1 LUT", "Tanh ADAA2 LUT" };

    params.push_back (std::make_unique<AudioParameterChoice> ("os", "Oversampling", osChoices, 0));
    params.push_back (std::make_unique<AudioParameterChoice> ("nl", "Nonlinearity", nlChoices, 0));
    params.push_back (std::make_unique<AudioParameterFloat> ("gain_db", "Gain [dB]", -12.0f, 24.0f, 0.0f));
}

void ADAAPlugin::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    gainProc.reset();
    nlProc.prepare (sampleRate, samplesPerBlock);

    loadMeasurer.reset (sampleRate, samplesPerBlock);
}

void ADAAPlugin::releaseResources()
{
}

void ADAAPlugin::processBlock (AudioBuffer<float>& buffer)
{
    AudioProcessLoadMeasurer::ScopedTimer loadTimer (loadMeasurer);

    gainProc.setGain (Decibels::decibelsToGain (gainDBParam->load()));
    gainProc.processBlock (buffer);

    nlProc.processBlock (buffer);
}

AudioProcessorEditor* ADAAPlugin::createEditor()
{
    auto builder = chowdsp::createGUIBuilder (magicState);
    builder->registerFactory ("NLViewer", &ViewerItem<ADAAPlugin, NLViewer>::factory);
    builder->registerFactory ("FFTViewer", &ViewerItem<ADAAPlugin, FFTViewer>::factory);
    builder->registerFactory ("CPUMeter", &CPUMeterItem<ADAAPlugin>::factory);

    return new foleys::MagicPluginEditor (magicState, BinaryData::tanh_adaa_gui_xml, BinaryData::tanh_adaa_gui_xmlSize, std::move (builder));
}

// This creates new instances of the plugin
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ADAAPlugin();
}
