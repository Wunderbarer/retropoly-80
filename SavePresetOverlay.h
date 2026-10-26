#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

class SavePresetOverlay : public juce::Component
{
public:
    SavePresetOverlay();

    void show (const juce::String& initialName);
    void hide();

    void paint (juce::Graphics&) override;
    void resized() override;

    std::function<void (const juce::String&)> onSave;
    std::function<void()> onCancel;

private:
    juce::Label titleLabel;
    juce::TextEditor nameEditor;
    juce::TextButton okButton { "Speichern" };
    juce::TextButton cancelButton { "Abbrechen" };

    void triggerSave();
};
