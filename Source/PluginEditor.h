#pragma once
#include "PluginProcessor.h"
#include "GUI/JunoLookAndFeel.h"
#include "GUI/PresetBar.h"

class JunoCloneAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit JunoCloneAudioProcessorEditor (JunoCloneAudioProcessor&);
    ~JunoCloneAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    JunoCloneAudioProcessor& proc;
    JunoLookAndFeel lnf;
    PresetBar presetBar { proc.presetManager };

    struct Control
    {
        std::unique_ptr<juce::Slider> slider;
        std::unique_ptr<juce::ComboBox> combo;
        juce::Label label;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sliderAttachment;
        std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> comboAttachment;
        juce::Component* getComponent() const { return slider ? (juce::Component*) slider.get() : (juce::Component*) combo.get(); }
    };

    struct Section
    {
        juce::String title;
        juce::GroupComponent group;
        std::vector<Control*> controls;
    };

    std::vector<std::unique_ptr<Control>> allControls;
    std::vector<std::unique_ptr<Section>> sections;

    Section& addSection (const juce::String& title);
    Control& addKnob  (Section& sec, const juce::String& paramId, const juce::String& text);
    Control& addFader  (Section& sec, const juce::String& paramId, const juce::String& text);
    Control& addCombo  (Section& sec, const juce::String& paramId, const juce::String& text);
};
