# RetroPoly-80 — Projektdokumentation (finale, zusammengefuehrte Version)

Ein eigenstaendiger Software-Synthesizer im Klangcharakter der legendaeren
Roland-Analogsynthesizer der 80er-Jahre, gebaut mit dem JUCE-Framework.
Laeuft als **VST3-Plugin** (z. B. in FL Studio) und als **eigenstaendige
Standalone-Anwendung**.

Dieses Paket fuehrt ALLE bisherigen Entwicklungsschritte aus zwei zuvor
getrennten Versionslinien in einem einzigen, vollstaendigen Projekt zusammen.

## Enthaltene Funktionen

**Klangerzeugung**
- **2 Oszillatoren** (OSC1 + OSC2) mit je Saw/Dreieck/Puls, eigener
  Stimmung, PWM und Sync-Option fuer OSC2
- **2 Sub-Oszillatoren**, Rauschgenerator, HPF mit 4 Stufen
- **3 LFOs**: LFO1 + LFO2 frei zur Modulation von Tonhoehe/PWM/Filter/Amp,
  LFO3 als klassisches Modulationsrad-Vibrato
- **2 Huellkurven**: EG1 steuert das Filter, EG2 die Lautstaerke, je mit
  Velocity- und Keyboard-Tracking
- Resonantes 24dB-Ladder-Filter mit weicher Saettigung (analoger Charakter)
- **Chorus** (Rate/Depth/Mix) und **Delay** (Time/Feedback/Mix) als echte
  Regler direkt im Panel editierbar
- **16 gleichzeitige Stimmen** (Polyphonie), Unisono-Modus (1-4 Stimmen)mit
  Voice-Stealing, Portamento/Glide, bis zu 2-fachem Oversampling

**Bedienung**
- **Kreisrunde Potis** mit gut sichtbarem, langem Zeiger (garantiert rund
  unter allen Layout-Bedingungen, nicht oval)
- **Gut lesbare, groessere Beschriftungen** aller Regler, Gruppen und
  Buttons
- Eigener, zuverlaessiger Speichern-Dialog (ersetzt das fehleranfaellige
  native Fenster)
- Live-Anzeige der aktiven Stimmen als LED-Leiste (16 LEDs)
- **"Random"-Button**: erzeugt per Klick einen kompletten, musikalisch
  sinnvollen Zufallsklang (siehe unten)
- Preset-Browser mit sechs Werks-Presets, Vor/Zurueck-Navigation

## Der Zufalls-Patch-Generator ("Random")

Ein stumpf gleichverteilter Zufallsgenerator ueber alle ~55 Parameter waere
in der Praxis fast immer unbrauchbar (stumm, dissonant oder staendig
verstimmt). Der "Random"-Button in der Preset-Leiste wuerfelt deshalb jeden
Parameter einzeln innerhalb eng gefasster, erprobter Bereiche:
- OSC1 bleibt immer die tonale Referenz; OSC2 bekommt musikalische
  Intervalle (Oktave, Quinte, Unisono, leichtes Detune) statt wilder
  Verstimmung
- Filter-Cutoff wird auf einer gehoerrichtigen (logarithmischen) Skala
  gewuerfelt, Resonanz bleibt unterhalb der Selbstoszillation
- Huellkurvenzeiten sind quadratisch gewichtet (meist kurz-mittel, selten
  sehr lang)
- Chorus/Delay bleiben dezent bis moderat, Oversampling wird nicht
  randomisiert (reine Qualitaetseinstellung, kein Klangparameter)

## Projektstruktur

```
RetroPoly-80/
├── CMakeLists.txt
├── Source/
│   ├── PluginProcessor.h / .cpp
│   ├── PluginEditor.h / .cpp
│   ├── DSP/
│   │   ├── JunoSound.h
│   │   ├── JunoVoice.h            (2 OSCs, 2 EGs, 3 LFOs)
│   │   ├── UnisonSynthesiser.h
│   │   ├── JunoChorus.h
│   │   └── JunoDelay.h
│   ├── Presets/
│   │   ├── PresetManager.h / .cpp (inkl. randomize())
│   │   └── FactoryPresets.h
│   └── GUI/
│       ├── JunoLookAndFeel.h      (kreisrunde Potis, lesbare Schrift)
│       ├── PresetBar.h / .cpp     (inkl. Random-Button)
│       ├── SavePresetOverlay.h / .cpp
│       └── VoiceLedBar.h
└── installer/
    └── installer.iss
```

## Build-Anleitung (Windows)

### 1. Werkzeuge installieren
- Visual Studio 2022 Community mit Workload **"Desktopentwicklung mit C++"**
- [JUCE](https://juce.com) als Git-Submodule:
  `git submodule add https://github.com/juce-framework/JUCE`
- [CMake](https://cmake.org) ≥ 3.22
- [Inno Setup](https://jrsoftware.org/isinfo.php) fuer den finalen Installer

### 2. Projekt bauen
```
cmake -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
```
Ergebnis:
- `build/RetroPoly80_artefacts/Release/VST3/RetroPoly-80.vst3`
- `build/RetroPoly80_artefacts/Release/Standalone/RetroPoly-80.exe`

### 3. Installer bauen
```
cd installer
iscc installer.iss
```
Ergebnis: `installer/Output/RetroPoly-80_Setup.exe` — installiert sowohl die
VST3-Version (in den System-VST3-Ordner) als auch die Standalone-exe.

### 4. Presets zuruecksetzen (empfohlen bei diesem Update)
Da sich der Parametersatz komplett geaendert hat (2 Oszillatoren, 2 EGs,
3 LFOs statt der alten einfachen Version), bitte vor dem ersten Start den
alten Presets-Ordner einmalig loeschen, damit die 6 neuen Werks-Presets
sauber neu angelegt werden:
`%AppData%\RetroPoly-80\Presets`

## Rechtlicher Hinweis

Dieses Projekt bildet den **Klangcharakter** (Oszillator-Wellenformen,
Chorus-Prinzip, Filterverhalten) algorithmisch nach und orientiert sich bei
den **Bedienelement-Typen** (Drehpotis, Schieberegler) am Original —
verwendet aber einen eigenen Produktnamen und ein eigenstaendiges
Panel-Design, um keine Marken- oder Designrechte zu verletzen.
