#pragma once
#include <juce_core/juce_core.h>
#include <map>
#include <vector>

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
                { "lfo1Wave", 0 }, { "lfo1Rate", 4.0f }, { "lfo2Wave", 0 }, { "lfo2Rate", 0.5f },
                { "lfo3Rate", 5.0f }, { "lfo3ModWheelAmount", 0.5f },
                { "osc1Wave", 0 }, { "osc1Tune", 0.0f }, { "osc1Lfo1Mod", 0.0f }, { "osc1Lfo2Mod", 0.0f },
                { "osc1Eg1Pwm", 0.0f }, { "osc1Lfo1Pwm", 0.0f },
                { "osc2Wave", 0 }, { "osc2Tune", 0.0f }, { "osc2Lfo1Mod", 0.0f }, { "osc2Lfo2Mod", 0.0f },
                { "osc2Eg1Pwm", 0.0f }, { "osc2Lfo1Pwm", 0.0f }, { "osc2Sync", 0.0f },
                { "mixOsc1", 0.7f }, { "mixSub1", 0.0f }, { "mixOsc2", 0.0f }, { "mixSub2", 0.0f }, { "mixNoise", 0.0f },
                { "hpfStep", 0.0f },
                { "vcfCutoff", 4000.0f }, { "vcfResonance", 0.05f }, { "vcfKeyFollow", 0.3f },
                { "vcfEg1Mod", 0.0f }, { "vcfLfo1Mod", 0.0f }, { "vcfLfo2Mod", 0.0f },
                { "ampLfo1Mod", 0.0f }, { "ampLfo2Mod", 0.0f }, { "ampKeyTrack", 0.0f }, { "ampVelocityMod", 0.7f },
                { "vcaGateMode", 0.0f },
                { "eg1Attack", 0.01f }, { "eg1Decay", 0.3f }, { "eg1Sustain", 0.5f }, { "eg1Release", 0.3f },
                { "eg1VelocityMod", 0.0f }, { "eg1KeyTrack", 0.0f },
                { "eg2Attack", 0.01f }, { "eg2Decay", 0.3f }, { "eg2Sustain", 0.8f }, { "eg2Release", 0.3f },
                { "eg2VelocityMod", 0.0f }, { "eg2KeyTrack", 0.0f },
                { "unisonVoices", 0 }, { "unisonDetune", 0.0f }, { "glideTime", 0.0f },
                { "chorusRate", 0.5f }, { "chorusDepth", 0.3f }, { "chorusMix", 0.0f },
                { "delayTime", 0.3f }, { "delayFeedback", 0.3f }, { "delayMix", 0.0f },
                { "masterVolume", 1.0f }, { "oversampling", 0 }
            }
        },
        {
            "02 Warm Pad 2-OSC",
            {
                { "lfo1Wave", 0 }, { "lfo1Rate", 3.0f }, { "lfo2Wave", 0 }, { "lfo2Rate", 0.3f },
                { "lfo3Rate", 5.0f }, { "lfo3ModWheelAmount", 0.5f },
                { "osc1Wave", 0 }, { "osc1Tune", 0.0f }, { "osc1Lfo1Mod", 0.05f }, { "osc1Lfo2Mod", 0.0f },
                { "osc1Eg1Pwm", 0.1f }, { "osc1Lfo1Pwm", 0.1f },
                { "osc2Wave", 1 }, { "osc2Tune", -12.0f }, { "osc2Lfo1Mod", 0.05f }, { "osc2Lfo2Mod", 0.0f },
                { "osc2Eg1Pwm", 0.0f }, { "osc2Lfo1Pwm", 0.0f }, { "osc2Sync", 0.0f },
                { "mixOsc1", 0.6f }, { "mixSub1", 0.1f }, { "mixOsc2", 0.5f }, { "mixSub2", 0.0f }, { "mixNoise", 0.0f },
                { "hpfStep", 0.0f },
                { "vcfCutoff", 1800.0f }, { "vcfResonance", 0.15f }, { "vcfKeyFollow", 0.5f },
                { "vcfEg1Mod", 0.2f }, { "vcfLfo1Mod", 0.0f }, { "vcfLfo2Mod", 0.1f },
                { "ampLfo1Mod", 0.0f }, { "ampLfo2Mod", 0.0f }, { "ampKeyTrack", 0.0f }, { "ampVelocityMod", 0.5f },
                { "vcaGateMode", 0.0f },
                { "eg1Attack", 0.8f }, { "eg1Decay", 1.2f }, { "eg1Sustain", 0.7f }, { "eg1Release", 1.5f },
                { "eg1VelocityMod", 0.0f }, { "eg1KeyTrack", 0.0f },
                { "eg2Attack", 0.9f }, { "eg2Decay", 1.0f }, { "eg2Sustain", 0.9f }, { "eg2Release", 1.8f },
                { "eg2VelocityMod", 0.0f }, { "eg2KeyTrack", 0.0f },
                { "unisonVoices", 1 }, { "unisonDetune", 8.0f }, { "glideTime", 0.0f },
                { "chorusRate", 0.5f }, { "chorusDepth", 0.5f }, { "chorusMix", 0.6f },
                { "delayTime", 0.4f }, { "delayFeedback", 0.2f }, { "delayMix", 0.15f },
                { "masterVolume", 1.0f }, { "oversampling", 1 }
            }
        },
        {
            "03 Fat Bass 2-OSC",
            {
                { "lfo1Wave", 0 }, { "lfo1Rate", 5.0f }, { "lfo2Wave", 0 }, { "lfo2Rate", 0.5f },
                { "lfo3Rate", 5.0f }, { "lfo3ModWheelAmount", 0.3f },
                { "osc1Wave", 0 }, { "osc1Tune", 0.0f }, { "osc1Lfo1Mod", 0.0f }, { "osc1Lfo2Mod", 0.0f },
                { "osc1Eg1Pwm", 0.0f }, { "osc1Lfo1Pwm", 0.0f },
                { "osc2Wave", 0 }, { "osc2Tune", 0.08f }, { "osc2Lfo1Mod", 0.0f }, { "osc2Lfo2Mod", 0.0f },
                { "osc2Eg1Pwm", 0.0f }, { "osc2Lfo1Pwm", 0.0f }, { "osc2Sync", 0.0f },
                { "mixOsc1", 0.6f }, { "mixSub1", 0.6f }, { "mixOsc2", 0.5f }, { "mixSub2", 0.0f }, { "mixNoise", 0.0f },
                { "hpfStep", 1.0f },
                { "vcfCutoff", 600.0f }, { "vcfResonance", 0.3f }, { "vcfKeyFollow", 0.7f },
                { "vcfEg1Mod", 0.6f }, { "vcfLfo1Mod", 0.0f }, { "vcfLfo2Mod", 0.0f },
                { "ampLfo1Mod", 0.0f }, { "ampLfo2Mod", 0.0f }, { "ampKeyTrack", 0.0f }, { "ampVelocityMod", 0.8f },
                { "vcaGateMode", 0.0f },
                { "eg1Attack", 0.001f }, { "eg1Decay", 0.25f }, { "eg1Sustain", 0.3f }, { "eg1Release", 0.2f },
                { "eg1VelocityMod", 0.3f }, { "eg1KeyTrack", 0.2f },
                { "eg2Attack", 0.001f }, { "eg2Decay", 0.2f }, { "eg2Sustain", 0.6f }, { "eg2Release", 0.15f },
                { "eg2VelocityMod", 0.3f }, { "eg2KeyTrack", 0.0f },
                { "unisonVoices", 0 }, { "unisonDetune", 0.0f }, { "glideTime", 0.0f },
                { "chorusRate", 0.5f }, { "chorusDepth", 0.3f }, { "chorusMix", 0.0f },
                { "delayTime", 0.3f }, { "delayFeedback", 0.0f }, { "delayMix", 0.0f },
                { "masterVolume", 1.0f }, { "oversampling", 1 }
            }
        },
        {
            "04 PWM Strings",
            {
                { "lfo1Wave", 0 }, { "lfo1Rate", 4.5f }, { "lfo2Wave", 0 }, { "lfo2Rate", 0.4f },
                { "lfo3Rate", 5.0f }, { "lfo3ModWheelAmount", 0.5f },
                { "osc1Wave", 2 }, { "osc1Tune", 0.0f }, { "osc1Lfo1Mod", 0.0f }, { "osc1Lfo2Mod", 0.0f },
                { "osc1Eg1Pwm", 0.0f }, { "osc1Lfo1Pwm", 0.5f },
                { "osc2Wave", 2 }, { "osc2Tune", 0.1f }, { "osc2Lfo1Mod", 0.0f }, { "osc2Lfo2Mod", 0.0f },
                { "osc2Eg1Pwm", 0.0f }, { "osc2Lfo1Pwm", 0.5f }, { "osc2Sync", 0.0f },
                { "mixOsc1", 0.6f }, { "mixSub1", 0.0f }, { "mixOsc2", 0.5f }, { "mixSub2", 0.0f }, { "mixNoise", 0.0f },
                { "hpfStep", 0.0f },
                { "vcfCutoff", 2600.0f }, { "vcfResonance", 0.1f }, { "vcfKeyFollow", 0.6f },
                { "vcfEg1Mod", 0.1f }, { "vcfLfo1Mod", 0.0f }, { "vcfLfo2Mod", 0.0f },
                { "ampLfo1Mod", 0.0f }, { "ampLfo2Mod", 0.0f }, { "ampKeyTrack", 0.0f }, { "ampVelocityMod", 0.5f },
                { "vcaGateMode", 0.0f },
                { "eg1Attack", 0.4f }, { "eg1Decay", 0.5f }, { "eg1Sustain", 0.8f }, { "eg1Release", 0.9f },
                { "eg1VelocityMod", 0.0f }, { "eg1KeyTrack", 0.0f },
                { "eg2Attack", 0.5f }, { "eg2Decay", 0.4f }, { "eg2Sustain", 0.85f }, { "eg2Release", 1.0f },
                { "eg2VelocityMod", 0.0f }, { "eg2KeyTrack", 0.0f },
                { "unisonVoices", 2 }, { "unisonDetune", 12.0f }, { "glideTime", 0.0f },
                { "chorusRate", 0.6f }, { "chorusDepth", 0.4f }, { "chorusMix", 0.5f },
                { "delayTime", 0.25f }, { "delayFeedback", 0.15f }, { "delayMix", 0.1f },
                { "masterVolume", 1.0f }, { "oversampling", 1 }
            }
        },
        {
            "05 Sync Lead",
            {
                { "lfo1Wave", 0 }, { "lfo1Rate", 5.0f }, { "lfo2Wave", 0 }, { "lfo2Rate", 0.5f },
                { "lfo3Rate", 5.0f }, { "lfo3ModWheelAmount", 0.6f },
                { "osc1Wave", 0 }, { "osc1Tune", 0.0f }, { "osc1Lfo1Mod", 0.0f }, { "osc1Lfo2Mod", 0.0f },
                { "osc1Eg1Pwm", 0.0f }, { "osc1Lfo1Pwm", 0.0f },
                { "osc2Wave", 0 }, { "osc2Tune", 7.0f }, { "osc2Lfo1Mod", 0.0f }, { "osc2Lfo2Mod", 0.2f },
                { "osc2Eg1Pwm", 0.0f }, { "osc2Lfo1Pwm", 0.0f }, { "osc2Sync", 1.0f },
                { "mixOsc1", 0.5f }, { "mixSub1", 0.0f }, { "mixOsc2", 0.8f }, { "mixSub2", 0.0f }, { "mixNoise", 0.0f },
                { "hpfStep", 0.0f },
                { "vcfCutoff", 3200.0f }, { "vcfResonance", 0.35f }, { "vcfKeyFollow", 0.5f },
                { "vcfEg1Mod", 0.3f }, { "vcfLfo1Mod", 0.0f }, { "vcfLfo2Mod", 0.0f },
                { "ampLfo1Mod", 0.0f }, { "ampLfo2Mod", 0.0f }, { "ampKeyTrack", 0.0f }, { "ampVelocityMod", 0.7f },
                { "vcaGateMode", 0.0f },
                { "eg1Attack", 0.01f }, { "eg1Decay", 0.3f }, { "eg1Sustain", 0.6f }, { "eg1Release", 0.3f },
                { "eg1VelocityMod", 0.0f }, { "eg1KeyTrack", 0.0f },
                { "eg2Attack", 0.01f }, { "eg2Decay", 0.3f }, { "eg2Sustain", 0.8f }, { "eg2Release", 0.3f },
                { "eg2VelocityMod", 0.0f }, { "eg2KeyTrack", 0.0f },
                { "unisonVoices", 0 }, { "unisonDetune", 0.0f }, { "glideTime", 0.18f },
                { "chorusRate", 0.5f }, { "chorusDepth", 0.3f }, { "chorusMix", 0.2f },
                { "delayTime", 0.3f }, { "delayFeedback", 0.2f }, { "delayMix", 0.1f },
                { "masterVolume", 1.0f }, { "oversampling", 1 }
            }
        },
        {
            "06 Brass Stab",
            {
                { "lfo1Wave", 0 }, { "lfo1Rate", 5.0f }, { "lfo2Wave", 0 }, { "lfo2Rate", 0.5f },
                { "lfo3Rate", 5.0f }, { "lfo3ModWheelAmount", 0.4f },
                { "osc1Wave", 0 }, { "osc1Tune", 0.0f }, { "osc1Lfo1Mod", 0.0f }, { "osc1Lfo2Mod", 0.0f },
                { "osc1Eg1Pwm", 0.0f }, { "osc1Lfo1Pwm", 0.0f },
                { "osc2Wave", 2 }, { "osc2Tune", 0.0f }, { "osc2Lfo1Mod", 0.0f }, { "osc2Lfo2Mod", 0.0f },
                { "osc2Eg1Pwm", 0.2f }, { "osc2Lfo1Pwm", 0.0f }, { "osc2Sync", 0.0f },
                { "mixOsc1", 0.7f }, { "mixSub1", 0.0f }, { "mixOsc2", 0.5f }, { "mixSub2", 0.0f }, { "mixNoise", 0.0f },
                { "hpfStep", 0.0f },
                { "vcfCutoff", 1200.0f }, { "vcfResonance", 0.25f }, { "vcfKeyFollow", 0.4f },
                { "vcfEg1Mod", 0.8f }, { "vcfLfo1Mod", 0.0f }, { "vcfLfo2Mod", 0.0f },
                { "ampLfo1Mod", 0.0f }, { "ampLfo2Mod", 0.0f }, { "ampKeyTrack", 0.0f }, { "ampVelocityMod", 0.8f },
                { "vcaGateMode", 0.0f },
                { "eg1Attack", 0.03f }, { "eg1Decay", 0.35f }, { "eg1Sustain", 0.5f }, { "eg1Release", 0.25f },
                { "eg1VelocityMod", 0.2f }, { "eg1KeyTrack", 0.0f },
                { "eg2Attack", 0.03f }, { "eg2Decay", 0.35f }, { "eg2Sustain", 0.6f }, { "eg2Release", 0.25f },
                { "eg2VelocityMod", 0.2f }, { "eg2KeyTrack", 0.0f },
                { "unisonVoices", 0 }, { "unisonDetune", 0.0f }, { "glideTime", 0.0f },
                { "chorusRate", 0.5f }, { "chorusDepth", 0.3f }, { "chorusMix", 0.3f },
                { "delayTime", 0.3f }, { "delayFeedback", 0.2f }, { "delayMix", 0.0f },
                { "masterVolume", 1.0f }, { "oversampling", 1 }
            }
        }
    };
    return presets;
}
