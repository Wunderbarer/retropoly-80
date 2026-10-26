#include "PresetBar.h"

PresetBar::PresetBar (PresetManager& pmIn) : pm (pmIn)
{
    addAndMakeVisible (prevButton);
    addAndMakeVisible (nextButton);
    addAndMakeVisible (saveButton);
    addAndMakeVisible (randomButton);
    addAndMakeVisible (nameLabel);

    nameLabel.setJustificationType (juce::Justification::centred);
    nameLabel.setColour (juce::Label::textColourId, juce::Colours::white);
    nameLabel.setFont (juce::Font (15.0f, juce::Font::bold));

    prevButton.onClick = [this] { pm.loadPrevious(); refreshLabel(); };
    nextButton.onClick = [this] { pm.loadNext();     refreshLabel(); };
    saveButton.onClick = [this]
    {
        if (onSaveRequested)
            onSaveRequested (pm.getCurrentPresetName());
    };
    // --- Schritt: Zufalls-Patch-Generator (GUI-Anbindung) ---
    // Ein Klick wuerfelt sofort einen neuen, musikalisch sinnvollen Klang.
    randomButton.onClick = [this] { pm.randomize(); refreshLabel(); };

    refreshLabel();
}

void PresetBar::refreshLabel()
{
    nameLabel.setText (pm.getCurrentPresetName(), juce::dontSendNotification);
}

void PresetBar::resized()
{
    auto area = getLocalBounds().reduced (4);
    prevButton.setBounds (area.removeFromLeft (30));
    saveButton.setBounds (area.removeFromRight (90));
    randomButton.setBounds (area.removeFromRight (70));
    nextButton.setBounds (area.removeFromRight (30));
    nameLabel.setBounds (area);
}
