#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "DSP/JunoSound.h"
#include "DSP/JunoVoice.h"

JunoCloneAudioProcessor::JunoCloneAudioProcessor()
    : AudioProcessor (BusesProperties().withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "PARAMS", createLayout())
{
    // --- Schritt: Voice-Stealing ---
    // Wenn alle Stimmen belegt sind, wird die aelteste/am wenigsten relevante
    // Stimme "gestohlen", anstatt neue Noten zu ignorieren - Standardverhalten
    // bei polyphonen Hardware- wie Software-Synths.
    synth.setNoteStealingEnabled (true);

    for (int i = 0; i < 8; ++i)
        synth.addVoice (new JunoVoice (voiceParams));
    synth.addSound (new JunoSound());

    auto bind = [this] (std::atomic<float>*& dest, const juce::String& id)
    {
        dest = apvts.getRawParameterValue (id);
    };
    bind (voiceParams.sawLevel,      "sawLevel");
    bind (voiceParams.pulseLevel,    "pulseLevel");
    bind (voiceParams.subLevel,      "subLevel");
    bind (voiceParams.noiseLevel,    "noiseLevel");
    bind (voiceParams.pwmAmount,     "pwmAmount");
    bind (voiceParams.pwmLfoAmount,  "pwmLfoAmount");
    bind (voiceParams.hpfCutoffStep, "hpfStep");
    bind (voiceParams.vcfCutoff,     "vcfCutoff");
    bind (voiceParams.vcfResonance,  "vcfResonance");
    bind (voiceParams.vcfEnvAmount,  "vcfEnvAmount");
    bind (voiceParams.vcfKeyFollow,  "vcfKeyFollow");
    bind (voiceParams.lfoRate,       "lfoRate");
    bind (voiceParams.lfoDelay,      "lfoDelay");
    bind (voiceParams.lfoToVcf,      "lfoToVcf");
    bind (voiceParams.lfoToDco,      "lfoToDco");
    bind (voiceParams.attack,        "attack");
    bind (voiceParams.decay,         "decay");
    bind (voiceParams.sustain,       "sustain");
    bind (voiceParams.release,       "release");
    bind (voiceParams.vcaGateMode,   "vcaGateMode");
    bind (voiceParams.glideTime,     "glideTime");

    chorusModeParam   = apvts.getRawParameterValue ("chorusMode");
    oversamplingParam = apvts.getRawParameterValue ("oversampling");

    synth.setUnisonParams (apvts.getRawParameterValue ("unisonVoices"),
                            apvts.getRawParameterValue ("unisonDetune"));

    apvts.addParameterListener ("oversampling", this);
}

JunoCloneAudioProcessor::~JunoCloneAudioProcessor()
{
    apvts.removeParameterListener ("oversampling", this);
}

juce::AudioProcessorValueTreeState::ParameterLayout JunoCloneAudioProcessor::createLayout()
{
    using P  = juce::AudioParameterFloat;
    using PC = juce::AudioParameterChoice;
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> p;

    p.push_back (std::make_unique<P> ("sawLevel",   "DCO Saw",   0.0f, 1.0f, 0.7f));
    p.push_back (std::make_unique<P> ("pulseLevel", "DCO Pulse", 0.0f, 1.0f, 0.0f));
    p.push_back (std::make_unique<P> ("subLevel",   "Sub Osc",   0.0f, 1.0f, 0.3f));
    p.push_back (std::make_unique<P> ("noiseLevel", "Noise",     0.0f, 1.0f, 0.0f));
    p.push_back (std::make_unique<P> ("pwmAmount",  "PWM",       -1.0f, 1.0f, 0.0f));
    p.push_back (std::make_unique<P> ("pwmLfoAmount","LFO>PWM",  0.0f, 1.0f, 0.0f));
    p.push_back (std::make_unique<P> ("hpfStep",    "HPF Step",  0.0f, 3.0f, 0.0f));
    p.push_back (std::make_unique<P> ("vcfCutoff",  "VCF Cutoff",
                    juce::NormalisableRange<float> (20.0f, 8000.0f, 1.0f, 0.3f), 1200.0f));
    p.push_back (std::make_unique<P> ("vcfResonance","VCF Resonance", 0.0f, 1.0f, 0.1f));
    p.push_back (std::make_unique<P> ("vcfEnvAmount","VCF Env Amount", -1.0f, 1.0f, 0.3f));
    p.push_back (std::make_unique<P> ("vcfKeyFollow","VCF Key Follow", 0.0f, 1.0f, 0.3f));
    p.push_back (std::make_unique<P> ("lfoRate",    "LFO Rate",  0.1f, 10.0f, 4.0f));
    p.push_back (std::make_unique<P> ("lfoDelay",   "LFO Delay", 0.0f, 3.0f, 0.0f));
    p.push_back (std::make_unique<P> ("lfoToVcf",   "LFO>VCF",   0.0f, 1.0f, 0.0f));
    p.push_back (std::make_unique<P> ("lfoToDco",   "LFO>DCO",   0.0f, 1.0f, 0.0f));
    p.push_back (std::make_unique<P> ("attack",     "Attack",    0.001f, 5.0f, 0.01f));
    p.push_back (std::make_unique<P> ("decay",      "Decay",     0.001f, 5.0f, 0.3f));
    p.push_back (std::make_unique<P> ("sustain",    "Sustain",   0.0f, 1.0f, 0.8f));
    p.push_back (std::make_unique<P> ("release",    "Release",   0.001f, 5.0f, 0.3f));
    p.push_back (std::make_unique<P> ("vcaGateMode","VCA Mode",  0.0f, 1.0f, 0.0f));

    // --- Neue Parameter aus diesem Erweiterungsschritt ---
    p.push_back (std::make_unique<PC> ("chorusMode", "Chorus",
                    juce::StringArray { "Off", "I", "II", "I+II" }, 0));
    p.push_back (std::make_unique<PC> ("unisonVoices", "Unison Voices",
                    juce::StringArray { "1", "2", "3", "4" }, 0));
    p.push_back (std::make_unique<P>  ("unisonDetune", "Unison Detune", 0.0f, 50.0f, 0.0f));
    p.push_back (std::make_unique<P>  ("glideTime", "Portamento", 0.0f, 2.0f, 0.0f));
    p.push_back (std::make_unique<PC> ("oversampling", "Quality",
                    juce::StringArray { "Normal", "High (2x Oversampling)" }, 0));

    return { p.begin(), p.end() };
}

