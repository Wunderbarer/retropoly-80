#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "../PluginProcessor.h"

class VoiceLedBar : public juce::Component, private juce::Timer
{
public:
    explicit VoiceLedBar (JunoCloneAudioProcessor& p) : proc (p)
    {
        startTimerHz (15);
    }

    void paint (juce::Graphics& g) override
    {
        const int numLeds = 8;
        auto bounds = getLocalBounds().toFloat();
        float ledSize = juce::jmin (bounds.getHeight(), bounds.getWidth() / (float) numLeds) * 0.8f;
        float spacing = bounds.getWidth() / (float) numLeds;

        for (int i = 0; i < numLeds; ++i)
        {
            float cx = spacing * (i + 0.5f);
            float cy = bounds.getCentreY();
            auto led = juce::Rectangle<float> (ledSize, ledSize).withCentre ({ cx, cy });
            g.setColour (i < activeVoices ? juce::Colour (0xfff4a93c) : juce::Colours::black.withAlpha (0.5f));
            g.fillEllipse (led);
            g.setColour (juce::Colours::white.withAlpha (0.2f));
            g.drawEllipse (led, 1.0f);
        }
    }

private:
    void timerCallback() override
    {
        int newCount = proc.getActiveVoiceCount();
        if (newCount != activeVoices)
        {
            activeVoices = newCount;
            repaint();
        }
    }

    JunoCloneAudioProcessor& proc;
    int activeVoices = 0;
};
