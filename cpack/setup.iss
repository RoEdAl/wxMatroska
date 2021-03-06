#include "ISPPBuiltins.iss"

#define Cue2MkcFileVersion GetVersionNumbersString(Cue2MkcExe)
#define Cue2MkcFileCompany GetFileCompany(Cue2MkcExe)
#define Cue2MkcFileCopyright GetFileCopyright(Cue2MkcExe)
#define Cue2MkcFileDescription GetFileDescription(Cue2MkcExe)
#define Cue2MkcProductVersion GetFileProductVersion(Cue2MkcExe)
#define Cue2MkcFileVersionString GetFileVersionString(Cue2MkcExe)

[Setup]
SourceDir={#Cue2MkcFilesDir}
AppID={#Cue2MkcBase}
AppName={cm:cue2mkc}
AppVerName={cm:cue2mkc} {#Cue2MkcExeArch} {#Cue2MkcFileVersion}
AppVersion={#Cue2MkcFileVersion}
AppCopyright={#Cue2MkcFileCopyright}
AppPublisher={#Cue2MkcFileCompany}
VersionInfoProductName={#Cue2MkcBase} ({#Cue2MkcExeArch})
VersionInfoDescription={#Cue2MkcFileDescription}
VersionInfoVersion={#Cue2MkcFileVersion}
VersionInfoCompany={#Cue2MkcFileCompany}
VersionInfoCopyright={#Cue2MkcFileCopyright}
DefaultDirName={autopf}\{#Cue2MkcBase}
SetupIconFile={#SetupIconFile}
ShowLanguageDialog=no
Compression=lzma2/Max
DefaultGroupName={#Cue2MkcBase}
ArchitecturesAllowed={#Cue2MkcExeArch}
ArchitecturesInstallIn64BitMode={#Cue2MkcExeArch}
TimeStampsInUTC=yes
TouchDate={#Cue2MkcTouchDate}
TouchTime={#Cue2MkcTouchTime}

[Languages]
Name: en; MessagesFile: compiler:Default.isl; LicenseFile: license.rtf

[Types]
Name: full; Description: {cm:full_installation}; 
Name: compact; Description: {cm:compact_installation}
Name: custom; Description: {cm:custom_installation}; Flags: iscustom

[Components]
Name: cli; Description: {cm:desc_component_cli}; Types: full compact custom; 
Name: gui; Description: {cm:desc_component_gui}; Types: full custom; 

[Tasks]
Name: desktopicon; Description: {cm:CreateDesktopIcon}; GroupDescription: {cm:AdditionalIcons}; Flags: unchecked;

[Files]
Source: {#Cue2MkcBase}.exe; DestDir: {app}; Flags: comparetimestamp touch; Components: cli; 
Source: {#Cue2MkcBase}-frontend.exe; DestDir: {app}; Flags: comparetimestamp touch; Components: gui;
Source: *.dll; DestDir: {app}; Flags: comparetimestamp; Components: cli gui; Attribs: notcontentindexed;
Source: *.txt; DestDir: {app}; Flags: comparetimestamp setntfscompression overwritereadonly uninsremovereadonly; Components: cli gui; Attribs: readonly notcontentindexed;
Source: *.cmake; DestDir: {app}; Flags: comparetimestamp setntfscompression overwritereadonly uninsremovereadonly; Components: cli; Attribs: readonly notcontentindexed;
Source: *.json; DestDir: {app}; Flags: comparetimestamp setntfscompression overwritereadonly uninsremovereadonly; Components: cli; Attribs: readonly notcontentindexed;

[Icons]
Name: {group}\{cm:cue2mkc} {cm:gui}; Filename: {app}\{#Cue2MkcBase}-frontend.exe; IconFilename: {app}\cue2mkc-frontend.exe; Comment: {cm:gui_comment}; Components: gui; 
Name: {group}\{cm:cue2mkc} {cm:console}; Filename: {cmd}; Parameters: "/T:3F /F:ON /V:ON /K SET PATH={app};!PATH!"; WorkingDir: {userdocs}; Components: cli; 
Name: {group}\{cm:license}; Filename: {app}\license.txt;
Name: {group}\{cm:UninstallProgram,{cm:cue2mkc}}; Filename: {uninstallexe}
Name: {autodesktop}\{cm:cue2mkc} {cm:gui}; Filename: {app}\{#Cue2MkcBase}-frontend.exe; WorkingDir: {app}; Tasks: desktopicon; Flags: createonlyiffileexists; Comment: {cm:gui_comment}; Components: gui;
Name: {autodesktop}\{cm:cue2mkc} {cm:console}; Filename: {cmd}; Parameters: "/T:3F /F:ON /V:ON /K SET PATH={app};!PATH!"; WorkingDir: {userdocs}; Tasks: desktopicon; Components: cli;

[Run]
Components: gui; Filename: {app}\{#Cue2MkcBase}-frontend.exe; Flags: PostInstall RunAsOriginalUser NoWait; Description: {cm:gui_run}; 

[Registry]
; http://docs.microsoft.com/en-us/windows/win32/fileio/maximum-file-path-limitation
Root: HKLM; Subkey: "SYSTEM\CurrentControlSet\Control\FileSystem"; ValueType: dword; ValueName: "LongPathsEnabled"; ValueData: 1; MinVersion: 10.0.14393; Check: IsAdminInstallMode;

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
 
[Code]

const
    VC_REG_KEY = 'SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\{#Cue2MkcExeArch}';

function IsVCRedistInstalled: Boolean;
var 
  major, minor, bld, rbld: Cardinal;
  packedVersion, minPackedVersion : int64;
begin
  if
    RegQueryDWordValue(HKEY_LOCAL_MACHINE, VC_REG_KEY, 'Major', major) and
    RegQueryDWordValue(HKEY_LOCAL_MACHINE, VC_REG_KEY, 'Minor', minor) and
    RegQueryDWordValue(HKEY_LOCAL_MACHINE, VC_REG_KEY, 'Bld', bld) and
    RegQueryDWordValue(HKEY_LOCAL_MACHINE, VC_REG_KEY, 'Rbld', rbld)
  then
  begin
    packedVersion := PackVersionComponents(major, minor, bld, rbld);
    minPackedVersion := PackVersionComponents(14, 0, 0, 0);
    Result := packedVersion >= minPackedVersion;
  end
  else
    Result := False;
end;

function InitializeSetup: Boolean;
begin
  Result := IsVCRedistInstalled
  if not Result then
    SuppressibleMsgBox(
        FmtMessage(SetupMessage(msgWinVersionTooLowError), ['Visual C++ 2015-2022 Redistributable ({#Cue2MkcExeArch})', '14.0']),
        mbCriticalError, MB_OK, IDOK
    );
end;
