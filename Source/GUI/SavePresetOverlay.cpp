#include "SavePresetOverlay.h"

SavePresetOverlay::SavePresetOverlay()
{
    setVisible (false);
    setAlwaysOnTop (true);

    titleLabel.setText ("Preset speichern", juce::dontSendNotification);
    titleLabel.setJustificationType (juce::Justification::centred);
    titleLabel.setColour (juce::Label::textColourId, juce::Colours::white);
    titleLabel.setFont (juce::Font (16.0f, juce::Font::bold));
    addAndMakeVisible (titleLabel);

    nameEditor.setMultiLine (false);
    nameEditor.setSelectAllWhenFocused (true);
    nameEditor.onReturnKey = [this] { triggerSave(); };
    nameEditor.onEscapeKey = [this]
    {
        if (onCancel) onCancel();
        hide();
    };
    addAndMakeVisible (nameEditor);

    okButton.onClick = [this] { triggerSave(); };
    cancelButton.onClick = [this]
    {
        if (onCancel) onCancel();
        hide();
    };
    addAndMakeVisible (okButton);
    addAndMakeVisible (cancelButton);
}

void SavePresetOverlay::show (const juce::String& initialName)
{
    nameEditor.setText (initialName, juce::dontSendNotification);
    setVisible (true);
    toFront (true);
    nameEditor.grabKeyboardFocus();
    nameEditor.selectAll();
    repaint();
}

void SavePresetOverlay::hide()
{
    setVisible (false);
}

void SavePresetOverlay::triggerSave()
{
    auto name = nameEditor.getText().trim();
    if (name.isNotEmpty() && onSave)
        onSave (name);
    hide();
}

void SavePresetOverlay::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::black.withAlpha (0.55f));

    auto box = getLocalBounds().withSizeKeepingCentre (340, 150);
    g.setColour (juce::Colour (0xff20334f));
    g.fillRoundedRectangle (box.toFloat(), 8.0f);
    g.setColour (juce::Colours::white.withAlpha (0.4f));
    g.drawRoundedRectangle (box.toFloat(), 8.0f, 1.5f);
}

void SavePresetOverlay::resized()
{
    auto box = getLocalBounds().withSizeKeepingCentre (340, 150).reduced (16);
    titleLabel.setBounds (box.removeFromTop (26));
    box.removeFromTop (8);
    nameEditor.setBounds (box.removeFromTop (30));
    box.removeFromTop (14);
    auto buttons = box.removeFromTop (32);
    okButton.setBounds (buttons.removeFromLeft (buttons.getWidth() / 2).reduced (4, 0));
    cancelButton.setBounds (buttons.reduced (4, 0));
}
