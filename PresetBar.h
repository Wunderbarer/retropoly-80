#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "../Presets/PresetManager.h"

class PresetBar : public juce::Component
{
public:
    explicit PresetBar (PresetManager& pmIn);

    void resized() override;
    void refreshLabel();

    std::function<void (const juce::String& currentName)> onSaveRequested;

private:
    PresetManager& pm;
    juce::TextButton prevButton { "<" };
    juce::TextButton nextButton { ">" };
    juce::TextButton saveButton { "Save As..." };
    // --- Schritt: Zufalls-Patch-Generator (GUI-Anbindung) ---
    juce::TextButton randomButton { "Random" };
    juce::Label nameLabel;
};
