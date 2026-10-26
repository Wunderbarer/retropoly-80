# RetroPoly-80 - Update-Paket

Dieses Paket enthaelt alle Dateien fuer das Update:
- Neue Optik (Drehregler/Buttons im Stil des Referenzbilds)
- Zweiter Oszillator (OSC1 + OSC2) mit allen zugehoerigen Modulationsmoeglichkeiten
- Zwei Huellkurven (EG1 fuers Filter, EG2 fuer die Lautstaerke), drei LFOs, Chorus + neuer Delay-Effekt
- Behobener Speichern-Fehler (eigenes Dialogfeld statt des fehlerhaften nativen Fensters)

## WICHTIG vor dem Hochladen

Dieses Paket enthaelt NUR die geaenderten bzw. neuen Dateien. Folgende Dateien
aus deinem bestehenden Projekt bleiben unveraendert und sind NICHT enthalten:
- Source/DSP/JunoSound.h
- Source/DSP/UnisonSynthesiser.h
- Source/Presets/PresetManager.h
- Source/Presets/PresetManager.cpp

Bitte lasse diese vier Dateien in deinem Repository unangetastet. Falls deine
bestehende PresetManager-Klasse noch keine Methode "savePresetAs(juce::String)"
besitzt, melde dich kurz - dann liefere ich dir die passende Ergaenzung dafuer.

## So laedst du die Dateien bei GitHub hoch (im Browser)

1. Entpacke dieses ZIP auf deinem Rechner. Du bekommst einen Ordner
   "RetroPoly80_Update" mit der gleichen Ordnerstruktur wie dein Projekt
   (Source/DSP, Source/GUI, Source/Presets, CMakeLists.txt).

2. Gehe in deinem GitHub-Repository in den Hauptordner (Root).

3. Klicke auf "Add file" -> "Upload files".

4. Ziehe mit der Maus den KOMPLETTEN Inhalt des entpackten Ordners
   "RetroPoly80_Update" (also die Unterordner Source/ und die Datei
   CMakeLists.txt) in das Upload-Fenster im Browser.
   Moderne Browser (Chrome, Edge) uebernehmen dabei automatisch die
   Ordnerstruktur. Falls das bei dir nicht klappt, lade die Dateien
   einzeln ueber den jeweils passenden Unterordner hoch (siehe Abschnitt
   "Manuell, Datei fuer Datei" unten).

5. GitHub fragt bei bereits vorhandenen Dateien automatisch, ob sie
   ueberschrieben werden sollen -> einfach bestaetigen. Neue Dateien
   (z. B. JunoDelay.h, SavePresetOverlay.h/.cpp, VoiceLedBar.h) werden
   automatisch angelegt.

6. Unten bei "Commit changes" auf "Commit changes" klicken, um alles in
   einem Schritt zu speichern.

## Manuell, Datei fuer Datei (falls Drag&Drop des ganzen Ordners nicht geht)

Gehe fuer jede Datei in den passenden Ordner im Repository und:
- Bei bereits vorhandenen Dateien: Stift-Symbol ("Edit this file") ->
  kompletten Inhalt ersetzen -> Commit.
- Bei neuen Dateien: "Add file" -> "Create new file" -> Pfad+Dateiname
  oben eintragen -> Inhalt einfuegen -> "Commit new file".

## Nach dem Hochladen

1. Gehe auf den Reiter "Actions" in GitHub und warte, bis der Build-Lauf
   fertig ist (gruenes Haekchen).
2. Lade dir dort die gebaute VST3-Datei als "Artifact" herunter.
3. Loesche einmalig deinen alten Presets-Ordner lokal
   (z. B. unter %AppData%\RetroPoly-80\Presets), damit die 6 neuen
   Werks-Presets beim ersten Start sauber neu angelegt werden.

Bei Fragen oder falls der Build-Lauf rot wird: Melde dich einfach, dann
schauen wir uns die Fehlermeldung gemeinsam an.
