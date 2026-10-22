#pragma once
#include <juce_core/juce_core.h>

// --- Schritt: Werks-Presets ---
// Handgebaute Startpunkte, die typische Juno-Klangcharaktere abdecken:
// warme Flaechen (Chorus I+II), perkussive Bass-Patches (Chorus Off, kurze
// Huellkurve), PWM-Streicher usw. Format: einfache "Parameter=Wert"-Paare,
// die beim ersten Programmstart in echte .xml-Dateien im Presets-Ordner
// umgewandelt werden (siehe PresetManager::writeFactoryPresetsIfMissing).
struct FactoryPreset
{
    juce::String name;
    std::map<juce::String, float> values;
};

inline const std::vector<FactoryPreset>& getFactoryPresets()
{
    static const std::vector<FactoryPreset> presets =
    {
        {
            "01 Init",
            {
                { "sawLevel", 0.7f }, { "pulseLevel", 0.0f }, { "subLevel", 0.0f }, { "noiseLevel", 0.0f },
                { "pwmAmount", 0.0f }, { "pwmLfoAmount", 0.0f }, { "hpfStep", 0.0f },
                { "vcfCutoff", 4000.0f }, { "vcfResonance", 0.05f }, { "vcfEnvAmount", 0.0f }, { "vcfKeyFollow", 0.3f },
                { "lfoRate", 4.0f }, { "lfoDelay", 0.0f }, { "lfoToVcf", 0.0f }, { "lfoToDco", 0.0f },
                { "attack", 0.01f }, { "decay", 0.3f }, { "sustain", 0.8f }, { "release", 0.3f },
                { "vcaGateMode", 0.0f }, { "chorusMode", 0.0f },
                { "unisonVoices", 0.0f }, { "unisonDetune", 0.0f }, { "glideTime", 0.0f }, { "oversampling", 0.0f }
            }
        },
        {
            "02 Warm Pad Chorus",
            {
                { "sawLevel", 0.8f }, { "pulseLevel", 0.3f }, { "subLevel", 0.2f }, { "noiseLevel", 0.0f },
                { "pwmAmount", 0.1f }, { "pwmLfoAmount", 0.2f }, { "hpfStep", 0.0f },
                { "vcfCutoff", 1800.0f }, { "vcfResonance", 0.15f }, { "vcfEnvAmount", 0.2f }, { "vcfKeyFollow", 0.5f },
                { "lfoRate", 3.5f }, { "lfoDelay", 0.6f }, { "lfoToVcf", 0.1f }, { "lfoToDco", 0.05f },
                { "attack", 0.8f }, { "decay", 1.2f }, { "sustain", 0.9f }, { "release", 1.5f },
                { "vcaGateMode", 0.0f }, { "chorusMode", 3.0f },
                { "unisonVoices", 1.0f }, { "unisonDetune", 8.0f }, { "glideTime", 0.0f }, { "oversampling", 1.0f }
            }
        },
        {
            "03 Classic Bass",
            {
                { "sawLevel", 0.6f }, { "pulseLevel", 0.0f }, { "subLevel", 0.7f }, { "noiseLevel", 0.0f },
                { "pwmAmount", 0.0f }, { "pwmLfoAmount", 0.0f }, { "hpfStep", 1.0f },
                { "vcfCutoff", 600.0f }, { "vcfResonance", 0.3f }, { "vcfEnvAmount", 0.6f }, { "vcfKeyFollow", 0.7f },
                { "lfoRate", 5.0f }, { "lfoDelay", 0.0f }, { "lfoToVcf", 0.0f }, { "lfoToDco", 0.0f },
                { "attack", 0.001f }, { "decay", 0.25f }, { "sustain", 0.4f }, { "release", 0.2f },
                { "vcaGateMode", 0.0f }, { "chorusMode", 0.0f },
                { "unisonVoices", 0.0f }, { "unisonDetune", 0.0f }, { "glideTime", 0.0f }, { "oversampling", 1.0f }
            }
        },
        {
            "04 PWM Strings",
            {
                { "sawLevel", 0.2f }, { "pulseLevel", 0.8f }, { "subLevel", 0.1f }, { "noiseLevel", 0.0f },
                { "pwmAmount", 0.0f }, { "pwmLfoAmount", 0.6f }, { "hpfStep", 0.0f },
                { "vcfCutoff", 2600.0f }, { "vcfResonance", 0.1f }, { "vcfEnvAmount", 0.1f }, { "vcfKeyFollow", 0.6f },
                { "lfoRate", 4.5f }, { "lfoDelay", 0.3f }, { "lfoToVcf", 0.0f }, { "lfoToDco", 0.0f },
                { "attack", 0.4f }, { "decay", 0.5f }, { "sustain", 0.85f }, { "release", 0.9f },
                { "vcaGateMode", 0.0f }, { "chorusMode", 2.0f },
                { "unisonVoices", 2.0f }, { "unisonDetune", 12.0f }, { "glideTime", 0.0f }, { "oversampling", 1.0f }
            }
        },
        {
            "05 Brass Stab",
            {
                { "sawLevel", 0.9f }, { "pulseLevel", 0.4f }, { "subLevel", 0.0f }, { "noiseLevel", 0.0f },
                { "pwmAmount", 0.0f }, { "pwmLfoAmount", 0.0f }, { "hpfStep", 0.0f },
                { "vcfCutoff", 1200.0f }, { "vcfResonance", 0.25f }, { "vcfEnvAmount", 0.8f }, { "vcfKeyFollow", 0.4f },
                { "lfoRate", 5.0f }, { "lfoDelay", 0.0f }, { "lfoToVcf", 0.0f }, { "lfoToDco", 0.0f },
                { "attack", 0.03f }, { "decay", 0.35f }, { "sustain", 0.6f }, { "release", 0.25f },
                { "vcaGateMode", 0.0f }, { "chorusMode", 1.0f },
                { "unisonVoices", 0.0f }, { "unisonDetune", 0.0f }, { "glideTime", 0.0f }, { "oversampling", 1.0f }
            }
        },
        {
            "06 Glide Lead",
            {
                { "sawLevel", 1.0f }, { "pulseLevel", 0.0f }, { "subLevel", 0.0f }, { "noiseLevel", 0.0f },
                { "pwmAmount", 0.0f }, { "pwmLfoAmount", 0.0f }, { "hpfStep", 0.0f },
                { "vcfCutoff", 3200.0f }, { "vcfResonance", 0.35f }, { "vcfEnvAmount", 0.3f }, { "vcfKeyFollow", 0.5f },
                { "lfoRate", 6.0f }, { "lfoDelay", 0.2f }, { "lfoToVcf", 0.0f }, { "lfoToDco", 0.15f },
                { "attack", 0.01f }, { "decay", 0.3f }, { "sustain", 0.8f }, { "release", 0.3f },
                { "vcaGateMode", 0.0f }, { "chorusMode", 1.0f },
                { "unisonVoices", 0.0f }, { "unisonDetune", 0.0f }, { "glideTime", 0.18f }, { "oversampling", 1.0f }
            }
        }
    };
    return presets;
}
