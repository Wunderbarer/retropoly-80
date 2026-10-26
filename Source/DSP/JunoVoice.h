#pragma once
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>
#include "JunoSound.h"

struct JunoParams
{
    std::atomic<float>* lfo1Wave = nullptr;
    std::atomic<float>* lfo1Rate = nullptr;
    std::atomic<float>* lfo2Wave = nullptr;
    std::atomic<float>* lfo2Rate = nullptr;
    std::atomic<float>* lfo3Rate = nullptr;
    std::atomic<float>* lfo3ModWheelAmount = nullptr;
    std::atomic<float>* osc1Wave = nullptr;
    std::atomic<float>* osc1Tune = nullptr;
    std::atomic<float>* osc1Lfo1Mod = nullptr;
    std::atomic<float>* osc1Lfo2Mod = nullptr;
    std::atomic<float>* osc1Eg1Pwm = nullptr;
    std::atomic<float>* osc1Lfo1Pwm = nullptr;
    std::atomic<float>* osc2Wave = nullptr;
    std::atomic<float>* osc2Tune = nullptr;
    std::atomic<float>* osc2Lfo1Mod = nullptr;
    std::atomic<float>* osc2Lfo2Mod = nullptr;
    std::atomic<float>* osc2Eg1Pwm = nullptr;
    std::atomic<float>* osc2Lfo1Pwm = nullptr;
    std::atomic<float>* osc2Sync = nullptr;
    std::atomic<float>* mixOsc1  = nullptr;
    std::atomic<float>* mixSub1  = nullptr;
    std::atomic<float>* mixOsc2  = nullptr;
    std::atomic<float>* mixSub2  = nullptr;
    std::atomic<float>* mixNoise = nullptr;
    std::atomic<float>* hpfCutoffStep = nullptr;
    std::atomic<float>* vcfCutoff    = nullptr;
    std::atomic<float>* vcfResonance = nullptr;
    std::atomic<float>* vcfKeyFollow = nullptr;
    std::atomic<float>* vcfEg1Mod    = nullptr;
    std::atomic<float>* vcfLfo1Mod   = nullptr;
    std::atomic<float>* vcfLfo2Mod   = nullptr;
    std::atomic<float>* ampLfo1Mod     = nullptr;
    std::atomic<float>* ampLfo2Mod     = nullptr;
    std::atomic<float>* ampKeyTrack    = nullptr;
    std::atomic<float>* ampVelocityMod = nullptr;
    std::atomic<float>* vcaGateMode    = nullptr;
    std::atomic<float>* eg1Attack   = nullptr;
    std::atomic<float>* eg1Decay    = nullptr;
    std::atomic<float>* eg1Sustain  = nullptr;
    std::atomic<float>* eg1Release  = nullptr;
    std::atomic<float>* eg1VelocityMod = nullptr;
    std::atomic<float>* eg1KeyTrack    = nullptr;
    std::atomic<float>* eg2Attack   = nullptr;
    std::atomic<float>* eg2Decay    = nullptr;
    std::atomic<float>* eg2Sustain  = nullptr;
    std::atomic<float>* eg2Release  = nullptr;
    std::atomic<float>* eg2VelocityMod = nullptr;
    std::atomic<float>* eg2KeyTrack    = nullptr;
    std::atomic<float>* glideTime = nullptr;
};

class JunoVoice : public juce::SynthesiserVoice
{
public:
    explicit JunoVoice (JunoParams& p) : params (p) {}

    void prepare (double sr)
    {
        sampleRate = sr;
        eg1.setSampleRate (sr);
        eg2.setSampleRate (sr);
        for (auto& s : filterStages) s = 0.0f;
        const double smoothSec = 0.015;
        cutoffSmoother.reset (sr, smoothSec);
        resoSmoother.reset   (sr, smoothSec);
        mixOsc1Smoother.reset  (sr, smoothSec);
        mixSub1Smoother.reset  (sr, smoothSec);
        mixOsc2Smoother.reset  (sr, smoothSec);
        mixSub2Smoother.reset  (sr, smoothSec);
        mixNoiseSmoother.reset (sr, smoothSec);
        freqSmoother.reset (sr, 0.0);
    }

