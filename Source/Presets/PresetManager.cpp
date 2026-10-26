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
    // Zufallszahl gleichverteilt zwischen lo und hi.
    float randRange (juce::Random& rng, float lo, float hi)
    {
        return lo + rng.nextFloat() * (hi - lo);
    }

    // Wuerfelt einen Index aus mehreren Optionen, gewichtet nach "weights"
    // (hoeheres Gewicht = wahrscheinlicher). Fuer Choice-Parameter und
    // "meistens so, aber nicht immer"-Entscheidungen.
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
}

// --- Schritt: Zufalls-Patch-Generator ---
// Ein stumpf GLEICHVERTEILTER Zufallsklang ueber alle Parameter hinweg klaenge
// in der grossen Mehrzahl der Faelle unbrauchbar: oft komplett stumm (wenn
// zufaellig alle Oszillator-Level nahe 0 landen), oft ein grelles Rausch-Chaos
// (hohe Resonanz + viel Noise + hoher Cutoff gleichzeitig) oder staendig leicht
// verstimmt (zu viel Vibrato/PWM-Modulation). Daher wird hier bewusst JEDER
// Parameter einzeln innerhalb eines von Hand erprobten, engeren Wertebereichs
// gewuerfelt, der fuer sich genommen bereits "brauchbar" klingt - zusammen
// ergibt das praktisch immer einen spielbaren, musikalisch sinnvollen Klang.
void PresetManager::randomize()
{
    juce::Random rng;
    rng.setSeedRandomly();

    auto setParam = [this] (const juce::String& id, float realValue)
    {
        if (auto* p = state.getParameter (id))
            p->setValueNotifyingHost (p->convertTo0to1 (realValue));
    };

    // --- DCO-Mischung ---
    // Zufaellig entweder Saegezahn- oder Pulswelle als dominante Hauptfarbe,
    // damit IMMER mindestens eine deutlich hoerbare Wellenform da ist.
    bool sawDominant = rng.nextFloat() < 0.6f;
    setParam ("sawLevel",   sawDominant ? randRange (rng, 0.5f, 1.0f) : randRange (rng, 0.0f, 0.4f));
    setParam ("pulseLevel", sawDominant ? randRange (rng, 0.0f, 0.4f) : randRange (rng, 0.5f, 1.0f));
    setParam ("subLevel",   randRange (rng, 0.0f, 0.5f));
    setParam ("noiseLevel", randRange (rng, 0.0f, 0.15f));          // dezent, sonst dominiert reines Rauschen
    setParam ("pwmAmount",    randRange (rng, -0.3f, 0.3f));
    setParam ("pwmLfoAmount", randRange (rng, 0.0f, 0.4f));
    setParam ("hpfStep", (float) randWeightedIndex (rng, { 3.0f, 2.0f, 1.0f, 0.3f })); // meist wenig Bassbeschnitt

    // --- Filter ---
    // Cutoff auf einer gehoerrichtigen (logarithmischen) Skala wuerfeln statt
    // linear in Hz - sonst landen >90% der Zufallswerte im duennen Hochtonbereich.
    float cutoffOct = randRange (rng, std::log2 (200.0f), std::log2 (6000.0f));
    setParam ("vcfCutoff", std::pow (2.0f, cutoffOct));
    setParam ("vcfResonance",  randRange (rng, 0.0f, 0.55f));   // bleibt unterhalb der Selbstoszillation
    setParam ("vcfEnvAmount",  randRange (rng, -0.3f, 0.8f));
    setParam ("vcfKeyFollow",  randRange (rng, 0.2f, 0.8f));

    // --- LFO ---
    setParam ("lfoRate",  randRange (rng, 0.5f, 7.0f));
    setParam ("lfoDelay", randRange (rng, 0.0f, 1.2f));
    setParam ("lfoToVcf", randRange (rng, 0.0f, 0.35f));
    setParam ("lfoToDco", randRange (rng, 0.0f, 0.15f));        // wenig Vibrato, sonst wirkt es verstimmt

    // --- Huellkurve (ADSR) ---
    // Attack/Decay/Release quadratisch statt linear verteilt wuerfeln: das
    // verschiebt die Mehrzahl der Zufallswerte in Richtung "kurz bis mittel"
    // und laesst lange Werte seltener, aber weiterhin moeglich auftreten.
    auto shortBiasedTime = [&] (float maxVal) { return rng.nextFloat() * rng.nextFloat() * maxVal; };
    setParam ("attack",  0.001f + shortBiasedTime (1.2f));
    setParam ("decay",   0.05f  + shortBiasedTime (1.2f));
    setParam ("sustain", randRange (rng, 0.3f, 1.0f));
    setParam ("release", 0.05f  + shortBiasedTime (1.2f));
    setParam ("vcaGateMode", rng.nextFloat() < 0.8f ? 0.0f : 1.0f);  // meist normale Huellkurve, selten Gate

    // --- Chorus / Unisono ---
    setParam ("chorusMode",   (float) randWeightedIndex (rng, { 1.0f, 2.0f, 2.0f, 1.0f }));
    setParam ("unisonVoices", (float) randWeightedIndex (rng, { 3.0f, 3.0f, 1.0f, 0.5f })); // meist 1-2 Stimmen
    setParam ("unisonDetune", randRange (rng, 0.0f, 18.0f));
    setParam ("glideTime", rng.nextFloat() < 0.8f ? 0.0f : randRange (rng, 0.02f, 0.25f));

    // Oversampling-Qualitaet bewusst NICHT wuerfeln - das ist eine reine
    // CPU/Render-Qualitaets-Einstellung, kein Klangparameter.

    currentIndex = -1;
    currentName = "Random";
}
