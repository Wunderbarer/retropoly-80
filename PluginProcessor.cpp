#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "DSP/JunoSound.h"
#include "DSP/JunoVoice.h"

JunoCloneAudioProcessor::JunoCloneAudioProcessor()
    : AudioProcessor (BusesProperties().withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "PARAMS", createLayout())
{
    synth.setNoteStealingEnabled (true);

    // --- Schritt: Polyphonie erhoeht ---
    // 16 statt vormals 8 gleichzeitige Stimmen.
    for (int i = 0; i < 16; ++i)
        synth.addVoice (new JunoVoice (voiceParams));
    synth.addSound (new JunoSound());

    auto bind = [this] (std::atomic<float>*& dest, const juce::String& id)
    {
        dest = apvts.getRawParameterValue (id);
    };

    bind (voiceParams.lfo1Wave, "lfo1Wave");
    bind (voiceParams.lfo1Rate, "lfo1Rate");
    bind (voiceParams.lfo2Wave, "lfo2Wave");
    bind (voiceParams.lfo2Rate, "lfo2Rate");
    bind (voiceParams.lfo3Rate, "lfo3Rate");
    bind (voiceParams.lfo3ModWheelAmount, "lfo3ModWheelAmount");

    bind (voiceParams.osc1Wave,     "osc1Wave");
    bind (voiceParams.osc1Tune,     "osc1Tune");
    bind (voiceParams.osc1Lfo1Mod,  "osc1Lfo1Mod");
    bind (voiceParams.osc1Lfo2Mod,  "osc1Lfo2Mod");
    bind (voiceParams.osc1Eg1Pwm,   "osc1Eg1Pwm");
    bind (voiceParams.osc1Lfo1Pwm,  "osc1Lfo1Pwm");

    bind (voiceParams.osc2Wave,     "osc2Wave");
    bind (voiceParams.osc2Tune,     "osc2Tune");
    bind (voiceParams.osc2Lfo1Mod,  "osc2Lfo1Mod");
    bind (voiceParams.osc2Lfo2Mod,  "osc2Lfo2Mod");
    bind (voiceParams.osc2Eg1Pwm,   "osc2Eg1Pwm");
    bind (voiceParams.osc2Lfo1Pwm,  "osc2Lfo1Pwm");
    bind (voiceParams.osc2Sync,     "osc2Sync");

    bind (voiceParams.mixOsc1,  "mixOsc1");
    bind (voiceParams.mixSub1,  "mixSub1");
    bind (voiceParams.mixOsc2,  "mixOsc2");
    bind (voiceParams.mixSub2,  "mixSub2");
    bind (voiceParams.mixNoise, "mixNoise");

    bind (voiceParams.hpfCutoffStep, "hpfStep");

    bind (voiceParams.vcfCutoff,    "vcfCutoff");
    bind (voiceParams.vcfResonance, "vcfResonance");
    bind (voiceParams.vcfKeyFollow, "vcfKeyFollow");
    bind (voiceParams.vcfEg1Mod,    "vcfEg1Mod");
    bind (voiceParams.vcfLfo1Mod,   "vcfLfo1Mod");
    bind (voiceParams.vcfLfo2Mod,   "vcfLfo2Mod");

    bind (voiceParams.ampLfo1Mod,     "ampLfo1Mod");
    bind (voiceParams.ampLfo2Mod,     "ampLfo2Mod");
    bind (voiceParams.ampKeyTrack,    "ampKeyTrack");
    bind (voiceParams.ampVelocityMod, "ampVelocityMod");
    bind (voiceParams.vcaGateMode,    "vcaGateMode");

    bind (voiceParams.eg1Attack,      "eg1Attack");
    bind (voiceParams.eg1Decay,       "eg1Decay");
    bind (voiceParams.eg1Sustain,     "eg1Sustain");
    bind (voiceParams.eg1Release,     "eg1Release");
    bind (voiceParams.eg1VelocityMod, "eg1VelocityMod");
    bind (voiceParams.eg1KeyTrack,    "eg1KeyTrack");

    bind (voiceParams.eg2Attack,      "eg2Attack");
    bind (voiceParams.eg2Decay,       "eg2Decay");
    bind (voiceParams.eg2Sustain,     "eg2Sustain");
    bind (voiceParams.eg2Release,     "eg2Release");
    bind (voiceParams.eg2VelocityMod, "eg2VelocityMod");
    bind (voiceParams.eg2KeyTrack,    "eg2KeyTrack");

    bind (voiceParams.glideTime, "glideTime");

    oversamplingParam  = apvts.getRawParameterValue ("oversampling");
    chorusRateParam    = apvts.getRawParameterValue ("chorusRate");
    chorusDepthParam   = apvts.getRawParameterValue ("chorusDepth");
    chorusMixParam     = apvts.getRawParameterValue ("chorusMix");
    delayTimeParam     = apvts.getRawParameterValue ("delayTime");
    delayFeedbackParam = apvts.getRawParameterValue ("delayFeedback");
    delayMixParam      = apvts.getRawParameterValue ("delayMix");
    masterVolumeParam  = apvts.getRawParameterValue ("masterVolume");

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
    using PB = juce::AudioParameterBool;
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> p;

    juce::StringArray lfoWaves { "Triangle", "Saw Up", "Saw Down", "Square", "Random" };
    juce::StringArray oscWaves { "Saw", "Triangle", "Pulse" };

    p.push_back (std::make_unique<PC> ("lfo1Wave", "LFO1 Wave", lfoWaves, 0));
    p.push_back (std::make_unique<P>  ("lfo1Rate", "LFO1 Rate", 0.02f, 20.0f, 4.0f));
    p.push_back (std::make_unique<PC> ("lfo2Wave", "LFO2 Wave", lfoWaves, 0));
    p.push_back (std::make_unique<P>  ("lfo2Rate", "LFO2 Rate", 0.02f, 20.0f, 0.5f));
    p.push_back (std::make_unique<P>  ("lfo3Rate", "LFO3 Rate", 0.5f, 10.0f, 5.0f));
    p.push_back (std::make_unique<P>  ("lfo3ModWheelAmount", "LFO3 ModWheel Amt", 0.0f, 1.0f, 0.5f));

    p.push_back (std::make_unique<PC> ("osc1Wave", "OSC1 Wave", oscWaves, 0));
    p.push_back (std::make_unique<P>  ("osc1Tune", "OSC1 Tune", -24.0f, 24.0f, 0.0f));
    p.push_back (std::make_unique<P>  ("osc1Lfo1Mod", "OSC1 LFO1 Mod", 0.0f, 1.0f, 0.0f));
    p.push_back (std::make_unique<P>  ("osc1Lfo2Mod", "OSC1 LFO2 Mod", 0.0f, 1.0f, 0.0f));
    p.push_back (std::make_unique<P>  ("osc1Eg1Pwm",  "OSC1 EG1 PWM",  0.0f, 1.0f, 0.0f));
    p.push_back (std::make_unique<P>  ("osc1Lfo1Pwm", "OSC1 LFO1 PWM", 0.0f, 1.0f, 0.0f));

    p.push_back (std::make_unique<PC> ("osc2Wave", "OSC2 Wave", oscWaves, 0));
    p.push_back (std::make_unique<P>  ("osc2Tune", "OSC2 Tune", -24.0f, 24.0f, 0.0f));
    p.push_back (std::make_unique<P>  ("osc2Lfo1Mod", "OSC2 LFO1 Mod", 0.0f, 1.0f, 0.0f));
    p.push_back (std::make_unique<P>  ("osc2Lfo2Mod", "OSC2 LFO2 Mod", 0.0f, 1.0f, 0.0f));
    p.push_back (std::make_unique<P>  ("osc2Eg1Pwm",  "OSC2 EG1 PWM",  0.0f, 1.0f, 0.0f));
    p.push_back (std::make_unique<P>  ("osc2Lfo1Pwm", "OSC2 LFO1 PWM", 0.0f, 1.0f, 0.0f));
    p.push_back (std::make_unique<PB> ("osc2Sync", "OSC2 Sync", false));

    p.push_back (std::make_unique<P> ("mixOsc1",  "Mix OSC1",  0.0f, 1.0f, 0.7f));
    p.push_back (std::make_unique<P> ("mixSub1",  "Mix Sub1",  0.0f, 1.0f, 0.0f));
    p.push_back (std::make_unique<P> ("mixOsc2",  "Mix OSC2",  0.0f, 1.0f, 0.0f));
    p.push_back (std::make_unique<P> ("mixSub2",  "Mix Sub2",  0.0f, 1.0f, 0.0f));
    p.push_back (std::make_unique<P> ("mixNoise", "Mix Noise", 0.0f, 1.0f, 0.0f));

    p.push_back (std::make_unique<P> ("hpfStep", "HPF Step", 0.0f, 3.0f, 0.0f));

    p.push_back (std::make_unique<P> ("vcfCutoff", "VCF Cutoff",
                    juce::NormalisableRange<float> (20.0f, 8000.0f, 1.0f, 0.3f), 1200.0f));
    p.push_back (std::make_unique<P> ("vcfResonance", "VCF Resonance", 0.0f, 1.0f, 0.1f));
    p.push_back (std::make_unique<P> ("vcfKeyFollow", "VCF Key Follow", 0.0f, 1.0f, 0.3f));
    p.push_back (std::make_unique<P> ("vcfEg1Mod", "VCF EG1 Mod", -1.0f, 1.0f, 0.3f));
    p.push_back (std::make_unique<P> ("vcfLfo1Mod", "VCF LFO1 Mod", 0.0f, 1.0f, 0.0f));
    p.push_back (std::make_unique<P> ("vcfLfo2Mod", "VCF LFO2 Mod", 0.0f, 1.0f, 0.0f));

    p.push_back (std::make_unique<P> ("ampLfo1Mod", "AMP LFO1 Mod", 0.0f, 1.0f, 0.0f));
    p.push_back (std::make_unique<P> ("ampLfo2Mod", "AMP LFO2 Mod", 0.0f, 1.0f, 0.0f));
    p.push_back (std::make_unique<P> ("ampKeyTrack", "AMP Key Track", 0.0f, 1.0f, 0.0f));
    p.push_back (std::make_unique<P> ("ampVelocityMod", "AMP Velocity Mod", 0.0f, 1.0f, 0.7f));
    p.push_back (std::make_unique<PC> ("vcaGateMode", "VCA Mode", juce::StringArray { "ENV", "GATE" }, 0));

    p.push_back (std::make_unique<P> ("eg1Attack",  "EG1 Attack",  0.001f, 5.0f, 0.01f));
    p.push_back (std::make_unique<P> ("eg1Decay",   "EG1 Decay",   0.001f, 5.0f, 0.3f));
    p.push_back (std::make_unique<P> ("eg1Sustain", "EG1 Sustain", 0.0f, 1.0f, 0.5f));
    p.push_back (std::make_unique<P> ("eg1Release", "EG1 Release", 0.001f, 5.0f, 0.3f));
    p.push_back (std::make_unique<P> ("eg1VelocityMod", "EG1 Velocity Mod", 0.0f, 1.0f, 0.0f));
    p.push_back (std::make_unique<P> ("eg1KeyTrack",    "EG1 Key Track",    0.0f, 1.0f, 0.0f));

    p.push_back (std::make_unique<P> ("eg2Attack",  "EG2 Attack",  0.001f, 5.0f, 0.01f));
    p.push_back (std::make_unique<P> ("eg2Decay",   "EG2 Decay",   0.001f, 5.0f, 0.3f));
    p.push_back (std::make_unique<P> ("eg2Sustain", "EG2 Sustain", 0.0f, 1.0f, 0.8f));
    p.push_back (std::make_unique<P> ("eg2Release", "EG2 Release", 0.001f, 5.0f, 0.3f));
    p.push_back (std::make_unique<P> ("eg2VelocityMod", "EG2 Velocity Mod", 0.0f, 1.0f, 0.0f));
    p.push_back (std::make_unique<P> ("eg2KeyTrack",    "EG2 Key Track",    0.0f, 1.0f, 0.0f));

    p.push_back (std::make_unique<PC> ("unisonVoices", "Unison Voices", juce::StringArray { "1", "2", "3", "4" }, 0));
    p.push_back (std::make_unique<P>  ("unisonDetune", "Unison Detune", 0.0f, 50.0f, 0.0f));
    p.push_back (std::make_unique<P>  ("glideTime", "Portamento", 0.0f, 2.0f, 0.0f));

    p.push_back (std::make_unique<P> ("chorusRate",  "Chorus Rate",  0.02f, 5.0f, 0.5f));
    p.push_back (std::make_unique<P> ("chorusDepth", "Chorus Depth", 0.0f, 1.0f, 0.3f));
    p.push_back (std::make_unique<P> ("chorusMix",   "Chorus Mix",   0.0f, 1.0f, 0.0f));

    p.push_back (std::make_unique<P> ("delayTime",     "Delay Time",     0.01f, 2.0f, 0.3f));
    p.push_back (std::make_unique<P> ("delayFeedback", "Delay Feedback", 0.0f, 0.95f, 0.3f));
    p.push_back (std::make_unique<P> ("delayMix",      "Delay Mix",      0.0f, 1.0f, 0.0f));

    p.push_back (std::make_unique<P>  ("masterVolume", "Master Volume", 0.0f, 1.5f, 1.0f));
    p.push_back (std::make_unique<PC> ("oversampling", "Quality", juce::StringArray { "Normal", "High (2x Oversampling)" }, 0));

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
    int factorExponent = choice >= 1 ? 1 : 0;
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
    delay.prepare (spec);
    delay.reset();
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

    juce::dsp::AudioBlock<float> originalBlock (buffer);

    auto osBlock = oversampler->processSamplesUp (originalBlock);
    const int numOS = (int) osBlock.getNumSamples();

    float* channelPointers[2];
    channelPointers[0] = osBlock.getChannelPointer (0);
    channelPointers[1] = osBlock.getNumChannels() > 1 ? osBlock.getChannelPointer (1)
                                                       : osBlock.getChannelPointer (0);

    juce::AudioBuffer<float> osBuffer (channelPointers, (int) osBlock.getNumChannels(), numOS);
    osBuffer.clear();

    juce::MidiBuffer scaledMidi;
    scaleMidiBuffer (midi, scaledMidi, currentOversamplingFactor);

    synth.renderNextBlock (osBuffer, scaledMidi, 0, numOS);

    chorus.setParams (chorusRateParam != nullptr ? chorusRateParam->load() : 0.5f,
                       chorusDepthParam != nullptr ? chorusDepthParam->load() : 0.3f,
                       chorusMixParam != nullptr ? chorusMixParam->load() : 0.0f);
    chorus.process (osBlock);

    delay.setParams (delayTimeParam != nullptr ? delayTimeParam->load() : 0.3f,
                      delayFeedbackParam != nullptr ? delayFeedbackParam->load() : 0.3f,
                      delayMixParam != nullptr ? delayMixParam->load() : 0.0f);
    delay.process (osBlock);

    oversampler->processSamplesDown (originalBlock);

    float mv = masterVolumeParam != nullptr ? masterVolumeParam->load() : 1.0f;
    buffer.applyGain (mv);
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