    void setUnisonOffsets (float detuneCentsIn, float panIn)
    {
        unisonDetuneCents = detuneCentsIn;
        unisonPan = juce::jlimit (-1.0f, 1.0f, panIn);
    }

    bool canPlaySound (juce::SynthesiserSound* s) override { return dynamic_cast<JunoSound*> (s) != nullptr; }

    void controllerMoved (int controllerNumber, int newValue) override
    {
        if (controllerNumber == 1) modWheelValue = (float) newValue / 127.0f;
    }

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
        dcoPhase1 = 0.0f; dcoPhase2 = 0.0f;
        subPhase1 = 0.0f; subPhase2 = 0.0f;
        lfo1Phase = 0.0f; lfo2Phase = 0.0f; lfo3Phase = 0.0f;
        velocityGain = velocity;
        float keyPos = juce::jmap (juce::jlimit (55.0f, 1760.0f, baseFreq), 55.0f, 1760.0f, 0.0f, 1.0f);
        currentKeyPos = keyPos;
        float velScale1 = 1.0f - params.eg1VelocityMod->load() * (1.0f - velocity);
        float velScale2 = 1.0f - params.eg2VelocityMod->load() * (1.0f - velocity);
        float keyScale1 = 1.0f / (1.0f + params.eg1KeyTrack->load() * keyPos * 1.5f);
        float keyScale2 = 1.0f / (1.0f + params.eg2KeyTrack->load() * keyPos * 1.5f);
        juce::ADSR::Parameters eg1p {
            juce::jmax (0.001f, params.eg1Attack->load()  * velScale1),
            juce::jmax (0.001f, params.eg1Decay->load()   * keyScale1),
            params.eg1Sustain->load(),
            juce::jmax (0.001f, params.eg1Release->load() * velScale1)
        };
        eg1.setParameters (eg1p);
        eg1.noteOn();
        juce::ADSR::Parameters eg2p {
            juce::jmax (0.001f, params.eg2Attack->load()  * velScale2),
            juce::jmax (0.001f, params.eg2Decay->load()   * keyScale2),
            params.eg2Sustain->load(),
            juce::jmax (0.001f, params.eg2Release->load() * velScale2)
        };
        eg2.setParameters (eg2p);
        eg2.noteOn();
    }

    void stopNote (float, bool allowTailOff) override
    {
        eg1.noteOff();
        eg2.noteOff();
        if (! allowTailOff) clearCurrentNote();
    }

    void pitchWheelMoved (int) override {}

    static float polyBlep (float t, float dt)
    {
        if (t < dt)        { t /= dt; return t + t - t * t - 1.0f; }
        if (t > 1.0f - dt) { t = (t - 1.0f) / dt; return t * t + t + t + 1.0f; }
        return 0.0f;
    }

    static float generateOsc (float phase, float dt, int waveChoice, float pw)
    {
        if (waveChoice == 1) { return 1.0f - std::abs (4.0f * phase - 2.0f); }
        if (waveChoice == 2)
        {
            float pulsePhase = phase + pw; if (pulsePhase >= 1.0f) pulsePhase -= 1.0f;
            float pulse = phase < pw ? 1.0f : -1.0f;
            pulse += polyBlep (phase, dt) - polyBlep (pulsePhase, dt);
            return pulse;
        }
        return (2.0f * phase - 1.0f) - polyBlep (phase, dt);
    }

    static float computeLfo (float& phase, float& heldRandom, juce::Random& rng, float rateHz, int waveformChoice, float sr)
    {
        phase += rateHz / sr;
        bool wrapped = false;
        if (phase >= 1.0f) { phase -= 1.0f; wrapped = true; }
        switch (waveformChoice)
        {
            case 1:  return 2.0f * phase - 1.0f;
            case 2:  return 1.0f - 2.0f * phase;
            case 3:  return phase < 0.5f ? 1.0f : -1.0f;
            case 4:
                if (wrapped) heldRandom = rng.nextFloat() * 2.0f - 1.0f;
                return heldRandom;
            default: return 1.0f - std::abs (4.0f * phase - 2.0f);
        }
    }

    void renderNextBlock (juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override
    {
        if (! isVoiceActive()) return;
        const float sr = (float) sampleRate;
        cutoffSmoother.setTargetValue   (params.vcfCutoff->load());
        resoSmoother.setTargetValue     (params.vcfResonance->load());
        mixOsc1Smoother.setTargetValue  (params.mixOsc1->load());
        mixSub1Smoother.setTargetValue  (params.mixSub1->load());
        mixOsc2Smoother.setTargetValue  (params.mixOsc2->load());
        mixSub2Smoother.setTargetValue  (params.mixSub2->load());
        mixNoiseSmoother.setTargetValue (params.mixNoise->load());

        for (int i = 0; i < numSamples; ++i)
        {
            float eg1Val = eg1.getNextSample();
            float lfo1Val = computeLfo (lfo1Phase, lfo1Held, lfoRandom1, params.lfo1Rate->load(), (int) params.lfo1Wave->load(), sr);
            float lfo2Val = computeLfo (lfo2Phase, lfo2Held, lfoRandom2, params.lfo2Rate->load(), (int) params.lfo2Wave->load(), sr);

            lfo3Phase += params.lfo3Rate->load() / sr;
            if (lfo3Phase >= 1.0f) lfo3Phase -= 1.0f;
            float lfo3Tri = 1.0f - std::abs (4.0f * lfo3Phase - 2.0f);
            float vibratoSemis = lfo3Tri * modWheelValue * params.lfo3ModWheelAmount->load() * 1.0f;

            float smoothFreq = freqSmoother.getNextValue();

            float pitchMod1 = lfo1Val * params.osc1Lfo1Mod->load() + lfo2Val * params.osc1Lfo2Mod->load();
            float semis1 = params.osc1Tune->load() + pitchMod1 * 2.0f + vibratoSemis;
            float freq1 = smoothFreq * std::pow (2.0f, semis1 / 12.0f);
            float dt1 = freq1 / sr;

            float pitchMod2 = lfo1Val * params.osc2Lfo1Mod->load() + lfo2Val * params.osc2Lfo2Mod->load();
            float semis2 = params.osc2Tune->load() + pitchMod2 * 2.0f + vibratoSemis;
            float freq2 = smoothFreq * std::pow (2.0f, semis2 / 12.0f);
            float dt2 = freq2 / sr;

            float pw1 = 0.5f + 0.49f * (eg1Val * params.osc1Eg1Pwm->load() + lfo1Val * params.osc1Lfo1Pwm->load());
            pw1 = juce::jlimit (0.02f, 0.98f, pw1);
            float pw2 = 0.5f + 0.49f * (eg1Val * params.osc2Eg1Pwm->load() + lfo1Val * params.osc2Lfo1Pwm->load());
            pw2 = juce::jlimit (0.02f, 0.98f, pw2);

            dcoPhase1 += dt1;
            bool wrapped1 = false;
            if (dcoPhase1 >= 1.0f) { dcoPhase1 -= 1.0f; wrapped1 = true; }
            float osc1Sample = generateOsc (dcoPhase1, dt1, (int) params.osc1Wave->load(), pw1);

            subPhase1 += dt1 * 0.5f;
            if (subPhase1 >= 1.0f) subPhase1 -= 1.0f;
            float sub1Sample = subPhase1 < 0.5f ? 1.0f : -1.0f;

            dcoPhase2 += dt2;
            if (dcoPhase2 >= 1.0f) dcoPhase2 -= 1.0f;
            if (wrapped1 && params.osc2Sync->load() > 0.5f) dcoPhase2 = 0.0f;
            float osc2Sample = generateOsc (dcoPhase2, dt2, (int) params.osc2Wave->load(), pw2);

            subPhase2 += dt2 * 0.5f;
            if (subPhase2 >= 1.0f) subPhase2 -= 1.0f;
            float sub2Sample = subPhase2 < 0.5f ? 1.0f : -1.0f;

            float noiseSample = random.nextFloat() * 2.0f - 1.0f;

            float dcoMix = osc1Sample * mixOsc1Smoother.getNextValue()
                         + sub1Sample * mixSub1Smoother.getNextValue()
                         + osc2Sample * mixOsc2Smoother.getNextValue()
                         + sub2Sample * mixSub2Smoother.getNextValue()
                         + noiseSample * mixNoiseSmoother.getNextValue();

            dcoMix = applyHpf (dcoMix, (int) params.hpfCutoffStep->load());

            float keyTrackFilter = currentKeyPos * params.vcfKeyFollow->load();
            float cutoffBase = cutoffSmoother.getNextValue();
            float cutoffHz = cutoffBase
                            * std::pow (2.0f, params.vcfEg1Mod->load() * eg1Val * 5.0f
                                              + lfo1Val * params.vcfLfo1Mod->load() * 2.0f
                                              + lfo2Val * params.vcfLfo2Mod->load() * 2.0f
                                              + keyTrackFilter * 4.0f);
            cutoffHz = juce::jlimit (20.0f, sr * 0.45f, cutoffHz);
            float filtered = ladderFilter (dcoMix, cutoffHz, resoSmoother.getNextValue(), sr);

            float eg2Val = eg2.getNextSample();
            float ampEnvVal = params.vcaGateMode->load() > 0.5f ? (eg2.isActive() ? 1.0f : 0.0f) : eg2Val;

            float tremolo = 1.0f + (lfo1Val * params.ampLfo1Mod->load() + lfo2Val * params.ampLfo2Mod->load()) * 0.4f;
            tremolo = juce::jmax (0.0f, tremolo);

            float keyTrackAmp = 1.0f + (currentKeyPos - 0.5f) * params.ampKeyTrack->load() * 0.6f;
            keyTrackAmp = juce::jlimit (0.3f, 1.7f, keyTrackAmp);

            float velBlend = params.ampVelocityMod->load();
            float velGain = (1.0f - velBlend) + velBlend * velocityGain;

            float out = filtered * ampEnvVal * tremolo * keyTrackAmp * velGain * 0.3f;

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

            if (! eg2.isActive()) { clearCurrentNote(); break; }
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
    float currentKeyPos = 0.5f;
    float modWheelValue = 0.0f;
    float dcoPhase1 = 0.0f, dcoPhase2 = 0.0f;
    float subPhase1 = 0.0f, subPhase2 = 0.0f;
    float lfo1Phase = 0.0f, lfo2Phase = 0.0f, lfo3Phase = 0.0f;
    float lfo1Held = 0.0f, lfo2Held = 0.0f;
    float hpfPrevX = 0.0f, hpfPrevY = 0.0f;
    float filterStages[4] = { 0, 0, 0, 0 };
    float unisonDetuneCents = 0.0f, unisonPan = 0.0f;
    bool hasPlayedBefore = false;
    juce::ADSR eg1, eg2;
    juce::Random random, lfoRandom1, lfoRandom2;
    juce::SmoothedValue<float> freqSmoother { 440.0f };
    juce::SmoothedValue<float> cutoffSmoother, resoSmoother;
    juce::SmoothedValue<float> mixOsc1Smoother, mixSub1Smoother, mixOsc2Smoother, mixSub2Smoother, mixNoiseSmoother;
};
