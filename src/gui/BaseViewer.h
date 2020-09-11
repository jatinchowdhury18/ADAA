#pragma once

#include "../NLProcessor.h"

class BaseViewer : public Component,
                   private AudioProcessorValueTreeState::Listener
{
public:
    BaseViewer (AudioProcessorValueTreeState& vts);
    virtual ~BaseViewer();

    enum ColourIDs
    {
        backgroundColourID,
        pathColourID,
    };

    void parameterChanged (const String& paramID, float value) override;
    virtual void processBuffer() {}
    virtual void updateCurve() {}
    
    void paint (Graphics& g) override;
    void resized() override;

protected:
    static AudioBuffer<float> createSine (float freq, float fs, int lengthSamples);

    std::atomic<float>* gainDBParam = nullptr;
    NLProcessor nlProc;

    Path curvePath;

private:
    AudioProcessorValueTreeState& vts;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BaseViewer)
};

template<typename Plugin, typename Viewer>
class ViewerItem : public foleys::GuiItem
{
public:
    FOLEYS_DECLARE_GUI_FACTORY (ViewerItem)

    ViewerItem (foleys::MagicGUIBuilder& builder, const ValueTree& node) :
        foleys::GuiItem (builder, node)
    {
        setColourTranslation ({
            {"background", BaseViewer::backgroundColourID},
            {"path", BaseViewer::pathColourID},
        });

        auto plugin = dynamic_cast<Plugin*> (builder.getMagicState().getProcessor());
        viewer = std::make_unique<Viewer> (plugin->getVTS());
        addAndMakeVisible (viewer.get());
    }

    void update() override
    {
        viewer->updateCurve();
    }

    Component* getWrappedComponent() override
    {
        return viewer.get();
    }

private:
    std::unique_ptr<Viewer> viewer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ViewerItem)
};
