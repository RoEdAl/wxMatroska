#include "ISPPBuiltins.iss"

#define Cue2MkcFileVersion GetVersionNumbersString(Cue2MkcExe)
#define Cue2MkcFileCompany GetFileCompany(Cue2MkcExe)
#define Cue2MkcFileCopyright GetFileCopyright(Cue2MkcExe)
#define Cue2MkcFileDescription GetFileDescription(Cue2MkcExe)
#define Cue2MkcProductVersion GetFileProductVersion(Cue2MkcExe)
#define Cue2MkcFileVersionString GetFileVersionString(Cue2MkcExe)

[Setup]
AppID=cue2mkc
AppName={cm:cue2mkc}
AppVerName={cm:cue2mkc} {#Cue2MkcExeArch} {#Cue2MkcFileVersion}
AppVersion={#Cue2MkcFileVersion}
AppCopyright={#Cue2MkcFileCopyright}
AppPublisher={#Cue2MkcFileCompany}
VersionInfoProductName=cue2mkc ({#Cue2MkcExeArch})
VersionInfoDescription={#Cue2MkcFileDescription}
VersionInfoVersion={#Cue2MkcFileVersion}
VersionInfoCompany={#Cue2MkcFileCompany}
VersionInfoCopyright={#Cue2MkcFileCopyright}
DefaultDirName={autopf}\cue2mkc
SetupIconFile=..\gui\icons\cd_mka.ico
ShowLanguageDialog=no
Compression=lzma2/Max
DefaultGroupName=cue2mkc
ArchitecturesAllowed={#Cue2MkcExeArch}

[Languages]
Name: en; MessagesFile: compiler:Default.isl; LicenseFile: {#LicenseFileRtf}

[Types]
Name: full; Description: {cm:full_installation}; 
Name: compact; Description: {cm:compact_installation}
Name: custom; Description: {cm:custom_installation}; Flags: iscustom

[Components]
Name: cli; Description: {cm:desc_component_cli}; Types: full compact custom; 
Name: gui; Description: {cm:desc_component_gui}; Types: full custom; 

[Tasks]
Name: desktopicon; Description: {cm:CreateDesktopIcon}; GroupDescription: {cm:AdditionalIcons}

[Files]
Source: {#Cue2MkcExe}; DestDir: {app}; Flags: comparetimestamp; Components: cli; 
Source: {#Cue2MkcGuiExe}; DestDir: {app}; Flags: comparetimestamp; Components: gui; 
Source: {#WXWidgetsLibDir}\wxbase{#WXVerCompact}u{#WXDebug}_{#WXDllSuffix}; DestDir: {app}; Flags: comparetimestamp; Components: cli gui;
Source: {#WXWidgetsLibDir}\wxbase{#WXVerCompact}u{#WXDebug}_xml_{#WXDllSuffix}; DestDir: {app}; Flags: comparetimestamp; Components: cli;
Source: {#WXWidgetsLibDir}\wxmsw{#WXVerCompact}u{#WXDebug}_core_{#WXDllSuffix}; DestDir: {app}; Flags: comparetimestamp; Components: cli gui;
Source: {#LicenseFileTxt}; DestDir: {app}; Flags: comparetimestamp; Components: cli gui; Attribs: readonly;
Source: ..\cue\ISO-639-2_utf-8.txt; DestDir: {app}; Flags: comparetimestamp; Components: cli; Attribs: readonly;
Source: ..\wxCueChapters\ff-scan.cmake; DestDir: {app}; Flags: comparetimestamp; Components: cli; Attribs: readonly;
Source: ..\wxCueChapters\app.tags.json; DestDir: {app}; DestName: {#Cue2MkcBase}.tags.json; Flags: comparetimestamp; Components: cli; Attribs: readonly;

[Icons]
Name: {group}\{cm:cue2mkc} {cm:gui}; Filename: {app}\cue2mkc-frontend.exe; IconFilename: {app}\cue2mkc-frontend.exe; Comment: {cm:gui_comment}; Components: gui; 
Name: {group}\{cm:cue2mkc} {cm:console}; Filename: {cmd}; Parameters: "/T:3F /F:ON /V:ON /K SET PATH={app};!PATH!"; WorkingDir: {userdocs}; Components: cli; 
Name: {group}\{cm:license}; Filename: {app}\license.txt;
Name: {group}\{cm:UninstallProgram,{cm:cue2mkc}}; Filename: {uninstallexe}
Name: {autodesktop}\{cm:cue2mkc} {cm:gui}; Filename: {app}\cue2mkc-frontend.exe; WorkingDir: {app}; Tasks: desktopicon; Flags: createonlyiffileexists; Comment: {cm:gui_comment}; Components: gui;
Name: {autodesktop}\{cm:cue2mkc} {cm:console}; Filename: {cmd}; Parameters: "/T:3F /F:ON /V:ON /K SET PATH={app};!PATH!"; WorkingDir: {userdocs}; Tasks: desktopicon; Components: cli;

[Run]
Components: gui; Filename: {app}\cue2mkc-frontend.exe; Flags: PostInstall RunAsOriginalUser NoWait; Description: {cm:gui_run}; 

[CustomMessages]
cue2mkc=cue2mkc
en.license=License
en.gui=GUI
en.console=console utility
en.gui_comment=Frontend to cue2mkc console utility
en.desc_component_cli=Command line utility
en.desc_component_gui=GUI Frontend
en.full_installation=Full instalation
en.compact_installation=Compact instalation
en.custom_installation=Custom instalation
en.gui_run=Run cue2mkc GUI frontend
 
