#pragma once
#include <juce_dsp/juce_dsp.h>

// Nachbildung des Juno-Ensemble-Chorus: zwei gegenphasig modulierte, hart
// gepannte Delay-Lines statt eines generischen digitalen Chorus-Algorithmus.
class JunoChorus
{
public:
    void prepare (const juce::dsp::ProcessSpec& spec)
    {
        sampleRate = spec.sampleRate;
        for (auto& d : delays)
        {
            d.prepare (spec);
            d.setMaximumDelayInSamples ((int) (0.02 * sampleRate));
        }
    }

    void reset()
    {
        for (auto& d : delays) d.reset();
        lfoPhase = 0.0f;
    }

    // 0 = Off, 1 = Chorus I, 2 = Chorus II, 3 = Chorus I+II
    void setMode (int m) { mode = m; }

    void process (juce::dsp::AudioBlock<float>& block)
    {
        if (mode == 0) return;

        float rateHz, depthMs;
        if (mode == 2)      { rateHz = 0.83f;  depthMs = 0.006f; }
        else if (mode == 3) { rateHz = 0.65f;  depthMs = 0.009f; }
        else                { rateHz = 0.513f; depthMs = 0.003f; }

        const float baseMs = 5.0f;

        auto* left  = block.getChannelPointer (0);
        auto* right = block.getNumChannels() > 1 ? block.getChannelPointer (1) : left;

        for (size_t i = 0; i < block.getNumSamples(); ++i)
        {
            lfoPhase += (float) (rateHz / sampleRate);
            if (lfoPhase >= 1.0f) lfoPhase -= 1.0f;
            float lfo = std::sin (lfoPhase * juce::MathConstants<float>::twoPi);

            float dL = (baseMs + depthMs * lfo) * 0.001f * (float) sampleRate;
            float dR = (baseMs - depthMs * lfo) * 0.001f * (float) sampleRate;

            float dry = left[i];
            delays[0].pushSample (0, dry);
            delays[1].pushSample (0, right[i]);
            float wetL = delays[0].popSample (0, dL);
            float wetR = delays[1].popSample (0, dR);

            left[i]  = 0.5f * (dry       + wetL);
            right[i] = 0.5f * (right[i]  + wetR);
        }
    }

private:
    int mode = 0;
    double sampleRate = 44100.0;
    float lfoPhase = 0.0f;
    std::array<juce::dsp::DelayLine<float>, 2> delays { juce::dsp::DelayLine<float> (4410), juce::dsp::DelayLine<float> (4410) };
};
