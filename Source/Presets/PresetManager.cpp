#include "PresetManager.h"
#include "FactoryPresets.h"

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
