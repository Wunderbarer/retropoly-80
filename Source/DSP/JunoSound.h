#pragma once
#include <juce_audio_basics/juce_audio_basics.h>

// Ein einziger "Sound", der auf alle Noten/Kanaele reagiert - wie beim Original,
// das keine Multisample-Zonen, sondern einen einzigen DCO-Signalpfad je Stimme hat.
class JunoSound : public juce::SynthesiserSound
{
public:
    bool appliesToNote (int) override    { return true; }
    bool appliesToChannel (int) override { return true; }
};
