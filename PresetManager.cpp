#include "PresetManager.h"
#include "FactoryPresets.h"
#include <vector>
#include <cmath>

// WICHTIG: Wir bauen das ValueTree NICHT von Hand zusammen (das interne Format
// von AudioProcessorValueTreeState ist ein Implementierungsdetail). Stattdessen
// setzen wir die echten Parameterwerte ueber die oeffentliche API und speichern
// danach den von APVTS selbst erzeugten, garantiert korrekten State-Baum.
// Der urspruengliche State wird danach wiederhergestellt.
void PresetManager::writeFactoryPresetsIfMissing()
{
    auto originalState = state.copyState().createCopy();

    for (auto& fp : getFactoryPresets())
    {
        auto file = presetFolder.getChildFile (fp.name + ".xml");
        if (file.existsAsFile())
            continue;

        for (auto& kv : fp.values)
            if (auto* param = state.getParameter (kv.first))
                param->setValueNotifyingHost (param->convertTo0to1 (kv.second));

        if (auto xml = state.copyState().createXml())
            xml->writeTo (file);
    }

    state.replaceState (originalState);
}

namespace
{
    float randRange (juce::Random& rng, float lo, float hi)
    {
        return lo + rng.nextFloat() * (hi - lo);
    }

    int randWeightedIndex (juce::Random& rng, const std::vector<float>& weights)
    {
        float total = 0.0f;
        for (auto w : weights) total += w;
        float r = rng.nextFloat() * total;
        float cum = 0.0f;
        for (int i = 0; i < (int) weights.size(); ++i)
        {
            cum += weights[i];
            if (r <= cum) return i;
        }
        return (int) weights.size() - 1;
    }

    // Kurz-mittel gewichtete Zeit: Produkt zweier Zufallszahlen verschiebt
    // die Verteilung Richtung kurzer Werte, laesst lange Werte aber weiterhin
    // (seltener) zu - passend fuer ADSR-Zeiten, die selten im Sekundenbereich
    // liegen sollen, aber gelegentlich duerfen.
    float shortBiasedTime (juce::Random& rng, float maxVal)
    {
        return rng.nextFloat() * rng.nextFloat() * maxVal;
    }
}

