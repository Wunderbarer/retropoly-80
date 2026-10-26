#pragma once
#include <juce_audio_processors/juce_audio_processors.h>

// --- Schritt: Preset-Browser ---
// Presets werden als einzelne .xml-Dateien im Benutzerverzeichnis abgelegt
// (wie bei den meisten kommerziellen Plugins). Beim allerersten Start schreibt
// die Klasse die eingebauten Werks-Presets einmalig auf die Platte, danach ist
// "Vorheriges/Naechstes Preset" nur noch ein alphabetisches Durchblaettern
// des Ordnerinhalts - das macht Speichern/Laden/Browsen sehr einfach und robust.
class PresetManager
{
public:
    explicit PresetManager (juce::AudioProcessorValueTreeState& stateIn) : state (stateIn)
    {
        presetFolder = juce::File::getSpecialLocation (juce::File::userApplicationDataDirectory)
                           .getChildFile ("RetroPoly-80")
                           .getChildFile ("Presets");
        presetFolder.createDirectory();
        writeFactoryPresetsIfMissing();
        refreshPresetList();
    }

    const juce::Array<juce::File>& getAllPresets() const { return presets; }

    void refreshPresetList()
    {
        presets.clear();
        for (auto& f : presetFolder.findChildFiles (juce::File::findFiles, false, "*.xml"))
            presets.add (f);
        presets.sort();
    }

    bool loadPreset (const juce::File& file)
    {
        auto xml = juce::XmlDocument::parse (file);
        if (xml == nullptr) return false;
        state.replaceState (juce::ValueTree::fromXml (*xml));
        currentIndex = presets.indexOf (file);
        currentName = file.getFileNameWithoutExtension();
        return true;
    }

    void savePresetAs (const juce::String& name)
    {
        auto file = presetFolder.getChildFile (name + ".xml");
        if (auto xml = state.copyState().createXml())
            xml->writeTo (file);
        refreshPresetList();
        currentIndex = presets.indexOf (file);
        currentName = name;
    }

    juce::String getCurrentPresetName() const { return currentName; }

    // --- Schritt: Zufalls-Patch-Generator ---
    void randomize();

    void loadNext()
    {
        if (presets.isEmpty()) return;
        currentIndex = (currentIndex + 1) % presets.size();
        loadPreset (presets[currentIndex]);
    }

    void loadPrevious()
    {
        if (presets.isEmpty()) return;
        currentIndex = (currentIndex - 1 + presets.size()) % presets.size();
        loadPreset (presets[currentIndex]);
    }

private:
    void writeFactoryPresetsIfMissing();

    juce::AudioProcessorValueTreeState& state;
    juce::File presetFolder;
    juce::Array<juce::File> presets;
    int currentIndex = -1;
    juce::String currentName { "Init" };
};
