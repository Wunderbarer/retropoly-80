# RetroPoly-80 — Projektdokumentation

Ein eigenstaendiger Software-Synthesizer im Klangcharakter der legendaeren
Roland-Analogsynthesizer der 80er-Jahre (DCO + Chorus + resonantes 24dB-Filter),
gebaut mit dem JUCE-Framework. Laeuft als **VST3-Plugin** (z.B. in FL Studio)
und als **eigenstaendige Standalone-Anwendung**.

## Enthaltene Erweiterungsschritte (in dieser Reihenfolge umgesetzt)

1. **Parameter-Smoothing** — `juce::SmoothedValue` in `JunoVoice.h` fuer Cutoff,
   Resonanz, Oszillator-Level und PWM. Verhindert Knack-/Zipper-Geraeusche bei
   schnellen Reglerbewegungen in der DAW.
2. **Voice-Stealing** — `synth.setNoteStealingEnabled(true)` im Processor:
   Wenn alle 8 Stimmen belegt sind, wird eine alte Stimme uebernommen statt
   neue Noten zu verschlucken.
3. **Unisono-Modus** — `UnisonSynthesiser.h` startet pro Note 1-4 Stimmen mit
   einstellbarer Verstimmung (Detune) und Stereo-Panorama fuer einen fetteren,
   breiteren Klang (typisch fuer Pad-/Lead-Sounds).
4. **Portamento/Glide** — in `JunoVoice::startNote()`: beim erneuten Anschlagen
   einer bereits aktiven Stimme wird die Frequenz ueber die eingestellte
   Glide-Zeit sanft nachgezogen statt sofort zu springen.
5. **Preset-Browser** — `PresetManager` + `PresetBar`: 6 handgebaute
   Werks-Presets (Init, Pad, Bass, Strings, Brass, Lead), Speichern eigener
   Presets per Dialog, Vor-/Zurueck-Navigation. Presets liegen als einzelne
   .xml-Dateien in `%AppData%/RetroPoly-80/Presets`.
6. **Oversampling** — `juce::dsp::Oversampling` im Processor: Im Modus
   "High Quality" wird die komplette DCO/Filter-Berechnung intern bei doppelter
   Samplerate ausgefuehrt und sauber heruntergefiltert — reduziert digitales
   Aliasing, besonders bei hoher Filter-Resonanz.
7. **Verfeinertes Filtermodell** — `ladderFilter()` in `JunoVoice.h`: sanfte
   `tanh()`-Saettigung pro Filterpol simuliert die Nichtlinearitaet eines
   analogen Transistor-Ladder-Filterchips, statt eines "cleanen" digitalen
   State-Variable-Filters. Resonanz bleibt bewusst "weich" statt aggressiv
   selbstschwingend (typisch fuer den Original-Klangcharakter).
8. **Eigenstaendige Optik** — `JunoLookAndFeel.h`: dunkles Panel, orange
   Akzentfarbe, handgezeichnete Drehpotis (mit Skalenstrichen + Zeiger) und
   Schieberegler (Schlitz + Kappe) — die gleichen zwei Bedienelement-Typen wie
   beim Original, aber eigenstaendiges Design ohne Markenlogo/-namen.

## Projektstruktur

```
JunoClone/
├── CMakeLists.txt
├── Source/
│   ├── PluginProcessor.h / .cpp
│   ├── PluginEditor.h / .cpp
│   ├── DSP/
│   │   ├── JunoSound.h
│   │   ├── JunoVoice.h
│   │   ├── UnisonSynthesiser.h
│   │   └── JunoChorus.h
│   ├── Presets/
│   │   ├── PresetManager.h / .cpp
│   │   └── FactoryPresets.h
│   └── GUI/
│       ├── JunoLookAndFeel.h
│       └── PresetBar.h / .cpp
└── installer/
    └── installer.iss
```

## Build-Anleitung (Windows)

