#include "ISPPBuiltins.iss"

#define Cue2MkcExe SourcePath + "/../bin/Release/cue2mkc.exe"
#define Cue2MkcFileVersion GetFileVersion(Cue2MkcExe)
#define Cue2MkcFileCompany GetFileCompany(Cue2MkcExe)
#define Cue2MkcFileCopyright GetFileCopyright(Cue2MkcExe)
#define Cue2MkcFileDescription GetFileDescription(Cue2MkcExe)
#define Cue2MkcProductVersion GetFileProductVersion(Cue2MkcExe)
#define Cue2MkcFileVersionString GetFileVersionString(Cue2MkcExe)

[Setup]
AppID=cue2mkc
AppName={cm:cue2mkc}
AppVerName={cm:cue2mkc} {#Cue2MkcFileVersion}
AppVersion={#Cue2MkcFileVersion}
AppCopyright={#Cue2MkcFileCopyright}
AppPublisher={#Cue2MkcFileCompany}
VersionInfoProductName=cue2mkc
VersionInfoDescription={#Cue2MkcFileDescription}
VersionInfoVersion={#Cue2MkcFileVersion}
VersionInfoCompany={#Cue2MkcFileCompany}
VersionInfoCopyright={#Cue2MkcFileCopyright}
DefaultDirName={pf}\cue2mkc
SetupIconFile=..\gui\icons\cd_mka.ico
OutputDir=..\bin\setup
OutputBaseFilename=cue2mkc_setup
ShowLanguageDialog=no
MinVersion=,5.1.2600
Compression=lzma2/Max
DefaultGroupName=cue2mkc
ArchitecturesAllowed=x86 x64
PrivilegesRequired=admin

[Languages]
Name: en; MessagesFile: compiler:Default.isl; LicenseFile: ..\wxCueChapters\license.txt

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
Source: ..\bin\Release\cue2mkc.exe; DestDir: {app}; Flags: comparetimestamp; Components: cli; 
Source: ..\bin\Release\cue2mkcgui.exe; DestDir: {app}; Flags: comparetimestamp; Components: gui; 
Source: ..\bin\MediaInfo\MediaInfo.dll; DestDir: {app}; Flags: comparetimestamp; Components: cli; 
Source: ..\wxCueChapters\license.txt; DestDir: {app}; Flags: comparetimestamp; 
Source: ..\cue\ISO-639-2_utf-8.txt; DestDir: {app}; Flags: comparetimestamp; 

[Icons]
Name: {group}\{cm:cue2mkc} {cm:gui}; Filename: {app}\cue2mkcgui.exe; WorkingDir: {userdocs}; IconFilename: {app}\cue2mkcgui.exe; Comment: {cm:gui_comment}; Components: gui; 
Name: {group}\{cm:cue2mkc} {cm:console}; Filename: {cmd}; Parameters: "/T:3F /F:ON /V:ON /K SET PATH={app};!PATH!"; WorkingDir: {userdocs}; Components: cli; 
Name: {group}\{cm:license}; Filename: {app}\license.txt;
Name: {group}\{cm:UninstallProgram,{cm:cue2mkc}}; Filename: {uninstallexe}
Name: {commondesktop}\{cm:cue2mkc} {cm:gui}; Filename: {app}\cue2mkcgui.exe; WorkingDir: {app}; Tasks: desktopicon; Flags: createonlyiffileexists; IconFilename: {app}\cue2mkc.exe; Comment: {cm:gui_comment}; Components: gui;
Name: {commondesktop}\{cm:cue2mkc} {cm:console}; Filename: {cmd}; Parameters: "/T:3F /F:ON /V:ON /K SET PATH={app};!PATH!"; WorkingDir: {userdocs}; Components: cli;

[Run]
Components: gui; Filename: {app}\cue2mkcgui.exe; WorkingDir: {userappdata}; Flags: PostInstall RunAsOriginalUser NoWait; Description: {cm:gui_run}; 

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
 
