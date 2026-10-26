#pragma once
#include <juce_dsp/juce_dsp.h>

class JunoDelay
{
public:
    void prepare (const juce::dsp::ProcessSpec& spec)
    {
        sampleRate = spec.sampleRate;
        for (auto& d : delayLines) { d.prepare (spec); d.setMaximumDelayInSamples ((int) (2.0 * sampleRate)); }
    }
    void reset() { for (auto& d : delayLines) d.reset(); }
    void setParams (float timeSecIn, float feedbackIn, float mixIn)
    {
        timeSec  = juce::jlimit (0.01f, 2.0f, timeSecIn);
        feedback = juce::jlimit (0.0f, 0.95f, feedbackIn);
        mix      = juce::jlimit (0.0f, 1.0f, mixIn);
    }
    void process (juce::dsp::AudioBlock<float>& block)
    {
        if (mix <= 0.0001f) return;
        auto* left  = block.getChannelPointer (0);
        auto* right = block.getNumChannels() > 1 ? block.getChannelPointer (1) : left;
        float delaySamples = timeSec * (float) sampleRate;
        for (size_t i = 0; i < block.getNumSamples(); ++i)
        {
            float wetL = delayLines[0].popSample (0, delaySamples);
            float wetR = delayLines[1].popSample (0, delaySamples);
            float inL = left[i]  + wetL * feedback;
            float inR = right[i] + wetR * feedback;
            delayLines[0].pushSample (0, inL);
            delayLines[1].pushSample (0, inR);
            left[i]  = left[i]  * (1.0f - mix) + wetL * mix;
            right[i] = right[i] * (1.0f - mix) + wetR * mix;
        }
    }
private:
    double sampleRate = 44100.0;
    float timeSec = 0.3f, feedback = 0.3f, mix = 0.0f;
    std::array<juce::dsp::DelayLine<float>, 2> delayLines { juce::dsp::DelayLine<float> (96000), juce::dsp::DelayLine<float> (96000) };
};
