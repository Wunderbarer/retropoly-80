#pragma once
#include <juce_audio_basics/juce_audio_basics.h>
#include "JunoVoice.h"

// --- Unisono + Voice-Stealing ---
// juce::Synthesiser startet pro Note normalerweise genau EINE freie Stimme.
// Fuer "fetten" Unisono-Modus wird noteOn() ueberschrieben und gezielt mehrfach
// eine freie Stimme gesucht (findFreeVoice) und gestartet (startVoice).
// Voice-Stealing wird zusaetzlich global aktiviert via setNoteStealingEnabled(true).
class UnisonSynthesiser : public juce::Synthesiser
{
public:
    void setUnisonParams (std::atomic<float>* voicesParam, std::atomic<float>* detuneParam)
    {
        unisonVoicesParam = voicesParam;
        unisonDetuneParam = detuneParam;
    }

    void noteOn (int midiChannel, int midiNoteNumber, float velocity) override
    {
        const juce::ScopedLock sl (lock);

        int voiceCount = 1;
        float detuneCents = 0.0f;

        if (unisonVoicesParam != nullptr)
            voiceCount = juce::jlimit (1, 4, (int) unisonVoicesParam->load() + 1);
        if (unisonDetuneParam != nullptr)
            detuneCents = unisonDetuneParam->load();

        static const float spread1[1] = { 0.0f };
        static const float spread2[2] = { -1.0f, 1.0f };
        static const float spread3[3] = { -1.0f, 0.0f, 1.0f };
        static const float spread4[4] = { -1.0f, -0.33f, 0.33f, 1.0f };
        const float* spread = voiceCount == 1 ? spread1
                             : voiceCount == 2 ? spread2
                             : voiceCount == 3 ? spread3 : spread4;

        for (auto* sound : sounds)
        {
            if (sound->appliesToNote (midiNoteNumber) && sound->appliesToChannel (midiChannel))
            {
                for (int i = 0; i < voiceCount; ++i)
                {
                    auto* voice = findFreeVoice (sound, midiChannel, midiNoteNumber, isNoteStealingEnabled());
                    if (voice == nullptr)
                        continue;

                    if (auto* jv = dynamic_cast<JunoVoice*> (voice))
                        jv->setUnisonOffsets (spread[i] * detuneCents, spread[i]);

                    startVoice (voice, sound, midiChannel, midiNoteNumber, velocity);
                }
            }
        }
    }

private:
    std::atomic<float>* unisonVoicesParam = nullptr;
    std::atomic<float>* unisonDetuneParam = nullptr;
};
