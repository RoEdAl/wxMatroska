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
Name: tool; Description: {cm:Downloads}
Name: tool/cmake; Description: {cm:cmake}; Flags: checkedonce
Name: tool/mkvtoolnix; Description: {cm:mkvtoolnix}; Flags: checkedonce
Name: tool/ffmpeg; Description: {cm:ffmpeg}; Flags: checkedonce
Name: tool/imgmagick; Description: {cm:imgmagick}; Flags: checkedonce unchecked
Name: tool/mupdf; Description: {cm:mupdf}; Flags: checkedonce unchecked

[Files]
; application files
Source: {#Cue2MkcBase}.exe; DestDir: {app}; Flags: comparetimestamp touch; Components: cli; 
Source: {#Cue2MkcBase}-frontend.exe; DestDir: {app}; Flags: comparetimestamp touch; Components: gui;
Source: *.dll; DestDir: {app}; Flags: comparetimestamp; Components: cli gui; Attribs: notcontentindexed;
Source: *.txt; DestDir: {app}; Flags: comparetimestamp setntfscompression overwritereadonly uninsremovereadonly; Components: cli gui; Attribs: readonly notcontentindexed;
Source: *.cmake; DestDir: {app}; Flags: comparetimestamp setntfscompression overwritereadonly uninsremovereadonly; Components: cli; Attribs: readonly notcontentindexed;
Source: *.json; DestDir: {app}; Flags: comparetimestamp setntfscompression overwritereadonly uninsremovereadonly; Components: cli; Attribs: readonly notcontentindexed;
; CMake files extracted from ZIP package
Source: {tmp}/cmake/cmake-{#CMakeVersion}-windows-x86_64/*; DestDir: {app}/tools/CMake; Flags: recursesubdirs external comparetimestamp setntfscompression overwritereadonly uninsremovereadonly; Attribs: readonly notcontentindexed; Tasks: tool/cmake; 
; MKVToolNix files extracted from ZIP package
Source: {tmp}/mkvtoolnix/*.exe; DestDir: {app}/tools/MKVToolNix; Flags: external comparetimestamp setntfscompression overwritereadonly uninsremovereadonly; Attribs: readonly notcontentindexed; Tasks: tool/mkvtoolnix
; FfMpeg files extracted from ZIP package
Source: {tmp}/ffmpeg/*.exe; DestDir: {app}/tools/ffmpeg/bin; Flags: external comparetimestamp setntfscompression overwritereadonly uninsremovereadonly; Attribs: readonly notcontentindexed; Tasks: tool/ffmpeg
; ImageMagick files extracted from ZIP package
Source: {tmp}/imgmagick/*.exe; DestDir: {app}/tools/ImageMagick; Flags: external comparetimestamp setntfscompression overwritereadonly uninsremovereadonly; Attribs: readonly notcontentindexed; Tasks: tool/imgmagick
Source: {tmp}/imgmagick/*.xml; DestDir: {app}/tools/ImageMagick; Flags: external comparetimestamp setntfscompression overwritereadonly uninsremovereadonly; Attribs: readonly notcontentindexed; Tasks: tool/imgmagick
Source: {tmp}/imgmagick/*.icc; DestDir: {app}/tools/ImageMagick; Flags: external comparetimestamp setntfscompression overwritereadonly uninsremovereadonly; Attribs: readonly notcontentindexed; Tasks: tool/imgmagick
; MuPDF files extracted from ZIP package
Source: {tmp}/mupdf/*.exe; DestDir: {app}/tools/mupdf; Flags: external comparetimestamp setntfscompression overwritereadonly uninsremovereadonly; Attribs: readonly notcontentindexed; Tasks: tool/mupdf
; unpacker (used internally)
Source: "7za.exe"; Flags: dontcopy

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
en.Downloads=Downloads:
en.gui_run=Run cue2mkc GUI frontend
en.cmake=CMake – general purpose scripting language‼ [version {#CMakeVersion}]
en.mkvtoolnix=MKVToolNix – Matroska tools [version {#MKVToolNixVersion}]
en.ffmpeg=FfMpeg [version {#FFMpegVersion}]
en.imgmagick=ImageMagick [version {#ImgMagickVersion}]
en.mupdf=MuPDF [version {#MuPdfVersion}]
en.DownloadAbortedByUser=Download aborted by user
en.ExtractingTitle=Extracting downloaded files…
en.ExtractingDesc=Extracting: %1
en.ExtractFailed=Fail to extract downloaded package %1
en.RenameFailed=Fail to rename extracted executable %1
 
[Code]

const
    VC_REG_KEY = 'SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\{#Cue2MkcExeArch}';
	
var
	DownloadPage: TDownloadWizardPage;

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

<event('InitializeSetup')>
function CheckVCRedist: Boolean;
begin
  Result := IsVCRedistInstalled
  if not Result then
    SuppressibleMsgBox(
        FmtMessage(SetupMessage(msgWinVersionTooLowError), ['Visual C++ 2015-2022 Redistributable ({#Cue2MkcExeArch})', '14.0']),
        mbCriticalError, MB_OK, IDOK
    );
end;

<event('InitializeWizard')>
procedure Download_Init;
begin
  DownloadPage := CreateDownloadPage(SetupMessage(msgWizardPreparing), SetupMessage(msgPreparingDesc), nil);
end;

<event('PrepareToInstall')>
function Download_Run(var NeedsRestart: Boolean): String;
var
	dlCnt: Integer;
	sevenZip: String;
	extractDir: String;
	exeRes: Integer;
begin
	DownloadPage.Clear;
	dlCnt := 0;
	if WizardIsTaskSelected('tool/cmake') then
	begin
		DownloadPage.Add('https://github.com/Kitware/CMake/releases/download/v{#CMakeVersion}/cmake-{#CMakeVersion}-windows-x86_64.zip', 'cmake-{#CMakeVersion}-windows-x86_64.zip', '{#CMakeChecksum}');
		dlCnt := dlCnt + 1;
	end;		
	if WizardIsTaskSelected('tool/mkvtoolnix') then
	begin
		DownloadPage.Add('https://mkvtoolnix.download/windows/releases/{#MKVToolNixVersion}/mkvtoolnix-64-bit-{#MKVToolNixVersion}.7z', 'mkvtoolnix-64-bit-{#MKVToolNixVersion}.7z', '{#MKVToolNixChecksum}');
		dlCnt := dlCnt + 1;
	end;	
	if WizardIsTaskSelected('tool/ffmpeg') then
	begin
		DownloadPage.Add('https://www.gyan.dev/ffmpeg/builds/packages/ffmpeg-{#FfMpegVersion}-full_build.7z', 'ffmpeg-{#FfMpegVersion}-full_build.7z', '{#FfMpegChecksum}');
		dlCnt := dlCnt + 1;
	end;
	if WizardIsTaskSelected('tool/imgmagick') then
	begin
		DownloadPage.Add('https://imagemagick.org/archive/binaries/ImageMagick-{#ImgMagickVersion}-portable-Q16-x64.zip', 'ImageMagick-{#ImgMagickVersion}-portable-Q16-x64.zip', '{#ImgMagickChecksum}');
		dlCnt := dlCnt + 1;
	end;	
	if WizardIsTaskSelected('tool/mupdf') then
	begin
		DownloadPage.Add('https://mupdf.com/downloads/archive/mupdf-{#MuPdfVersion}-windows.zip', 'mupdf-{#MuPdfVersion}-windows.zip', '{#MuPdfChecksum}');
		dlCnt := dlCnt + 1;
	end;
	if dlCnt > 0 then
	begin
		DownloadPage.ProgressBar.Visible := True;
		DownloadPage.AbortButton.Visible := True;
		DownloadPage.Show;
		try
		  try
			DownloadPage.Download; // This downloads the files to {tmp}
			
			ExtractTemporaryFile('7za.exe');
			extractDir := ExpandConstant('{tmp}')
			sevenZip := ExpandConstant('{tmp}/7za.exe');
			
			DownloadPage.ProgressBar.Visible := False;
			DownloadPage.AbortButton.Visible := False;
			
			if WizardIsTaskSelected('tool/cmake') then
			begin
				exeRes := 0;
				DownloadPage.SetText(ExpandConstant('{cm:ExtractingTitle}'), ExpandConstant('{cm:ExtractingDesc,CMake}'));
				Exec(sevenZip, 'x cmake-{#CMakeVersion}-windows-x86_64.zip -ocmake -aoa -bso0 -xr!cmake-gui.exe -xr!ctest.exe -xr!cpack.exe -xr!doc -xr!man -xr!vim -xr!emacs -xr!bash* -xr!aclocal -xr!Help', extractDir, SW_HIDE, ewWaitUntilTerminated, exeRes);
				if exeRes <> 0 then RaiseException(ExpandConstant('{cm:ExtractFailed,CMake}'));
			end;			
			
			if WizardIsTaskSelected('tool/mkvtoolnix') then
			begin
				exeRes := 0;
				DownloadPage.SetText(ExpandConstant('{cm:ExtractingTitle}'), ExpandConstant('{cm:ExtractingDesc,MKVToolNix}'));
				Exec(sevenZip, 'e mkvtoolnix-64-bit-{#MKVToolNixVersion}.7z -omkvtoolnix -r mkvmerge.exe mkvpropedit.exe -aoa -bso0', extractDir, SW_HIDE, ewWaitUntilTerminated, exeRes);
				if exeRes <> 0 then RaiseException(ExpandConstant('{cm:ExtractFailed,MKVToolNix}'));
			end;
			
			if WizardIsTaskSelected('tool/ffmpeg') then
			begin
				exeRes := 0;
				DownloadPage.SetText(ExpandConstant('{cm:ExtractingTitle}'), ExpandConstant('{cm:ExtractingDesc,FfMpeg}'));
				Exec(sevenZip, 'e ffmpeg-{#FfMpegVersion}-full_build.7z -offmpeg -r ffmpeg.exe ffprobe.exe -aoa -bso0', extractDir, SW_HIDE, ewWaitUntilTerminated, exeRes);
				if exeRes <> 0 then RaiseException(ExpandConstant('{cm:ExtractFailed,Ffmpeg}'));
			end;
			
			if WizardIsTaskSelected('tool/imgmagick') then
			begin
				exeRes := 0;
				DownloadPage.SetText(ExpandConstant('{cm:ExtractingTitle}'), ExpandConstant('{cm:ExtractingDesc,ImageMagick}'));
				Exec(sevenZip, 'e ImageMagick-{#ImgMagickVersion}-portable-Q16-x64.zip -oimgmagick -r magick.exe *.xml *.icc -aoa -bso0', extractDir, SW_HIDE, ewWaitUntilTerminated, exeRes);
				if exeRes <> 0 then RaiseException(ExpandConstant('{cm:ExtractFailed,ImageMagick}'));
			end;
			
			if WizardIsTaskSelected('tool/mupdf') then
			begin
				exeRes := 0;
				DownloadPage.SetText(ExpandConstant('{cm:ExtractingTitle}'), ExpandConstant('{cm:ExtractingDesc,MuPDF}'));
				Exec(sevenZip, 'e mupdf-{#MuPdfVersion}-windows.zip -omupdf -r *.txt mutool.exe -aoa -bso0', extractDir, SW_HIDE, ewWaitUntilTerminated, exeRes);
				if exeRes <> 0 then RaiseException(ExpandConstant('{cm:ExtractFailed,MuPDF}'));
			end;
			
			Result := '';
		  except
			if DownloadPage.AbortedByUser then
				Result := ExpandConstant('{cm:DownloadAbortedByUser}')
			else
				Result := GetExceptionMessage;
		  end;
		finally
		  DownloadPage.Hide;
		end;
	end
	else Result := '';
end;