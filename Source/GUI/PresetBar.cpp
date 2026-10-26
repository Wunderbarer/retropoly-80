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
    saveButton.onClick = [this] { showSaveDialog(); };
    // --- Schritt: Zufalls-Patch-Generator (GUI-Anbindung) ---
    // Ein Klick wuerfelt sofort einen neuen, musikalisch sinnvollen Klang
    // (siehe PresetManager::randomize()) und zeigt "Random" als Namen an.
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

void PresetBar::showSaveDialog()
{
    // Asynchroner Dialog (in Plugins darf niemals blockierend gewartet werden)
    auto* aw = new juce::AlertWindow ("Preset speichern", "Name eingeben:", juce::AlertWindow::NoIcon);
    aw->addTextEditor ("name", pm.getCurrentPresetName());
    aw->addButton ("OK", 1, juce::KeyPress (juce::KeyPress::returnKey));
    aw->addButton ("Abbrechen", 0, juce::KeyPress (juce::KeyPress::escapeKey));

    // deleteWhenDismissed = true: JUCE loescht das Fenster selbst nach dem Schliessen,
    // daher hier KEIN manuelles "delete aw" (sonst Doppel-Freigabe).
    aw->enterModalState (true, juce::ModalCallbackFunction::create ([this, aw] (int result)
    {
        if (result == 1)
        {
            auto name = aw->getTextEditorContents ("name");
            if (name.isNotEmpty())
            {
                pm.savePresetAs (name);
                refreshLabel();
            }
        }
    }), true);
}
