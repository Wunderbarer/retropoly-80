#pragma once
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>
#include "JunoSound.h"

// Zentrale Parameter-Referenzen, die von allen Stimmen gemeinsam genutzt werden.
struct JunoParams
{
    std::atomic<float>* sawLevel      = nullptr;
    std::atomic<float>* pulseLevel    = nullptr;
    std::atomic<float>* subLevel      = nullptr;
    std::atomic<float>* noiseLevel    = nullptr;
    std::atomic<float>* pwmAmount     = nullptr;
    std::atomic<float>* pwmLfoAmount  = nullptr;
    std::atomic<float>* hpfCutoffStep = nullptr;
    std::atomic<float>* vcfCutoff     = nullptr;
    std::atomic<float>* vcfResonance  = nullptr;
    std::atomic<float>* vcfEnvAmount  = nullptr;
    std::atomic<float>* vcfKeyFollow  = nullptr;
    std::atomic<float>* lfoRate       = nullptr;
    std::atomic<float>* lfoDelay      = nullptr;
    std::atomic<float>* lfoToVcf      = nullptr;
    std::atomic<float>* lfoToDco      = nullptr;
    std::atomic<float>* attack        = nullptr;
    std::atomic<float>* decay         = nullptr;
    std::atomic<float>* sustain       = nullptr;
    std::atomic<float>* release       = nullptr;
    std::atomic<float>* vcaGateMode   = nullptr;
    std::atomic<float>* glideTime     = nullptr;
};

class JunoVoice : public juce::SynthesiserVoice
{
public:
    explicit JunoVoice (JunoParams& p) : params (p) {}

    void prepare (double sr)
    {
        sampleRate = sr;
        ampEnv.setSampleRate (sr);
        filtEnv.setSampleRate (sr);
        for (auto& s : filterStages) s = 0.0f;

        const double smoothSec = 0.015;
        cutoffSmoother.reset (sr, smoothSec);
        resoSmoother.reset   (sr, smoothSec);
        sawSmoother.reset    (sr, smoothSec);
        pulseSmoother.reset  (sr, smoothSec);
        subSmoother.reset    (sr, smoothSec);
        noiseSmoother.reset  (sr, smoothSec);
        pwmSmoother.reset    (sr, smoothSec);

        freqSmoother.reset (sr, 0.0);
    }

    void setUnisonOffsets (float detuneCentsIn, float panIn)
    {
        unisonDetuneCents = detuneCentsIn;
        unisonPan = juce::jlimit (-1.0f, 1.0f, panIn);
    }

    bool canPlaySound (juce::SynthesiserSound* s) override { return dynamic_cast<JunoSound*> (s) != nullptr; }

    void startNote (int midiNoteNumber, float velocity, juce::SynthesiserSound*, int) override
    {
        float baseFreq = (float) juce::MidiMessage::getMidiNoteInHertz (midiNoteNumber);
        float targetFreq = baseFreq * std::pow (2.0f, unisonDetuneCents / 1200.0f);

        float glideSec = params.glideTime != nullptr ? params.glideTime->load() : 0.0f;
        if (hasPlayedBefore && glideSec > 0.0001f)
        {
            freqSmoother.reset (sampleRate, glideSec);
            freqSmoother.setTargetValue (targetFreq);
        }
        else
        {
            freqSmoother.setCurrentAndTargetValue (targetFreq);
        }
        hasPlayedBefore = true;

        dcoPhase = 0.0f;
        subPhase = 0.0f;
        lfoPhase = 0.0f;
        lfoDelayCounter = 0.0f;
        velocityGain = velocity;

        juce::ADSR::Parameters ap { params.attack->load(), params.decay->load(), params.sustain->load(), params.release->load() };
        ampEnv.setParameters (ap);
        filtEnv.setParameters (ap);
        ampEnv.noteOn();
        filtEnv.noteOn();
    }

    void stopNote (float, bool allowTailOff) override
    {
        ampEnv.noteOff();
        filtEnv.noteOff();
        if (! allowTailOff) clearCurrentNote();
    }

    void pitchWheelMoved (int) override {}
    void controllerMoved (int, int) override {}

    static float polyBlep (float t, float dt)
    {
        if (t < dt)        { t /= dt; return t + t - t * t - 1.0f; }
        if (t > 1.0f - dt) { t = (t - 1.0f) / dt; return t * t + t + t + 1.0f; }
        return 0.0f;
    }

