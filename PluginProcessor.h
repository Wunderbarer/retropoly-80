#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "DSP/UnisonSynthesiser.h"
#include "DSP/JunoChorus.h"
#include "DSP/JunoDelay.h"
#include "Presets/PresetManager.h"

class JunoCloneAudioProcessor : public juce::AudioProcessor,
                                 private juce::AudioProcessorValueTreeState::Listener
{
public:
    JunoCloneAudioProcessor();
    ~JunoCloneAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "RetroPoly-80"; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override
    {
        return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
    }

    int getActiveVoiceCount()
    {
        int count = 0;
        for (int i = 0; i < synth.getNumVoices(); ++i)
            if (synth.getVoice (i)->isVoiceActive())
                ++count;
        return count;
    }

    juce::AudioProcessorValueTreeState apvts;
    PresetManager presetManager { apvts };

private:
    juce::AudioProcessorValueTreeState::ParameterLayout createLayout();
    void parameterChanged (const juce::String& parameterID, float newValue) override;
    void rebuildOversampler();
    static void scaleMidiBuffer (const juce::MidiBuffer& source, juce::MidiBuffer& dest, int factor);

    UnisonSynthesiser synth;
    JunoParams voiceParams;
    JunoChorus chorus;
    JunoDelay delay;

    std::unique_ptr<juce::dsp::Oversampling<float>> oversampler;
    int currentOversamplingFactor = 1;
    double hostSampleRate = 44100.0;
    int hostBlockSize = 512;
    std::atomic<bool> oversamplingDirty { true };

    std::atomic<float>* oversamplingParam   = nullptr;
    std::atomic<float>* chorusRateParam     = nullptr;
    std::atomic<float>* chorusDepthParam    = nullptr;
    std::atomic<float>* chorusMixParam      = nullptr;
    std::atomic<float>* delayTimeParam      = nullptr;
    std::atomic<float>* delayFeedbackParam  = nullptr;
    std::atomic<float>* delayMixParam       = nullptr;
    std::atomic<float>* masterVolumeParam   = nullptr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JunoCloneAudioProcessor)
};
