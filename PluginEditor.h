#pragma once
#include "PluginProcessor.h"
#include "GUI/JunoLookAndFeel.h"
#include "GUI/PresetBar.h"
#include "GUI/SavePresetOverlay.h"
#include "GUI/VoiceLedBar.h"

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
    VoiceLedBar voiceLedBar { proc };
    SavePresetOverlay saveOverlay;

    struct Control
    {
        std::unique_ptr<juce::Slider> slider;
        std::unique_ptr<juce::ComboBox> combo;
        std::unique_ptr<juce::TextButton> toggle;
        juce::Label label;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sliderAttachment;
        std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> comboAttachment;
        std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> buttonAttachment;

        juce::Component* getComponent() const
        {
            if (slider) return slider.get();
            if (combo)  return combo.get();
            return toggle.get();
        }
    };

    struct Section
    {
        juce::String title;
        int row = 0;
        juce::GroupComponent group;
        std::vector<Control*> controls;
    };

    std::vector<std::unique_ptr<Control>> allControls;
    std::vector<std::unique_ptr<Section>> sections;

    Section& addSection (const juce::String& title, int row);
    Control& addKnob   (Section& sec, const juce::String& paramId, const juce::String& text);
    Control& addCombo  (Section& sec, const juce::String& paramId, const juce::String& text);
    Control& addToggle (Section& sec, const juce::String& paramId, const juce::String& text);

    void layoutRow (const std::vector<Section*>& rowSections, juce::Rectangle<int> rowBounds);
};
