#include "NLProcessor.h"
#include "HardClip.h"
#include "TanhNL.h"
#include "NLWaveguide.h"
#include "ADAA/ADAA2LUT.h"

template <typename T>
std::vector<std::unique_ptr<BaseNL>> getChannelProcs (int numChannels)
{
    std::vector<std::unique_ptr<BaseNL>> chProcs;
    for (size_t ch = 0; ch < numChannels; ++ch)
        chProcs.push_back (std::make_unique<T>());

    return chProcs;
}

NLProcessor::NLProcessor (const AudioProcessorValueTreeState& vts, size_t nChannels) :
    nChannels (nChannels)
{
    for (int i = 0; i < 3; ++i)
        oversample[i] = std::make_unique<dsp::Oversampling<float>> (nChannels, i + 1, dsp::Oversampling<float>::filterHalfBandPolyphaseIIR);

    NLSet hcProcs;
    hcProcs.push_back (getChannelProcs<HardClip<StandardNL>> (nChannels));
    hcProcs.push_back (getChannelProcs<HardClip<ADAA1>> (nChannels));
    hcProcs.push_back (getChannelProcs<HardClip<ADAA2>> (nChannels));
    hcProcs.push_back (getChannelProcs<HardClip<StandardLUT<(1 << 10)>>> (nChannels));
    hcProcs.push_back (getChannelProcs<HardClip<ADAA1LUT<(1 << 12)>>> (nChannels));
    hcProcs.push_back (getChannelProcs<HardClip<ADAA2LUT<(1 << 18)>>> (nChannels));
    nlProcs.push_back (std::move (hcProcs));

    NLSet tanhProcs;
    tanhProcs.push_back (getChannelProcs<TanhNL<StandardNL>> (nChannels));
    tanhProcs.push_back (getChannelProcs<TanhNL<ADAA1>> (nChannels));
    tanhProcs.push_back (getChannelProcs<TanhNL<ADAA2>> (nChannels));
    tanhProcs.push_back (getChannelProcs<TanhNL<StandardLUT<(1 << 10)>>> (nChannels));
    tanhProcs.push_back (getChannelProcs<TanhNL<ADAA1LUT<(1 << 12)>>> (nChannels));
    tanhProcs.push_back (getChannelProcs<TanhNL<ADAA2LUT<(1 << 18)>>> (nChannels));
    nlProcs.push_back (std::move (tanhProcs));

    NLSet statefulProcs;
    statefulProcs.push_back (getChannelProcs<NLWaveguide<StandardNL>> (nChannels));
    statefulProcs.push_back (getChannelProcs<NLWaveguide<ADAA1, 1>> (nChannels));
    statefulProcs.push_back (getChannelProcs<NLWaveguide<ADAA2, 2>> (nChannels));
    statefulProcs.push_back (getChannelProcs<NLWaveguide<StandardLUT<(1 << 10)>>> (nChannels));
    statefulProcs.push_back (getChannelProcs<NLWaveguide<ADAA1LUT<(1 << 12)>, 1>> (nChannels));
    statefulProcs.push_back (getChannelProcs<NLWaveguide<ADAA2LUT<(1 << 18)>, 2>> (nChannels));
    nlProcs.push_back (std::move (statefulProcs));

    osParam = vts.getRawParameterValue ("os");
    nlParam = vts.getRawParameterValue ("nl");
    adaaParam = vts.getRawParameterValue ("adaa");
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
    mySampleRate = sampleRate;
    mySamplesPerBlock = samplesPerBlock;
    prevOS = (int) osParam->load();

    for (auto& os: oversample)
        os->initProcessing (samplesPerBlock);

    for (auto& nlSet : nlProcs)
    {
        for (auto& nl : nlSet)
        {
            for (size_t ch = 0; ch < nChannels; ++ch)
                nl[ch]->prepare (sampleRate * oversample[prevOS]->getOversamplingFactor(), samplesPerBlock);
        }
    }
}

void NLProcessor::processBlock (AudioBuffer<float>& buffer)
{
    auto curOS = (int) osParam->load();
    auto curOversample = oversample[curOS].get();

    if (curOS != prevOS)
    {
        for (auto& nlSet : nlProcs)
        {
            for (auto& nl : nlSet)
            {
                for (size_t ch = 0; ch < nChannels; ++ch)
                    nl[ch]->prepare (mySampleRate * oversample[curOS]->getOversamplingFactor(), mySamplesPerBlock);
            }
        }

        prevOS = curOS;
    }

    dsp::AudioBlock<float> block { buffer };
    auto osBlock = curOversample->processSamplesUp (block);

    auto& curNLSet = nlProcs[(int) nlParam->load()];
    auto& curNLProc = curNLSet[(int) adaaParam->load()];
    for (size_t ch = 0; ch < osBlock.getNumChannels(); ++ch)
        curNLProc[ch]->processBlock (osBlock.getChannelPointer (ch), (int) osBlock.getNumSamples());

    curOversample->processSamplesDown (block);
}
