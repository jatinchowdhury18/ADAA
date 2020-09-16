#include "NLProcessor.h"
#include "Tanh/BaseTanh.h"
#include "Tanh/TanhADAA1.h"
#include "Tanh/TanhADAA2.h"
#include "Tanh/TanhLUT.h"
#include "Tanh/TanhADAA1LUT.h"
#include "Tanh/TanhADAA2LUT.h"

NLProcessor::NLProcessor (const AudioProcessorValueTreeState& vts, size_t nChannels) :
    nChannels (nChannels)
{
    for (int i = 0; i < 3; ++i)
        oversample[i] = std::make_unique<dsp::Oversampling<float>> (nChannels, i + 1, dsp::Oversampling<float>::filterHalfBandPolyphaseIIR);

    for (size_t ch = 0; ch < nChannels; ++ch)
    {
        nlProcs[0][ch] = std::make_unique<BaseTanh>();
        nlProcs[1][ch] = std::make_unique<TanhADAA1>();
        nlProcs[2][ch] = std::make_unique<TanhADAA2>();
        nlProcs[3][ch] = std::make_unique<TanhLUT<1024>>();
        nlProcs[4][ch] = std::make_unique<TanhADAA1LUT<32768>>();
        nlProcs[5][ch] = std::make_unique<TanhADAA2LUT<1024>>();
    }

    osParam = vts.getRawParameterValue ("os");
    nlParam = vts.getRawParameterValue ("nl");
}

float NLProcessor::getLatencySamples() const noexcept
{
    auto latency = oversample[(int) osParam->load()]->getLatencyInSamples();

    const auto nlChoice = (int) nlParam->load();
    if (nlChoice != 0 && nlChoice != 3) // ADAA mode
        latency += 0.5f;

    return latency;
}

void NLProcessor::prepare (double sampleRate, int samplesPerBlock)
{
    for (auto& os: oversample)
        os->initProcessing (samplesPerBlock);

    for (auto& nl : nlProcs)
    {
        for (size_t ch = 0; ch < nChannels; ++ch)
            nl[ch]->prepare (sampleRate,samplesPerBlock);
    }
}

void NLProcessor::processBlock (AudioBuffer<float>& buffer)
{
    auto curOversample = oversample[(int) osParam->load()].get();

    dsp::AudioBlock<float> block { buffer };
    auto osBlock = curOversample->processSamplesUp (block);

    auto curNLProc = nlProcs[(int) nlParam->load()];
    for (size_t ch = 0; ch < osBlock.getNumChannels(); ++ch)
        curNLProc[ch]->processBlock (osBlock.getChannelPointer (ch), (int) osBlock.getNumSamples());

    curOversample->processSamplesDown (block);
}