// --- Schritt: Zufalls-Patch-Generator ---
// Ein stumpf GLEICHVERTEILTER Zufallsklang ueber alle ~55 Parameter dieses
// erweiterten Synths (2 Oszillatoren, 2 Huellkurven, 3 LFOs, Filter, Amp,
// Chorus, Delay) waere in der grossen Mehrzahl der Faelle unbrauchbar: oft
// stumm, oft ein grelles Rausch-/Dissonanz-Chaos, oft dauerhaft verstimmt.
// Daher wird hier bewusst JEDER Parameter einzeln innerhalb eines von Hand
// erprobten, engeren und musikalisch sinnvollen Wertebereichs gewuerfelt.
void PresetManager::randomize()
{
    juce::Random rng;
    rng.setSeedRandomly();

    auto setParam = [this] (const juce::String& id, float realValue)
    {
        if (auto* p = state.getParameter (id))
            p->setValueNotifyingHost (p->convertTo0to1 (realValue));
    };

    // --- LFOs ---
    setParam ("lfo1Wave", (float) randWeightedIndex (rng, { 3.0f, 1.0f, 1.0f, 1.0f, 0.5f }));
    setParam ("lfo1Rate", randRange (rng, 0.3f, 7.0f));
    setParam ("lfo2Wave", (float) randWeightedIndex (rng, { 3.0f, 1.0f, 1.0f, 1.0f, 0.5f }));
    setParam ("lfo2Rate", randRange (rng, 0.1f, 2.0f));
    setParam ("lfo3Rate", randRange (rng, 3.0f, 7.0f));
    setParam ("lfo3ModWheelAmount", randRange (rng, 0.2f, 0.8f));

    // --- OSC 1: bleibt immer die klangbestimmende Hauptstimme auf der
    //     Grundtonhoehe, sonst klingt praktisch jeder Zufallsklang verstimmt.
    setParam ("osc1Wave", (float) randWeightedIndex (rng, { 3.0f, 1.0f, 2.0f }));
    setParam ("osc1Tune", 0.0f);
    setParam ("osc1Lfo1Mod", randRange (rng, 0.0f, 0.15f));
    setParam ("osc1Lfo2Mod", randRange (rng, 0.0f, 0.1f));
    setParam ("osc1Eg1Pwm",  randRange (rng, 0.0f, 0.3f));
    setParam ("osc1Lfo1Pwm", randRange (rng, 0.0f, 0.3f));

    // --- OSC 2: haeufig aktiv fuer Dicke/Intervalle, manchmal ganz stumm.
    //     Statt wilder Verstimmung werden typische musikalische Intervalle
    //     gewuerfelt (unisono, Oktave, Quinte, leichtes Detune) - das macht
    //     den Zufallsklang sofort "richtig" statt schief.
    bool osc2Active = rng.nextFloat() < 0.75f;
    setParam ("osc2Wave", (float) randWeightedIndex (rng, { 3.0f, 1.0f, 2.0f }));
    static const float intervals[] = { 0.0f, 0.0f, -12.0f, 12.0f, 7.0f, -0.08f, 0.08f };
    setParam ("osc2Tune", intervals[randWeightedIndex (rng, { 3.0f, 2.0f, 1.5f, 1.5f, 1.0f, 1.0f, 1.0f })]);
    setParam ("osc2Lfo1Mod", randRange (rng, 0.0f, 0.1f));
    setParam ("osc2Lfo2Mod", randRange (rng, 0.0f, 0.1f));
    setParam ("osc2Eg1Pwm",  randRange (rng, 0.0f, 0.3f));
    setParam ("osc2Lfo1Pwm", randRange (rng, 0.0f, 0.3f));
    setParam ("osc2Sync", rng.nextFloat() < 0.12f ? 1.0f : 0.0f); // selten, sehr aggressiv

    // --- Mixer ---
    setParam ("mixOsc1", randRange (rng, 0.6f, 1.0f));
    setParam ("mixSub1", randRange (rng, 0.0f, 0.35f));
    setParam ("mixOsc2", osc2Active ? randRange (rng, 0.4f, 0.9f) : 0.0f);
    setParam ("mixSub2", randRange (rng, 0.0f, 0.2f));
    setParam ("mixNoise", randRange (rng, 0.0f, 0.1f));
    setParam ("hpfStep", (float) randWeightedIndex (rng, { 3.0f, 2.0f, 1.0f, 0.3f }));

    // --- Filter: Cutoff auf gehoerrichtiger (logarithmischer) Skala
    //     gewuerfelt - sonst landen >90% der Zufallswerte im duennen
    //     Hochtonbereich. Resonanz bleibt unterhalb der Selbstoszillation.
    float cutoffOct = randRange (rng, std::log2 (200.0f), std::log2 (6000.0f));
    setParam ("vcfCutoff", std::pow (2.0f, cutoffOct));
    setParam ("vcfResonance", randRange (rng, 0.0f, 0.55f));
    setParam ("vcfKeyFollow", randRange (rng, 0.2f, 0.8f));
    setParam ("vcfEg1Mod", randRange (rng, -0.2f, 0.8f));
    setParam ("vcfLfo1Mod", randRange (rng, 0.0f, 0.3f));
    setParam ("vcfLfo2Mod", randRange (rng, 0.0f, 0.2f));

    // --- Verstaerker ---
    setParam ("ampLfo1Mod", randRange (rng, 0.0f, 0.15f));
    setParam ("ampLfo2Mod", randRange (rng, 0.0f, 0.1f));
    setParam ("ampKeyTrack", randRange (rng, 0.0f, 0.4f));
    setParam ("ampVelocityMod", randRange (rng, 0.4f, 0.9f));
    setParam ("vcaGateMode", rng.nextFloat() < 0.85f ? 0.0f : 1.0f);

    // --- Huellkurven: Zeiten quadratisch (kurz-mittel) gewichtet gewuerfelt,
    //     EG1 steuert das Filter, EG2 die Lautstaerke.
    setParam ("eg1Attack",  0.001f + shortBiasedTime (rng, 1.0f));
    setParam ("eg1Decay",   0.05f  + shortBiasedTime (rng, 1.2f));
    setParam ("eg1Sustain", randRange (rng, 0.2f, 0.9f));
    setParam ("eg1Release", 0.05f  + shortBiasedTime (rng, 1.2f));
    setParam ("eg1VelocityMod", randRange (rng, 0.0f, 0.4f));
    setParam ("eg1KeyTrack",    randRange (rng, 0.0f, 0.3f));

    setParam ("eg2Attack",  0.001f + shortBiasedTime (rng, 0.8f));
    setParam ("eg2Decay",   0.05f  + shortBiasedTime (rng, 1.0f));
    setParam ("eg2Sustain", randRange (rng, 0.3f, 1.0f));
    setParam ("eg2Release", 0.05f  + shortBiasedTime (rng, 1.2f));
    setParam ("eg2VelocityMod", randRange (rng, 0.0f, 0.4f));
    setParam ("eg2KeyTrack",    randRange (rng, 0.0f, 0.3f));

    // --- Unisono / Glide ---
    setParam ("unisonVoices", (float) randWeightedIndex (rng, { 3.0f, 3.0f, 1.0f, 0.5f }));
    setParam ("unisonDetune", randRange (rng, 0.0f, 18.0f));
    setParam ("glideTime", rng.nextFloat() < 0.8f ? 0.0f : randRange (rng, 0.02f, 0.25f));

    // --- Chorus: moderate, meist angenehme Werte ---
    setParam ("chorusRate", randRange (rng, 0.2f, 1.2f));
    setParam ("chorusDepth", randRange (rng, 0.15f, 0.6f));
    setParam ("chorusMix", rng.nextFloat() < 0.7f ? randRange (rng, 0.15f, 0.6f) : 0.0f);

    // --- Delay: oft dezent, manchmal komplett aus ---
    setParam ("delayTime", randRange (rng, 0.1f, 0.6f));
    setParam ("delayFeedback", randRange (rng, 0.0f, 0.45f));
    setParam ("delayMix", rng.nextFloat() < 0.5f ? randRange (rng, 0.05f, 0.3f) : 0.0f);

    // --- Lautstaerke ---
    setParam ("masterVolume", randRange (rng, 0.85f, 1.1f));

    // Oversampling bewusst NICHT randomisieren - reine CPU-/Qualitaetswahl,
    // kein Klangparameter.

    currentIndex = -1;
    currentName = "Random";
}
