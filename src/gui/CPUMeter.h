#pragma once

#include <JuceHeader.h>

template<typename Plugin>
class CPUMeterItem : public foleys::GuiItem,
                     private Timer
{
public:
    FOLEYS_DECLARE_GUI_FACTORY (CPUMeterItem)

    CPUMeterItem (foleys::MagicGUIBuilder& builder, const ValueTree& node) :
        foleys::GuiItem (builder, node),
        loadMeasurer (dynamic_cast<Plugin*> (builder.getMagicState().getProcessor())->getLoadMeasurer())
    {
        setColourTranslation ({
            {"background", ProgressBar::backgroundColourId},
            {"foreground", ProgressBar::foregroundColourId},
        });

        progress = std::make_unique<ProgressBar> (loadProportion);
        addAndMakeVisible (progress.get());

        constexpr double sampleRate = 20.0;
        startTimerHz ((int) sampleRate);

        constexpr double smoothTime = 5.0; // seconds
        smoother.prepare ({ sampleRate, 128, 1 });
        smoother.setAttackTime (smoothTime * 1000.0);
        smoother.setReleaseTime (smoothTime * 1000.0);
        smoother.setLevelCalculationType (dsp::BallisticsFilterLevelCalculationType::RMS);
    }

    void update() override {}

    void timerCallback() override
    {
        loadProportion = smoother.processSample (0, loadMeasurer.getLoadAsProportion());
    }

    Component* getWrappedComponent() override
    {
        return progress.get();
    }

private:
    std::unique_ptr<ProgressBar> progress;
    double loadProportion = 0.0;
    dsp::BallisticsFilter<double> smoother;

    AudioProcessLoadMeasurer& loadMeasurer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CPUMeterItem)
};
