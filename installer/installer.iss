; installer/installer.iss
; Erstellt die finale Setup.exe fuer RetroPoly-80 (VST3 + Standalone)
; Kompilieren mit Inno Setup: iscc installer.iss

[Setup]
AppName=RetroPoly-80
AppVersion=1.1.0
AppPublisher=DeinStudioName
DefaultDirName={autopf}\RetroPoly-80
DefaultGroupName=RetroPoly-80
DisableProgramGroupPage=yes
OutputDir=Output
OutputBaseFilename=RetroPoly-80_Setup
Compression=lzma2
SolidCompression=yes
WizardStyle=modern
ArchitecturesInstallIn64BitMode=x64
PrivilegesRequired=admin

[Files]
; VST3 geht immer in den gemeinsamen VST3-Systemordner, damit FL Studio
; (und jede andere VST3-faehige DAW) es automatisch findet.
Source: "..\build\RetroPoly80_artefacts\Release\VST3\RetroPoly-80.vst3\*"; \
    DestDir: "{commoncf64}\VST3\RetroPoly-80.vst3"; Flags: recursesubdirs ignoreversion

; Standalone-Version als eigenstaendige exe
Source: "..\build\RetroPoly80_artefacts\Release\Standalone\RetroPoly-80.exe"; \
    DestDir: "{app}"; Flags: ignoreversion

[Icons]
Name: "{group}\RetroPoly-80"; Filename: "{app}\RetroPoly-80.exe"
Name: "{autodesktop}\RetroPoly-80"; Filename: "{app}\RetroPoly-80.exe"; Tasks: desktopicon

[Tasks]
Name: desktopicon; Description: "Desktop-Symbol anlegen"; GroupDescription: "Zusaetzliche Symbole:"

[UninstallDelete]
Type: filesandordirs; Name: "{commoncf64}\VST3\RetroPoly-80.vst3"