    void renderNextBlock (juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override
    {
        if (! isVoiceActive()) return;

        const float sr = (float) sampleRate;

        cutoffSmoother.setTargetValue (params.vcfCutoff->load());
        resoSmoother.setTargetValue   (params.vcfResonance->load());
        sawSmoother.setTargetValue    (params.sawLevel->load());
        pulseSmoother.setTargetValue  (params.pulseLevel->load());
        subSmoother.setTargetValue    (params.subLevel->load());
        noiseSmoother.setTargetValue  (params.noiseLevel->load());
        pwmSmoother.setTargetValue    (params.pwmAmount->load());

        for (int i = 0; i < numSamples; ++i)
        {
            float lfoRateHz = params.lfoRate->load();
            lfoPhase += lfoRateHz / sr;
            if (lfoPhase >= 1.0f) lfoPhase -= 1.0f;
            float lfoTri = 1.0f - std::abs (4.0f * lfoPhase - 2.0f);

            float lfoDelayTime = params.lfoDelay->load();
            if (lfoDelayTime > 0.0f && lfoDelayCounter < lfoDelayTime)
            {
                lfoDelayCounter += 1.0f / sr;
                lfoTri *= juce::jlimit (0.0f, 1.0f, lfoDelayCounter / lfoDelayTime);
            }

            float smoothFreq = freqSmoother.getNextValue();
            float vibrato = 1.0f + (lfoTri * params.lfoToDco->load() * 0.03f);
            float freq = smoothFreq * vibrato;
            float dt = freq / sr;

            dcoPhase += dt;
            if (dcoPhase >= 1.0f) dcoPhase -= 1.0f;

            float saw = (2.0f * dcoPhase - 1.0f) - polyBlep (dcoPhase, dt);

            float pw = 0.5f + 0.49f * (pwmSmoother.getNextValue() + lfoTri * params.pwmLfoAmount->load());
            pw = juce::jlimit (0.02f, 0.98f, pw);
            float pulsePhase = dcoPhase + pw; if (pulsePhase >= 1.0f) pulsePhase -= 1.0f;
            float pulse = dcoPhase < pw ? 1.0f : -1.0f;
            pulse += polyBlep (dcoPhase, dt) - polyBlep (pulsePhase, dt);

            subPhase += dt * 0.5f;
            if (subPhase >= 1.0f) subPhase -= 1.0f;
            float sub = subPhase < 0.5f ? 1.0f : -1.0f;

            float noise = random.nextFloat() * 2.0f - 1.0f;

            float dcoMix = saw * sawSmoother.getNextValue()
                         + pulse * pulseSmoother.getNextValue()
                         + sub * subSmoother.getNextValue()
                         + noise * noiseSmoother.getNextValue();

            dcoMix = applyHpf (dcoMix, (int) params.hpfCutoffStep->load());

            float envVal = filtEnv.getNextSample();
            float keyTrack = juce::jmap (smoothFreq, 55.0f, 1760.0f, 0.0f, 1.0f) * params.vcfKeyFollow->load();
            float cutoffBase = cutoffSmoother.getNextValue();
            float cutoffHz = cutoffBase
                            * std::pow (2.0f, params.vcfEnvAmount->load() * envVal * 5.0f
                                              + lfoTri * params.lfoToVcf->load() * 2.0f
                                              + keyTrack * 4.0f);
            cutoffHz = juce::jlimit (20.0f, sr * 0.45f, cutoffHz);
            float filtered = ladderFilter (dcoMix, cutoffHz, resoSmoother.getNextValue(), sr);

            float ampVal = params.vcaGateMode->load() > 0.5f
                           ? (ampEnv.isActive() ? 1.0f : 0.0f)
                           : ampEnv.getNextSample();
            float out = filtered * ampVal * velocityGain * 0.3f;

            float panNorm = (unisonPan + 1.0f) * 0.5f;
            float gainL = std::cos (panNorm * juce::MathConstants<float>::halfPi);
            float gainR = std::sin (panNorm * juce::MathConstants<float>::halfPi);

            if (outputBuffer.getNumChannels() >= 2)
            {
                outputBuffer.addSample (0, startSample + i, out * gainL);
                outputBuffer.addSample (1, startSample + i, out * gainR);
            }
            else
            {
                outputBuffer.addSample (0, startSample + i, out);
            }

            if (! ampEnv.isActive()) { clearCurrentNote(); break; }
        }
    }

private:
    float applyHpf (float x, int step)
    {
        static const float cutoffs[4] = { 30.0f, 150.0f, 500.0f, 1500.0f };
        float fc = cutoffs[juce::jlimit (0, 3, step)];
        float rc = 1.0f / (2.0f * juce::MathConstants<float>::pi * fc);
        float a = rc / (rc + (1.0f / (float) sampleRate));
        float y = a * (hpfPrevY + x - hpfPrevX);
        hpfPrevX = x; hpfPrevY = y;
        return y;
    }

    float ladderFilter (float input, float cutoffHz, float resonance, float sr)
    {
        float g = std::tan (juce::MathConstants<float>::pi * cutoffHz / sr);
        float G = g / (1.0f + g);
        float k = resonance * 3.6f;

        float input_fb = input - k * filterStages[3];
        float stage = std::tanh (input_fb * 0.8f) * 1.25f;

        for (int s = 0; s < 4; ++s)
        {
            float v = (stage - filterStages[s]) * G;
            float out = v + filterStages[s];
            out = std::tanh (out * 0.9f) * 1.111f;
            filterStages[s] = out + v;
            stage = out;
        }
        return stage;
    }

    JunoParams& params;
    double sampleRate = 44100.0;
    float velocityGain = 1.0f;
    float dcoPhase = 0.0f, subPhase = 0.0f, lfoPhase = 0.0f, lfoDelayCounter = 0.0f;
    float hpfPrevX = 0.0f, hpfPrevY = 0.0f;
    float filterStages[4] = { 0, 0, 0, 0 };
    float unisonDetuneCents = 0.0f, unisonPan = 0.0f;
    bool hasPlayedBefore = false;

    juce::ADSR ampEnv, filtEnv;
    juce::Random random;

    juce::SmoothedValue<float> freqSmoother { 440.0f };
    juce::SmoothedValue<float> cutoffSmoother, resoSmoother;
    juce::SmoothedValue<float> sawSmoother, pulseSmoother, subSmoother, noiseSmoother, pwmSmoother;
};
