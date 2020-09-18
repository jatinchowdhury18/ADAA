#include "NLProcessor.h"
#include "Tanh/TanhHeader.h"
#include "HardClip/HardClipHeader.h"

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
    hcProcs.push_back (getChannelProcs<BaseHardClip> (nChannels));
    hcProcs.push_back (getChannelProcs<HardClipADAA1> (nChannels));
    hcProcs.push_back (getChannelProcs<HardClipADAA2> (nChannels));
    hcProcs.push_back (getChannelProcs<HardClipLUT<1024>> (nChannels));
    hcProcs.push_back (getChannelProcs<HardClipADAA1LUT<32768>> (nChannels));
    hcProcs.push_back (getChannelProcs<HardClipADAA2LUT<32768>> (nChannels));
    nlProcs.push_back (std::move (hcProcs));

    NLSet tanhProcs;
    tanhProcs.push_back (getChannelProcs<BaseTanh> (nChannels));
    tanhProcs.push_back (getChannelProcs<TanhADAA1> (nChannels));
    tanhProcs.push_back (getChannelProcs<TanhADAA2> (nChannels));
    tanhProcs.push_back (getChannelProcs<TanhLUT<1024>> (nChannels));
    tanhProcs.push_back (getChannelProcs<TanhADAA1LUT<32768>> (nChannels));
    tanhProcs.push_back (getChannelProcs<TanhADAA2LUT<32768>> (nChannels));
    nlProcs.push_back (std::move (tanhProcs));

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
    for (auto& os: oversample)
        os->initProcessing (samplesPerBlock);

    for (auto& nlSet : nlProcs)
    {
        for (auto& nl : nlSet)
        {
            for (size_t ch = 0; ch < nChannels; ++ch)
                nl[ch]->prepare (sampleRate,samplesPerBlock);
        }
    }
}

void NLProcessor::processBlock (AudioBuffer<float>& buffer)
{
    auto curOversample = oversample[(int) osParam->load()].get();

    dsp::AudioBlock<float> block { buffer };
    auto osBlock = curOversample->processSamplesUp (block);

    auto& curNLSet = nlProcs[(int) nlParam->load()];
    auto& curNLProc = curNLSet[(int) adaaParam->load()];
    for (size_t ch = 0; ch < osBlock.getNumChannels(); ++ch)
        curNLProc[ch]->processBlock (osBlock.getChannelPointer (ch), (int) osBlock.getNumSamples());

    curOversample->processSamplesDown (block);
}