void JunoCloneAudioProcessor::parameterChanged (const juce::String& parameterID, float)
{
    if (parameterID == "oversampling")
        oversamplingDirty.store (true);
}

void JunoCloneAudioProcessor::rebuildOversampler()
{
    int choice = oversamplingParam != nullptr ? (int) oversamplingParam->load() : 0;
    int factorExponent = choice >= 1 ? 1 : 0; // 0 -> 1x, 1 -> 2x
    currentOversamplingFactor = 1 << factorExponent;

    oversampler = std::make_unique<juce::dsp::Oversampling<float>> (
        2, factorExponent, juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR, true);
    oversampler->initProcessing ((size_t) juce::jmax (1, hostBlockSize));
    oversampler->reset();

    double voiceSampleRate = hostSampleRate * (double) currentOversamplingFactor;
    synth.setCurrentPlaybackSampleRate (voiceSampleRate);
    for (int i = 0; i < synth.getNumVoices(); ++i)
        if (auto* v = dynamic_cast<JunoVoice*> (synth.getVoice (i)))
            v->prepare (voiceSampleRate);

    juce::dsp::ProcessSpec spec { voiceSampleRate,
                                   (juce::uint32) (juce::jmax (1, hostBlockSize) * currentOversamplingFactor),
                                   2 };
    chorus.prepare (spec);
    chorus.reset();
}

void JunoCloneAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    hostSampleRate = sampleRate;
    hostBlockSize  = samplesPerBlock;
    rebuildOversampler();
    oversamplingDirty.store (false);
}

void JunoCloneAudioProcessor::scaleMidiBuffer (const juce::MidiBuffer& source, juce::MidiBuffer& dest, int factor)
{
    dest.clear();
    for (const auto metadata : source)
        dest.addEvent (metadata.getMessage(), metadata.samplePosition * factor);
}

void JunoCloneAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    buffer.clear();

    if (oversamplingDirty.exchange (false))
        rebuildOversampler();

    if (oversampler == nullptr)
        return;

    auto block = juce::dsp::AudioBlock<float> (buffer);

    size_t numOS = (size_t) buffer.getNumSamples() * (size_t) currentOversamplingFactor;
    auto osBlockFull = oversampler->getOversampledBuffer();
    auto osBlock = osBlockFull.getSubBlock (0, numOS);

    float* chans[2];
    chans[0] = osBlock.getChannelPointer (0);
    chans[1] = osBlock.getNumChannels() > 1 ? osBlock.getChannelPointer (1) : osBlock.getChannelPointer (0);

    juce::AudioBuffer<float> osBuffer (chans, (int) osBlock.getNumChannels(), (int) numOS);
    osBuffer.clear();

    juce::MidiBuffer scaledMidi;
    scaleMidiBuffer (midi, scaledMidi, currentOversamplingFactor);

    chorus.setMode (chorusModeParam != nullptr ? (int) chorusModeParam->load() : 0);

    synth.renderNextBlock (osBuffer, scaledMidi, 0, (int) numOS);
    chorus.process (osBlock);

    oversampler->processSamplesDown (block);
}

void JunoCloneAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    if (auto xml = apvts.copyState().createXml())
        copyXmlToBinary (*xml, destData);
}

void JunoCloneAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary (data, sizeInBytes))
        apvts.replaceState (juce::ValueTree::fromXml (*xml));
}

juce::AudioProcessorEditor* JunoCloneAudioProcessor::createEditor()
{
    return new JunoCloneAudioProcessorEditor (*this);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new JunoCloneAudioProcessor();
}