### 1. Werkzeuge installieren
- Visual Studio 2022 Community mit Workload **"Desktopentwicklung mit C++"**
- [JUCE](https://juce.com) (aktuelle Version) oder als Git-Submodule:
  ```
  git submodule add https://github.com/juce-framework/JUCE
  ```
- [CMake](https://cmake.org) ≥ 3.22 (meist schon in VS enthalten)
- [Inno Setup](https://jrsoftware.org/isinfo.php) fuer den finalen Installer

### 2. Projekt konfigurieren und bauen
```
cmake -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
```
Ergebnis:
- `build/RetroPoly80_artefacts/Release/VST3/RetroPoly-80.vst3`
- `build/RetroPoly80_artefacts/Release/Standalone/RetroPoly-80.exe`

### 3. In FL Studio testen
1. VST3-Ordner aus dem Build-Verzeichnis nach
   `C:\Program Files\Common Files\VST3\` kopieren (oder direkt den Installer
   nutzen, siehe Schritt 4).
2. FL Studio → Options → Manage Plugins → "Find Plugins" / Scan ausfuehren.
3. RetroPoly-80 erscheint in der Plugin-Datenbank unter Generators.

### 4. Installer bauen
```
cd installer
iscc installer.iss
```
Ergebnis: `installer/Output/RetroPoly-80_Setup.exe` — installiert automatisch
sowohl die VST3-Version (in den System-VST3-Ordner) als auch die
Standalone-exe (mit Desktop-/Startmenue-Verknuepfung).

### 5. Code-Signierung (empfohlen, aber optional)
Ohne digitale Signatur zeigt Windows SmartScreen beim ersten Start eine
Warnung ("Unbekannter Herausgeber"). Fuer eine saubere, warnungsfreie
Installation:
1. Ein **Code-Signing-Zertifikat** bei einer Zertifizierungsstelle erwerben
   (z.B. DigiCert, Sectigo, oder ein guenstigeres "OV"-Zertifikat).
2. Die fertige `RetroPoly-80_Setup.exe` signieren:
   ```
   signtool sign /f meinZertifikat.pfx /p PASSWORT /fd sha256 /tr http://timestamp.digicert.com /td sha256 RetroPoly-80_Setup.exe
   ```
3. Optional zusaetzlich die VST3- und Standalone-exe VOR dem Verpacken im
   Installer separat signieren.

## Bekannte Einschraenkungen / naechste sinnvolle Schritte

- Die `UnisonSynthesiser`-Klasse nutzt `protected`-Member von
  `juce::Synthesiser` (`sounds`, `lock`, `findFreeVoice`, `startVoice`).
  Die genauen Namen koennen sich zwischen JUCE-Hauptversionen leicht
  unterscheiden — bei Compile-Fehlern bitte gegen die installierte
  `juce_Synthesiser.h` abgleichen.
- Portamento ist pro Stimme implementiert (nicht global monophon wie bei
  manchen Original-Haerdware-Modi) — fuer die meisten musikalischen Zwecke
  klingt das sehr aehnlich, ist aber technisch eine Vereinfachung.
- Factory-Presets werden beim allerersten Start einmalig ueber die echte
  Processor-Parameter-API erzeugt und als .xml gespeichert (siehe
  `PresetManager::writeFactoryPresetsIfMissing`) — dafuer muss der Prozessor
  bereits mit allen Parametern initialisiert sein, was im Konstruktor der
  Fall ist.
- Fuer eine marktreife Version: eigenes Grafik-Artwork (SVG/PNG statt
  programmatisch gezeichneter Regler), AU-Format fuer macOS, automatisierte
  Tests, sowie eine feinere messtechnische Kalibrierung des Filtermodells.

## Rechtlicher Hinweis

Dieses Projekt bildet den **Klangcharakter** (DCO-Wellenformen, Chorus-Prinzip,
Filterverhalten) algorithmisch nach und orientiert sich bei den
**Bedienelement-Typen** (Drehpotis, Schieberegler) am Original — verwendet
aber einen eigenen Produktnamen und ein eigenstaendiges Panel-Design, um keine
Marken- oder Designrechte zu verletzen.
