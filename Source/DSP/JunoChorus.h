#pragma once
#include <juce_dsp/juce_dsp.h>

class JunoChorus
{
public:
    void prepare (const juce::dsp::ProcessSpec& spec)
    {
        sampleRate = spec.sampleRate;
        for (auto& d : delays) { d.prepare (spec); d.setMaximumDelayInSamples ((int) (0.03 * sampleRate)); }
    }
    void reset() { for (auto& d : delays) d.reset(); lfoPhase = 0.0f; }
    void setParams (float rateHzIn, float depthIn, float mixIn)
    {
        rateHz = juce::jlimit (0.02f, 5.0f, rateHzIn);
        depth  = juce::jlimit (0.0f, 1.0f, depthIn);
        mix    = juce::jlimit (0.0f, 1.0f, mixIn);
    }
    void process (juce::dsp::AudioBlock<float>& block)
    {
        if (mix <= 0.0001f) return;
        const float baseMs  = 6.0f;
        const float depthMs = depth * 4.0f;
        auto* left  = block.getChannelPointer (0);
        auto* right = block.getNumChannels() > 1 ? block.getChannelPointer (1) : left;
        for (size_t i = 0; i < block.getNumSamples(); ++i)
        {
            lfoPhase += (float) (rateHz / sampleRate);
            if (lfoPhase >= 1.0f) lfoPhase -= 1.0f;
            float lfo = std::sin (lfoPhase * juce::MathConstants<float>::twoPi);
            float dL = (baseMs + depthMs * lfo) * 0.001f * (float) sampleRate;
            float dR = (baseMs - depthMs * lfo) * 0.001f * (float) sampleRate;
            float dryL = left[i];
            float dryR = right[i];
            delays[0].pushSample (0, dryL);
            delays[1].pushSample (0, dryR);
            float wetL = delays[0].popSample (0, dL);
            float wetR = delays[1].popSample (0, dR);
            left[i]  = dryL * (1.0f - mix) + wetL * mix;
            right[i] = dryR * (1.0f - mix) + wetR * mix;
        }
    }
private:
    double sampleRate = 44100.0;
    float lfoPhase = 0.0f;
    float rateHz = 0.5f, depth = 0.3f, mix = 0.0f;
    std::array<juce::dsp::DelayLine<float>, 2> delays { juce::dsp::DelayLine<float> (4410), juce::dsp::DelayLine<float> (4410) };
};
