#NoTrayIcon
#region ;**** Directives created by AutoIt3Wrapper_GUI ****
#AutoIt3Wrapper_Icon=icons/cd_mka.ico
#AutoIt3Wrapper_Outfile=../bin/Release/cue2mkcgui.exe
#AutoIt3Wrapper_Compression=4
#AutoIt3Wrapper_Res_Comment=This is frontend to cue2mkc tool
#AutoIt3Wrapper_Res_Description=Graphical user interface for cue2mkc command line tool
#AutoIt3Wrapper_Res_Fileversion=0.1.0.82
#AutoIt3Wrapper_Res_Fileversion_AutoIncrement=y
#AutoIt3Wrapper_Res_LegalCopyright=Simplified BSD License - http://www.opensource.org/licenses/bsd-license.html
#AutoIt3Wrapper_Res_SaveSource=y
#AutoIt3Wrapper_Res_requestedExecutionLevel=asInvoker
#AutoIt3Wrapper_Res_Field=Company|Edmunt Pienkowsky - roed@onet.eu
#AutoIt3Wrapper_Res_Icon_Add=icons/cue_file.ico
#AutoIt3Wrapper_Res_Icon_Add=icons/cue_file_add.ico
#AutoIt3Wrapper_Res_Icon_Add=icons/cue_file_remove.ico
#AutoIt3Wrapper_Res_Icon_Add=icons/media_file.ico
#AutoIt3Wrapper_Res_Icon_Add=icons/media_file_add.ico
#AutoIt3Wrapper_Res_Icon_Add=icons/file_mask.ico
#AutoIt3Wrapper_Run_Tidy=y
#endregion ;**** Directives created by AutoIt3Wrapper_GUI ****
;
; cue2mkcgui - simple frontend cue2mkc utility.
;

AutoItSetOption("TrayIconHide", 1)
AutoItSetOption("GUICloseOnESC", 0)

#include <Constants.au3>
#include <GUIConstantsEx.au3>
#include <ButtonConstants.au3>
#include <EditConstants.au3>
#include <GUIConstantsEx.au3>
#include <ListBoxConstants.au3>
#include <TreeViewConstants.au3>
#include <ComboConstants.au3>
#include <StaticConstants.au3>
#include <TabConstants.au3>
#include <WindowsConstants.au3>
#include <Misc.au3>
#include <GuiListBox.au3>
#include <GuiComboBox.au3>
#include <GuiEdit.au3>
#include <WinAPI.au3>
#include <File.au3>
#include <GuiTreeView.au3>

Func get_tool_full_path($sExeName, $sExt = "exe")
	Local $sBin = $sExeName & "." & $sExt
	Local $sPath = @ScriptDir & "\" & $sBin
	If FileExists($sPath) Then
		Return SetError(0, 1, $sPath)
	Else
		Return SetError(0, 0, $sBin)
	EndIf
EndFunc   ;==>get_tool_full_path

Dim Const $APP_NAME = "cue2mkc GUI"

Dim Const $CUE2MKC_EXE = get_tool_full_path("cue2mkc")
Dim Const $CUE2MKC_EXE_EXISTS = @extended

Dim $sErr = ""

If Not FileExists($CUE2MKC_EXE) Then
	$sErr &= $CUE2MKC_EXE
	$sErr &= @CRLF
EndIf

If StringLen($sErr) > 0 Then
	MsgBox(0x10, $APP_NAME, "The following utilities are missing:" & @CRLF & $sErr)
EndIf

Dim Const $CUE_FILTER = "CUE files (*.cue)|Text files (*.txt)|Audio files (*.flac;*.ape;*.wv;*.wav;*.aiff;*.tta)|All files (*)"
Dim Const $XML_FILTER = "XML files (*.xml)|Matroska chapters XML files (*.mkc.xml)|Text files (*.txt)|All files (*)"
Dim Const $MEDIA_FILTER = "Audio files (*.flac;*.ape;*.wv;*.wav;*.aiff;*.tta)|All files (*)"
Dim Const $STILL_ACTIVE = 259
Dim Const $APP_EXIT_CODE = "Exit code: %d."
Dim Const $MEDIA_INFO = "MediaInfo"

Func is_directory($s)
	If Not FileExists($s) Then
		Return SetError(1, 1, False)
	EndIf

	Local $a = FileGetAttrib($s)
	Return SetError(0, 0, (StringInStr($s, "D") > 0))
EndFunc   ;==>is_directory

Func get_directory($sPath)
	Local $sDrive, $sDir, $sFName, $sExt
	_PathSplit($sPath, $sDrive, $sDir, $sFName, $sExt)
	Local $sDirectory = _PathMake($sDrive, $sDir, "", "")
	$sDirectory = StringTrimRight($sDirectory, 1)
	Return SetError(0, 0, $sDirectory)
EndFunc   ;==>get_directory

Func get_mkvmerge_dir()
	Local $sMkvmergeExecutable = RegRead("HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\mmg.exe", "")
	If @error Then
		Return SetError(0, 1, "")
	Else
		Return get_directory($sMkvmergeExecutable)
	EndIf
EndFunc   ;==>get_mkvmerge_dir

#region ### START Koda GUI section ### Form=C:\Users\Normal\Documents\Visual Studio 2010\Projects\wxMatroska\gui\cue2mkcgui.kxf
$FormMain = GUICreate("cue2mkc GUI", 545, 410, -1, -1, BitOR($GUI_SS_DEFAULT_GUI, $WS_MAXIMIZEBOX, $WS_SIZEBOX, $WS_THICKFRAME, $WS_TABSTOP), BitOR($WS_EX_ACCEPTFILES, $WS_EX_WINDOWEDGE))
GUISetFont(8, 400, 0, "Microsoft Sans Serif")
$MainTab = GUICtrlCreateTab(0, 0, 541, 369, $TCS_MULTILINE)
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKRIGHT + $GUI_DOCKTOP + $GUI_DOCKBOTTOM)
$PaneInputOutput = GUICtrlCreateTabItem("&Source && destination")
$LabelFiles = GUICtrlCreateLabel("&Sources list:", 4, 24, 80, 21, $SS_CENTERIMAGE)
GUICtrlSetFont(-1, 8, 800, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$TreeViewInputFiles = GUICtrlCreateTreeView(4, 49, 529, 210, BitOR($TVS_HASBUTTONS, $TVS_HASLINES, $TVS_DISABLEDRAGDROP, $TVS_SHOWSELALWAYS, $TVS_FULLROWSELECT), $WS_EX_STATICEDGE)
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKRIGHT + $GUI_DOCKTOP + $GUI_DOCKBOTTOM)
$ButtonInputAdd = GUICtrlCreateButton("+", 5, 262, 52, 52, $BS_NOTIFY)
GUICtrlSetFont(-1, 14, 800, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKBOTTOM + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Add specified file or file mask to list")
$ButtonInputDelete = GUICtrlCreateButton("-", 61, 262, 52, 52, $BS_NOTIFY)
GUICtrlSetFont(-1, 14, 800, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKBOTTOM + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Delete selected items from list")
$ButtonMakeMask = GUICtrlCreateButton("•", 118, 262, 52, 52, $BS_NOTIFY)
GUICtrlSetFont(-1, 14, 800, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKBOTTOM + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Make file mask with selected items")
$ButtonDataFile = GUICtrlCreateButton("+", 174, 262, 52, 52, $BS_NOTIFY)
GUICtrlSetFont(-1, 12, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKBOTTOM + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Add media file to selected cue sheet")
$LabelOutput = GUICtrlCreateLabel("&Destination:", 4, 319, 80, 17, $SS_CENTERIMAGE)
GUICtrlSetFont(-1, 8, 800, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKBOTTOM + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$ComboOutput = GUICtrlCreateCombo("", 4, 339, 137, 25, BitOR($CBS_DROPDOWNLIST, $CBS_AUTOHSCROLL))
GUICtrlSetData(-1, "Input directory|Directory|File")
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKBOTTOM + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$InputOuputDir = GUICtrlCreateInput("", 144, 339, 353, 21, $GUI_SS_DEFAULT_INPUT)
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKRIGHT + $GUI_DOCKBOTTOM + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Destination directory")
$ButtonOutputDirectory = GUICtrlCreateButton("1", 501, 339, 29, 21, $BS_NOTIFY)
GUICtrlSetFont(-1, 10, 400, 0, "Wingdings")
GUICtrlSetResizing(-1, $GUI_DOCKRIGHT + $GUI_DOCKBOTTOM + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Specify output directory or file")
$OutputPane = GUICtrlCreateTabItem("&Messages")
$LabelLog = GUICtrlCreateLabel("&Messages:", 4, 24, 71, 21, $SS_CENTERIMAGE)
GUICtrlSetFont(-1, 8, 800, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$ListLog = GUICtrlCreateList("", 4, 46, 529, 276, BitOR($LBS_NOTIFY, $LBS_USETABSTOPS, $LBS_NOINTEGRALHEIGHT, $LBS_NOSEL, $WS_HSCROLL, $WS_VSCROLL), $WS_EX_STATICEDGE)
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetBkColor(-1, 0xF0F0F0)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKRIGHT + $GUI_DOCKTOP + $GUI_DOCKBOTTOM)
GUICtrlSetTip(-1, "Application messages")
$ButtonMsgCopy = GUICtrlCreateButton("&Copy", 4, 325, 61, 29, $BS_NOTIFY)
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKBOTTOM + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Copy messages to clipboard")
$ButtonClearLog = GUICtrlCreateButton("Clear", 68, 325, 61, 29, $BS_NOTIFY)
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKBOTTOM + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Clear log")
$OptionsPane = GUICtrlCreateTabItem("&General options")
$GroupGeneral = GUICtrlCreateGroup("&General", 4, 27, 265, 53, -1, $WS_EX_TRANSPARENT)
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$CheckBoxMerge = GUICtrlCreateCheckbox("Merge mode", 8, 43, 85, 17)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Merge all input files into one cue sheet (requires MediaInfo lubrary)")
$CheckBoxA = GUICtrlCreateCheckbox("Abort when conversion errors occurs", 8, 58, 201, 17)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlCreateGroup("", -99, -99, 1, 1)
$GroupInputOptions = GUICtrlCreateGroup("&Input", 5, 80, 265, 93)
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$CheckBoxEc = GUICtrlCreateCheckbox("Embedded cue sheet", 9, 96, 129, 17)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Try to read embedded cue sheet (requires MediaInfo library)")
$CheckBoxSingleMediaFile = GUICtrlCreateCheckbox("Media file(s) without cuesheet", 9, 112, 169, 17)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Assume input file as media file without cue sheet")
$CheckBoxDf = GUICtrlCreateCheckbox("Use media file(s)", 9, 128, 97, 17)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Use media file(s) to calculate end time of chapters")
$LabelAlternateExt = GUICtrlCreateLabel("Alternate media files extensions:", 9, 146, 153, 21, $SS_CENTERIMAGE)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Comma separated list of alternate media extensions")
$InputAlternateExt = GUICtrlCreateInput("", 167, 146, 93, 21, $GUI_SS_DEFAULT_INPUT)
GUICtrlSetLimit(-1, 150)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Comma-separated list of alternate extensions of media files")
GUICtrlCreateGroup("", -99, -99, 1, 1)
$GroupOutputOptions = GUICtrlCreateGroup("&Output", 4, 173, 265, 125)
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$LabelOutputFormat = GUICtrlCreateLabel("Format:", 8, 188, 43, 21, $SS_CENTERIMAGE)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$ComboOutputFormat = GUICtrlCreateCombo("", 56, 188, 177, 25, BitOR($CBS_DROPDOWNLIST, $CBS_AUTOHSCROLL))
GUICtrlSetData(-1, "cue sheet|Matroska chapters|cue points")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$CheckBoxT = GUICtrlCreateCheckbox("Generate tags XML file", 26, 211, 157, 17)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$CheckBoxK = GUICtrlCreateCheckbox("Generate options file for mkvmerge", 26, 227, 213, 17)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$LabelLang = GUICtrlCreateLabel("Language:", 26, 243, 53, 21, $SS_CENTERIMAGE)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$InputLang = GUICtrlCreateInput("eng", 84, 243, 41, 21, $GUI_SS_DEFAULT_INPUT)
GUICtrlSetLimit(-1, 3)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "This option also affects tags file generation")
$LabelCueSheetEncoding = GUICtrlCreateLabel("&Encoding:", 8, 266, 55, 21, $SS_CENTERIMAGE)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Encoding of generated text files.")
$ComboCueSheetEncoding = GUICtrlCreateCombo("", 73, 266, 141, 25, BitOR($CBS_DROPDOWNLIST, $CBS_AUTOHSCROLL), $WS_EX_CLIENTEDGE)
GUICtrlSetData(-1, "default|UTF-8|UTF-8 with BOM|UTF-16|UTF-16 with BOM|UTF-16BE|UTF-16BE with BOM")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlCreateGroup("", -99, -99, 1, 1)
$GroupTags = GUICtrlCreateGroup("&Tags parsing", 276, 27, 233, 85)
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$CheckBoxCq = GUICtrlCreateCheckbox("Correct ""simple 'quotation' marks""", 280, 43, 201, 17)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Correct ""simple 'quotation' marks"" inside tags")
$CheckBoxTc = GUICtrlCreateCheckbox("Parse tags from cue sheet comments", 280, 59, 197, 17)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Try to parse tags from cue sheet comments")
$CheckBoxEt = GUICtrlCreateCheckbox("Ellipsize tags", 280, 74, 97, 17)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Replace last three dots [...] with ellipsis […] character")
$CheckBoxRs = GUICtrlCreateCheckbox("Remove extra spaces", 280, 90, 161, 17)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Remove extra spaces (e.g. more than two) from tags")
GUICtrlCreateGroup("", -99, -99, 1, 1)
$GroupUsedTags = GUICtrlCreateGroup("&Used tags", 276, 115, 233, 72, -1, $WS_EX_TRANSPARENT)
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$CheckBoxTagUseCdText = GUICtrlCreateCheckbox("Use CD-TEXT tags", 280, 131, 125, 17)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Use CD-TEXT tags")
$CheckBoxTagUseCueComments = GUICtrlCreateCheckbox("Use tags from cuesheet comments", 280, 148, 205, 17)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Use tags taken from cuesheet comments")
$CheckBoxTagUseFromMedia = GUICtrlCreateCheckbox("Use tags from media file(s)", 280, 164, 161, 17)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Use tags taken from media's metadata")
GUICtrlCreateGroup("", -99, -99, 1, 1)
$GroupOther = GUICtrlCreateGroup("&Other options", 276, 187, 233, 71, -1, $WS_EX_TRANSPARENT)
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$CheckBoxEu = GUICtrlCreateCheckbox("Generate edition UID", 280, 203, 129, 17)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$CheckBoxReadMetadata = GUICtrlCreateCheckbox("Read metadata", 280, 219, 101, 17)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Read metadata from media file")
$CheckBoxMLang = GUICtrlCreateCheckbox("Use MLang library", 280, 236, 117, 17)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlCreateGroup("", -99, -99, 1, 1)
$ChapterOptionsPane = GUICtrlCreateTabItem("&Chapters file generation options")
$GroupChapterOptions = GUICtrlCreateGroup("&Chapters file", 4, 24, 293, 117, -1, $WS_EX_TRANSPARENT)
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$CheckBoxCe = GUICtrlCreateCheckbox("Calculate end time of chapters if possible", 8, 38, 217, 17)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$CheckBoxCn = GUICtrlCreateCheckbox("Calculate chapter's end time from next chapter", 8, 54, 276, 17)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "If track's end time is unknown set it to next track position using frame offset")
$LabelFo = GUICtrlCreateLabel("Offset:", 24, 75, 35, 21, $SS_CENTERIMAGE)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$InputFo = GUICtrlCreateInput("150", 61, 75, 65, 21, BitOR($GUI_SS_DEFAULT_INPUT, $ES_NUMBER))
GUICtrlSetLimit(-1, 4)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Frame offset")
$LabelTrack01_1 = GUICtrlCreateLabel("For track 01 assume index", 8, 98, 128, 21, $SS_CENTERIMAGE)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$ComboTrack01 = GUICtrlCreateCombo("", 140, 98, 41, 25, BitOR($CBS_DROPDOWNLIST, $CBS_AUTOHSCROLL), $WS_EX_CLIENTEDGE)
GUICtrlSetData(-1, "00|01", "01")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$LabelTrack01_2 = GUICtrlCreateLabel("as beginning of track", 185, 98, 103, 21, $SS_CENTERIMAGE)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$CheckBoxHiddenIndexes = GUICtrlCreateCheckbox("Convert indexes to hidden chapters", 8, 121, 217, 17)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "For indexes > 1")
GUICtrlCreateGroup("", -99, -99, 1, 1)
$GroupFormatting = GUICtrlCreateGroup("&Formatting", 4, 146, 289, 69)
$LabelTf = GUICtrlCreateLabel("Track title format:", 8, 162, 86, 21, $SS_CENTERIMAGE)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$InputTf = GUICtrlCreateInput("%dp% - %dt% - %tt%", 104, 162, 181, 21, $GUI_SS_DEFAULT_INPUT)
GUICtrlSetLimit(-1, 150)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$LabelMf = GUICtrlCreateLabel("MKA tilte format:", 8, 186, 86, 21, $SS_CENTERIMAGE)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$InputMf = GUICtrlCreateInput("%dp% - %dt%", 104, 186, 181, 21, $GUI_SS_DEFAULT_INPUT)
GUICtrlSetLimit(-1, 150)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlCreateGroup("", -99, -99, 1, 1)
$OtherOptionsPane = GUICtrlCreateTabItem("&Advanced options")
$GroupFileExtensions = GUICtrlCreateGroup("&File extensions", 284, 24, 169, 113, -1, $WS_EX_TRANSPARENT)
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKRIGHT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$LabelDce = GUICtrlCreateLabel("Cue sheet:", 288, 40, 67, 21, $SS_CENTERIMAGE)
GUICtrlSetResizing(-1, $GUI_DOCKRIGHT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$InputDce = GUICtrlCreateInput("cue", 389, 40, 53, 21, $GUI_SS_DEFAULT_INPUT)
GUICtrlSetLimit(-1, 50)
GUICtrlSetResizing(-1, $GUI_DOCKRIGHT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$LabelDme = GUICtrlCreateLabel("Matroska chapters:", 288, 62, 95, 21, $SS_CENTERIMAGE)
GUICtrlSetResizing(-1, $GUI_DOCKRIGHT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$InputDme = GUICtrlCreateInput("mkc.xml", 389, 62, 53, 21, $GUI_SS_DEFAULT_INPUT)
GUICtrlSetLimit(-1, 50)
GUICtrlSetResizing(-1, $GUI_DOCKRIGHT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$LabelDte = GUICtrlCreateLabel("Matroska tags:", 288, 85, 95, 21, $SS_CENTERIMAGE)
GUICtrlSetResizing(-1, $GUI_DOCKRIGHT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$InputDte = GUICtrlCreateInput("mkt.xml", 389, 85, 53, 21, $GUI_SS_DEFAULT_INPUT)
GUICtrlSetLimit(-1, 50)
GUICtrlSetResizing(-1, $GUI_DOCKRIGHT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$LabelDoe = GUICtrlCreateLabel("mkvmerge options:", 288, 107, 95, 21, $SS_CENTERIMAGE)
GUICtrlSetResizing(-1, $GUI_DOCKRIGHT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$InputDoe = GUICtrlCreateInput("opt.txt", 389, 107, 53, 21, $GUI_SS_DEFAULT_INPUT)
GUICtrlSetLimit(-1, 50)
GUICtrlSetResizing(-1, $GUI_DOCKRIGHT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlCreateGroup("", -99, -99, 1, 1)
$GroupMkvmerge = GUICtrlCreateGroup("mkvmerge", 4, 24, 277, 141)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKRIGHT + $GUI_DOCKTOP + $GUI_DOCKHEIGHT)
$CheckBoxEacLog = GUICtrlCreateCheckbox("Attach EAC log(s)", 8, 41, 113, 17)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Attach EAC log file(s) to generated MKA file")
$CheckBoxCover = GUICtrlCreateCheckbox("Attach &cover image", 8, 56, 121, 17)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Attach cover image (cover|front|picture.jpg|png)")
$LabelCueSheetAttachMode = GUICtrlCreateLabel("Cue sheet attach mode", 8, 75, 115, 21, $SS_CENTERIMAGE)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$ComboCueSheetAttachMode = GUICtrlCreateCombo("", 130, 75, 141, 25, BitOR($CBS_DROPDOWNLIST, $CBS_AUTOHSCROLL), $WS_EX_CLIENTEDGE)
GUICtrlSetData(-1, "default|none|source|decoded|rendered")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$LabelMkvmergeDir = GUICtrlCreateLabel("Location:", 10, 104, 51, 21, $SS_CENTERIMAGE)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Directory where mkvmerge tool resides")
$InputMkvmergeDir = GUICtrlCreateInput("", 61, 104, 181, 21, $GUI_SS_DEFAULT_INPUT)
GUICtrlSetLimit(-1, 50)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKRIGHT + $GUI_DOCKTOP + $GUI_DOCKHEIGHT)
$ButtonMkvmergeDir = GUICtrlCreateButton("1", 244, 104, 29, 21, $BS_NOTIFY)
GUICtrlSetFont(-1, 10, 400, 0, "Wingdings")
GUICtrlSetResizing(-1, $GUI_DOCKRIGHT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Specify output directory or file")
$CheckBoxRunMkvmerge = GUICtrlCreateCheckbox("Run mkvmerge after options file generation", 8, 126, 229, 17)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$CheckBoxFullPaths = GUICtrlCreateCheckbox("Generate full paths in options file", 8, 142, 181, 17)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlCreateGroup("", -99, -99, 1, 1)
GUICtrlCreateTabItem("")
$CheckBoxVerbose = GUICtrlCreateCheckbox("&Verbose mode", 2, 372, 89, 17)
GUICtrlSetFont(-1, 7, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKBOTTOM + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Be more verbose during cu2mkc execution")
$CheckBoxSwitchToOutput = GUICtrlCreateCheckbox("&Switch to messages pane", 2, 389, 133, 17)
GUICtrlSetFont(-1, 7, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKBOTTOM + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Switch to messages pane before running")
$ButtonInf = GUICtrlCreateButton("< &Info", 349, 376, 49, 29, $BS_NOTIFY)
GUICtrlSetFont(-1, 7, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKRIGHT + $GUI_DOCKBOTTOM + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Runs cue2mkc with --info option")
$ButtonInfo = GUICtrlCreateButton("&Help", 400, 376, 41, 29, $BS_NOTIFY)
GUICtrlSetFont(-1, 7, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKRIGHT + $GUI_DOCKBOTTOM + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Runs cue2mkc with --help option")
$ButtonGo = GUICtrlCreateButton("&Run", 452, 376, 85, 29, $BS_NOTIFY)
GUICtrlSetFont(-1, 8, 800, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKRIGHT + $GUI_DOCKBOTTOM + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Runs cue2mkc tool")
$ComboInfoSubject = GUICtrlCreateCombo("", 248, 380, 97, 25, BitOR($CBS_DROPDOWNLIST, $CBS_AUTOHSCROLL))
GUICtrlSetData(-1, "version|usage|formatting|license")
GUICtrlSetResizing(-1, $GUI_DOCKRIGHT + $GUI_DOCKBOTTOM + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$DummyOutput = GUICtrlCreateDummy()
#endregion ### END Koda GUI section ###

Func set_output_mode($nMode)
	Switch $nMode
		Case 0 ; same as inpuut directory
			GUICtrlSetState($InputOuputDir, $GUI_DISABLE)
			GUICtrlSetState($ButtonOutputDirectory, $GUI_DISABLE)

		Case 1, 2 ; directory
			GUICtrlSetState($InputOuputDir, $GUI_ENABLE)
			GUICtrlSetState($InputOuputDir, $GUI_FOCUS)
			GUICtrlSetState($ButtonOutputDirectory, $GUI_ENABLE)
			If StringLen(GUICtrlRead($InputOuputDir)) = 0 Then
				GUICtrlSendToDummy($DummyOutput)
			EndIf
	EndSwitch
EndFunc   ;==>set_output_mode

Func frame_offset_enable($bEnable)
	Local $state = _Iif($bEnable, $GUI_ENABLE, $GUI_DISABLE)
	GUICtrlSetState($LabelFo, $state)
	GUICtrlSetState($InputFo, $state)

	If $bEnable Then
		GUICtrlSetState($InputFo, $GUI_FOCUS)
	EndIf
EndFunc   ;==>frame_offset_enable

Func generate_tags_enable($nMode)
	Switch $nMode
		Case 0
			GUICtrlSetState($CheckBoxT, $GUI_DISABLE)
			GUICtrlSetState($CheckBoxK, $GUI_DISABLE)
			GUICtrlSetState($LabelLang, $GUI_DISABLE)
			GUICtrlSetState($InputLang, $GUI_DISABLE)
		Case 1
			GUICtrlSetState($CheckBoxT, $GUI_ENABLE)
			GUICtrlSetState($CheckBoxK, $GUI_ENABLE)
			GUICtrlSetState($LabelLang, $GUI_ENABLE)
			GUICtrlSetState($InputLang, $GUI_ENABLE)
	EndSwitch
EndFunc   ;==>generate_tags_enable

Func log_msg($message)
	Local $nIndex = _GUICtrlListBox_AddString($ListLog, $message)
	_GUICtrlListBox_SetCaretIndex($ListLog, $nIndex)
EndFunc   ;==>log_msg

Func copy_log_to_clipboard()
	Local $s = ""
	Local $nItems = _GUICtrlListBox_GetCount($ListLog)
	For $i = 0 To $nItems - 1
		$s &= _GUICtrlListBox_GetText($ListLog, $i)
		$s &= @CRLF
	Next
	ClipPut($s)
EndFunc   ;==>copy_log_to_clipboard

Func clear_log()
	_GUICtrlListBox_ResetContent($ListLog)
EndFunc   ;==>clear_log

Func run_wait($sCmd, $nPriority = 1, $sDir = @WorkingDir)

	log_msg($sCmd)
	If StringLen($sDir) = 0 Then
		$sDir = @WorkingDir
	EndIf

	GUISetCursor(15, 1)
	Local $pid = Run($sCmd, $sDir, @SW_HIDE, $STDERR_MERGED)
	If @error Then
		GUISetCursor(2)
		Return SetError(1)
	EndIf

	ProcessSetPriority($pid, $nPriority)

	Local $hProcess = _WinAPI_OpenProcess(0x400, 0, $pid)
	Local $exitCode, $as
	Local $sOut = ""

	While True
		$sOut &= StdoutRead($pid)

		If @error Then
			ExitLoop
		EndIf

		If StringLen($sOut) > 0 Then
			$as = StringSplit(StringReplace($sOut, @LF, ""), @CR, 1)
			If $as[0] > 1 Then
				For $i = 1 To $as[0] - 1
					log_msg($as[$i])
				Next
			EndIf
			If $as[0] > 0 Then
				$sOut = $as[$as[0]]
			EndIf
		EndIf

		Sleep(150)
	WEnd

	log_msg($sOut)

	While True
		$exitCode = DllCall('kernel32.dll', 'ptr', 'GetExitCodeProcess', 'ptr', $hProcess, 'dword*', 0)

		If @error Then
			_WinAPI_CloseHandle($hProcess)
			ProcessClose($pid)
			GUISetCursor(2)
			Return SetError(1, $sOut, 0)
		EndIf

		If $exitCode[2] <> $STILL_ACTIVE Then
			_WinAPI_CloseHandle($hProcess)
			ExitLoop
		EndIf

		Sleep(200)
	WEnd

	GUISetCursor(2)
	Return SetError(0, 0, $exitCode[2])
EndFunc   ;==>run_wait

Func set_default_options()
	GUICtrlSetState($CheckBoxCe, $GUI_CHECKED)
	GUICtrlSetState($CheckBoxEc, $GUI_UNCHECKED)
	GUICtrlSetState($CheckBoxDf, $GUI_CHECKED)
	GUICtrlSetState($CheckBoxCn, $GUI_UNCHECKED)
	GUICtrlSetData($InputFo, 150)
	frame_offset_enable(False)
	GUICtrlSetData($InputAlternateExt, "")
	GUICtrlSetData($InputTf, "%dp% - %dt% - %tt%")
	GUICtrlSetData($InputMf, "%dp% - %dt%")
	GUICtrlSetData($InputLang, "und")
	GUICtrlSetState($CheckBoxHiddenIndexes, $GUI_UNCHECKED)
	GUICtrlSetData($InputDce, "cue")
	GUICtrlSetData($InputDme, "mkc.xml")
	GUICtrlSetData($InputDte, "mkt.xml")
	GUICtrlSetData($InputDoe, "opt.txt")
	_GUICtrlComboBox_SetCurSel($ComboOutputFormat, 1)
	GUICtrlSetState($CheckBoxT, $GUI_UNCHECKED)
	GUICtrlSetState($CheckBoxK, $GUI_UNCHECKED)
	GUICtrlSetState($CheckBoxRunMkvmerge, $GUI_CHECKED)
	_GUICtrlComboBox_SetCurSel($ComboCueSheetEncoding, 2)
	generate_tags_enable(1)
	_GUICtrlComboBox_SetCurSel($ComboTrack01, 1)
	GUICtrlSetState($CheckBoxCq, $GUI_CHECKED)
	GUICtrlSetState($CheckBoxEu, $GUI_UNCHECKED)
	GUICtrlSetState($CheckBoxTc, $GUI_CHECKED)
	GUICtrlSetState($CheckBoxTagUseCdText, $GUI_CHECKED)
	GUICtrlSetState($CheckBoxTagUseCueComments, $GUI_CHECKED)
	GUICtrlSetState($CheckBoxTagUseFromMedia, $GUI_CHECKED)
	GUICtrlSetState($CheckBoxA, $GUI_CHECKED)
	GUICtrlSetState($CheckBoxVerbose, $GUI_UNCHECKED)
	GUICtrlSetState($CheckBoxMerge, $GUI_UNCHECKED)
	GUICtrlSetState($CheckBoxReadMetadata, $GUI_CHECKED)
	GUICtrlSetState($CheckBoxMLang, $GUI_CHECKED)
	GUICtrlSetData($InputMkvmergeDir, get_mkvmerge_dir())
	GUICtrlSetState($CheckBoxFullPaths, $GUI_UNCHECKED)
	GUICtrlSetState($CheckBoxEt, $GUI_CHECKED)
	GUICtrlSetState($CheckBoxEacLog, $GUI_CHECKED)
	GUICtrlSetState($CheckBoxCover, $GUI_CHECKED)
	GUICtrlSetState($CheckBoxRs, $GUI_CHECKED)
	_GUICtrlComboBox_SetCurSel($ComboCueSheetAttachMode, 0)
EndFunc   ;==>set_default_options

Func get_encoding_str($nSel)
	Local $sRet
	Switch $nSel
		Case 0
			$sRet = "local"

		Case 1
			$sRet = "utf8"

		Case 2
			$sRet = "utf8_bom"

		Case 3
			$sRet = "utf16le"

		Case 4
			$sRet = "utf16le_bom"

		Case 5
			$sRet = "utf16be"

		Case 6
			$sRet = "utf16be_bom"
	EndSwitch
	Return SetError(0, 0, $sRet)
EndFunc   ;==>get_encoding_str

Func get_attach_mode_str($nSel)
	Local $sRet
	Switch $nSel
		Case 0
			$sRet = "default"

		Case 1
			$sRet = "none"

		Case 2
			$sRet = "source"

		Case 3
			$sRet = "decoded"

		Case 4
			$sRet = "rendered"

	EndSwitch
	Return SetError(0, 0, $sRet)
EndFunc   ;==>get_attach_mode_str



Func negatable_switch(ByRef $cmdLine, $CheckBoxId, $cmdSwitch)
	$cmdLine &= "-" & _Iif(GUICtrlRead($CheckBoxId) = $GUI_CHECKED, $cmdSwitch, $cmdSwitch & "-")
	$cmdLine &= " "
EndFunc   ;==>negatable_switch

Func negatable_switch_long(ByRef $cmdLine, $CheckBoxId, $cmdSwitch)
	$cmdLine &= "--" & _Iif(GUICtrlRead($CheckBoxId) = $GUI_CHECKED, $cmdSwitch, $cmdSwitch & "-")
	$cmdLine &= " "
EndFunc   ;==>negatable_switch_long

Func read_options()
	Local $s = "", $w

	negatable_switch($s, $CheckBoxCe, "ce")
	negatable_switch($s, $CheckBoxEc, "ec")
	negatable_switch($s, $CheckBoxDf, "df")
	negatable_switch($s, $CheckBoxCn, "cn")
	negatable_switch($s, $CheckBoxMerge, "j")

	If GUICtrlRead($CheckBoxCn) = $GUI_CHECKED Then
		$s &= "-fo "
		$s &= GUICtrlRead($InputFo)
		$s &= " "
	EndIf

	$w = GUICtrlRead($InputAlternateExt)
	If StringLen($w) > 0 Then
		$s &= "-x """
		$s &= $w
		$s &= """ "
	EndIf

	$w = GUICtrlRead($InputTf)
	If StringLen($w) > 0 Then
		$s &= "-f """
		$s &= $w
		$s &= """ "
	EndIf

	$w = GUICtrlRead($InputMf)
	If StringLen($w) > 0 Then
		$s &= "-mf """
		$s &= $w
		$s &= """ "
	EndIf

	$w = GUICtrlRead($InputLang)
	If StringLen($w) > 0 Then
		$s &= "-l "
		$s &= $w
		$s &= " "
	EndIf

	$w = GUICtrlRead($InputMkvmergeDir)
	If StringLen($w) > 0 Then
		$s &= "--mkvmerge-directory """
		$s &= $w
		$s &= """ "
	EndIf

	negatable_switch_long($s, $CheckBoxHiddenIndexes, "hidden-indexes")

	$w = GUICtrlRead($InputDce)
	If StringLen($w) > 0 Then
		$s &= "--cue-sheet-file-extension "
		$s &= $w
		$s &= " "
	EndIf

	$w = GUICtrlRead($InputDme)
	If StringLen($w) > 0 Then
		$s &= "--matroska-chapters-file-extension "
		$s &= $w
		$s &= " "
	EndIf

	$w = GUICtrlRead($InputDte)
	If StringLen($w) > 0 Then
		$s &= "--matroska-tags-file-extension "
		$s &= $w
		$s &= " "
	EndIf

	$w = GUICtrlRead($InputDoe)
	If StringLen($w) > 0 Then
		$s &= "--mkvmerge-options-file-extension "
		$s &= $w
		$s &= " "
	EndIf

	$w = _GUICtrlComboBox_GetCurSel($ComboOutputFormat)
	Switch $w
		Case 0
			$s &= "-m cuesheet -e "
			$s &= get_encoding_str(_GUICtrlComboBox_GetCurSel($ComboCueSheetEncoding))

		Case 1
			$s &= "-m matroska "
			negatable_switch($s, $CheckBoxT, "t")
			negatable_switch($s, $CheckBoxK, "k")
			negatable_switch($s, $CheckBoxEu, "eu")
			negatable_switch($s, $CheckBoxTc, "tc")
			negatable_switch_long($s, $CheckBoxTagUseCdText, "use-cdtext-tags")
			negatable_switch_long($s, $CheckBoxTagUseCueComments, "use-cue-comments-tags")
			negatable_switch_long($s, $CheckBoxTagUseFromMedia, "use-media-tags")

		Case 2
			$s &= "-m wav2img -e "
			$s &= get_encoding_str(_GUICtrlComboBox_GetCurSel($ComboCueSheetEncoding))

	EndSwitch

	$s &= " "

	$w = _GUICtrlComboBox_GetCurSel($ComboTrack01)
	Switch $w
		Case 0
			$s &= "-t1i0 "

		Case 1
			$s &= "-t1i0- "
	EndSwitch

	negatable_switch($s, $CheckBoxCq, "cq")
	negatable_switch($s, $CheckBoxEt, "et")
	negatable_switch($s, $CheckBoxA, "a")

	negatable_switch_long($s, $CheckBoxSingleMediaFile, "single-media-file")
	negatable_switch_long($s, $CheckBoxReadMetadata, "read-media-tags")
	negatable_switch_long($s, $CheckBoxMLang, "use-mlang")
	negatable_switch_long($s, $CheckBoxFullPaths, "use-full-paths")
	negatable_switch_long($s, $CheckBoxEacLog, "attach-eac-log")
	negatable_switch_long($s, $CheckBoxCover, "attach-cover")
	negatable_switch($s, $CheckBoxRs, "rs")
	negatable_switch_long($s, $CheckBoxRunMkvmerge, "run-mkvmerge")

	$s &= " --cue-sheet-attach-mode "
	$s &= get_attach_mode_str(_GUICtrlComboBox_GetCurSel($ComboCueSheetAttachMode))
	$s &= " "

	If GUICtrlRead($CheckBoxVerbose) = $GUI_CHECKED Then
		$s &= " --verbose"
		$s &= " "
	EndIf

	Return StringTrimRight($s, 1)
EndFunc   ;==>read_options

Func get_cmd_line()
	Local $sExe = $CUE2MKC_EXE
	Local $sParams = "", $s
	Local $sOptions = read_options()

	Local $sInput = "", $w = 0, $s, $hChild
	Local $hItem = _GUICtrlTreeView_GetFirstItem(GUICtrlGetHandle($TreeViewInputFiles))
	While $hItem <> 0
		$w += 1

		$s = _GUICtrlTreeView_GetText(GUICtrlGetHandle($TreeViewInputFiles), $hItem)
		$hChild = _GUICtrlTreeView_GetFirstChild(GUICtrlGetHandle($TreeViewInputFiles), $hItem)
		While $hChild <> 0
			$s &= ';'
			$s &= _GUICtrlTreeView_GetText(GUICtrlGetHandle($TreeViewInputFiles), $hChild)
			$hChild = _GUICtrlTreeView_GetNextChild(GUICtrlGetHandle($TreeViewInputFiles), $hChild)
		WEnd

		$sInput &= """"
		$sInput &= $s
		$sInput &= """ "

		$hItem = _GUICtrlTreeView_GetNextSibling(GUICtrlGetHandle($TreeViewInputFiles), $hItem)
	WEnd

	Local $sOutputFile = "", $sOutputDir = ""
	Switch _GUICtrlComboBox_GetCurSel($ComboOutput)
		Case 1 ; directory
			$sOutputDir = GUICtrlRead($InputOuputDir)

		Case 2 ; file
			$sOutputFile = GUICtrlRead($InputOuputDir)
	EndSwitch

	$s = """" & $sExe & """ " & $sOptions
	If StringLen($sOutputFile) > 0 Then
		$s &= " -o """
		$s &= $sOutputFile
		$s &= """"
	EndIf

	If StringLen($sOutputDir) > 0 Then
		$s &= " -od """
		$s &= $sOutputDir
		$s &= """"
	EndIf

	If $w > 0 Then
		$s &= " "
		$s &= $sInput
	EndIf

	Return SetError(0, 0, $s)
EndFunc   ;==>get_cmd_line

Func output_dir_or_file_dlg($nMode)
	Local $s = GUICtrlRead($InputOuputDir)
	If StringLen($s) = 0 Or Not FileExists($s) Then
		$s = @WorkingDir
	ElseIf FileExists($s) Then
		If Not is_directory($s) Then
			$s = get_directory($s)
		EndIf
	EndIf

	Switch $nMode
		Case 2 ; file
			$s = FileSaveDialog("Specify output file", $s, $XML_FILTER, 2 + 16, "", $FormMain)
			If Not @error Then
				Return SetError(0, 1, $s)
			Else
				Return SetError(1, 1)
			EndIf

		Case 1 ; directory
			$s = FileSelectFolder("Specify output directory", "", 1 + 2 + 4, $s, $FormMain)
			If Not @error Then
				Return SetError(0, 1, $s)
			Else
				Return SetError(1, 1)
			EndIf

		Case Else
			Return SetError(1)
	EndSwitch
EndFunc   ;==>output_dir_or_file_dlg

Func make_mask($sPath)
	Local $sDrive, $sDir, $sFName, $sExt
	_PathSplit($sPath, $sDrive, $sDir, $sFName, $sExt)
	Local $sMask = _PathMake($sDrive, $sDir, "*", $sExt)
	Return SetError(0, 0, $sMask)
EndFunc   ;==>make_mask

Func add_main_item($s)
	Local $hItem = _GUICtrlTreeView_Add(GUICtrlGetHandle($TreeViewInputFiles), 0, $s)
	If $hItem <> 0 Then
		If @Compiled Then
			_GUICtrlTreeView_SetIcon(GUICtrlGetHandle($TreeViewInputFiles), $hItem, @ScriptFullPath, 4)
		Else
			_GUICtrlTreeView_SetIcon(GUICtrlGetHandle($TreeViewInputFiles), $hItem, @ScriptDir & "\icons\cue_file.ico")
		EndIf
		_GUICtrlTreeView_SelectItem(GUICtrlGetHandle($TreeViewInputFiles), $hItem)
	EndIf
EndFunc   ;==>add_main_item

Func add_child_item($hParent, $s)
	Local $hItem = _GUICtrlTreeView_AddChild(GUICtrlGetHandle($TreeViewInputFiles), $hParent, $s)
	If $hItem <> 0 Then
		If @Compiled Then
			_GUICtrlTreeView_SetIcon(GUICtrlGetHandle($TreeViewInputFiles), $hItem, @ScriptFullPath, 7)
		Else
			_GUICtrlTreeView_SetIcon(GUICtrlGetHandle($TreeViewInputFiles), $hItem, @ScriptDir & "\icons\media_file.ico")
		EndIf
		_GUICtrlTreeView_SelectItem(GUICtrlGetHandle($TreeViewInputFiles), $hItem)
	EndIf
EndFunc   ;==>add_child_item

Func get_full_file_select($s)
	Local $as = StringSplit($s, "|", 1)
	If $as[0] = 0 Then Return SetError(1, $as[0])
	If $as[0] = 1 Then
		Local $ar[1] = [$as[1]]
		Return SetError(0, $as[0], $ar)
	EndIf

	Local $sDir = $as[1]
	;ConsoleWrite($as[0] & @CRLF)
	Local $ar[$as[0] - 1]
	For $i = 2 To $as[0]
		$ar[$i - 2] = $sDir & "\" & $as[$i]
	Next
	Return SetError(0, 0, $ar)
EndFunc   ;==>get_full_file_select

log_msg("Simple frontend to cue2mkc utility.")
log_msg("cue2mkc version: " & FileGetVersion($CUE2MKC_EXE) & ".")
If @Compiled Then
	log_msg("Script version: " & FileGetVersion(@AutoItExe, "CompiledScript") & _Iif(@AutoItX64, " (x64)", "") & ".")
Else
	log_msg("AutoIt version: " & @AutoItVersion & _Iif(@AutoItX64, " (x64)", "") & ".")
EndIf
log_msg("Author: roed@onet.eu.")
log_msg("Icons taken from Primo Icons Set - http://www.webdesignerdepot.com/2009/07/200-free-exclusive-vector-icons-primo/");
log_msg("")

If Not $CUE2MKC_EXE_EXISTS Then
	;GUICtrlSetState($LabelPdftoppm, $GUI_DISABLE)
	log_msg("Warning: cue2mkc tool not found.")
EndIf

$tip = "Try to correct ""simple 'quotation' marks"" inside strings." & @CRLF & _
		"Examples:" & @CRLF & _
		@TAB & "„polish ‚quotation’ marks”" & @CRLF & _
		@TAB & "“english (U.S.) ‘quotation’ marks”" & @CRLF & _
		@TAB & "« french ‹ angle › marks »" & @CRLF & _
		@TAB & "„german ‚quotation‘ marks“"
GUICtrlSetTip($CheckBoxCq, $tip, "Correct ""simple 'quotation' marks""", 1)

$tip = "CUE2MKC simply assumes that EAC log file is a file" & @CRLF & _
		"with the same name that cuesheet but with “log” extension" & @CRLF & _
		@CRLF & _
		"For example EAC log file for file “c:\xxx\yyy.cue” is “c:\xxx\yyy.log”"
GUICtrlSetTip($CheckBoxEacLog, $tip, "Attach EAC log(s)", 1)

$tip = "Following files are searching in the same directory where cuesheet resides:" & @CRLF & _
		@TAB & "cover.*" & @CRLF & _
		@TAB & "front.*" & @CRLF & _
		@TAB & "folder.*" & @CRLF & _
		@TAB & "picture.*" & @CRLF & _
		@CRLF & _
		"Possible file extensions (case insensitive):" & @CRLF & _
		@TAB & "jpg" & @CRLF & _
		@TAB & "jpeg" & @CRLF & _
		@TAB & "png";
GUICtrlSetTip($CheckBoxCover, $tip, "Attach cover image", 1)

If @Compiled Then
	GUISetIcon(@ScriptFullPath, -1, $FormMain)
	GUICtrlSetStyle($ButtonInputAdd, $BS_ICON)
	GUICtrlSetImage($ButtonInputAdd, @ScriptFullPath, -6, 1)
	GUICtrlSetStyle($ButtonDataFile, $BS_ICON)
	GUICtrlSetImage($ButtonDataFile, @ScriptFullPath, -9, 1)
	GUICtrlSetStyle($ButtonInputDelete, $BS_ICON)
	GUICtrlSetImage($ButtonInputDelete, @ScriptFullPath, -7, 1)
	GUICtrlSetStyle($ButtonMakeMask, $BS_ICON)
	GUICtrlSetImage($ButtonMakeMask, @ScriptFullPath, -10, 1)
Else
	GUISetIcon(@ScriptDir & "\icons\cd_mka.ico", -1, $FormMain)
	GUICtrlSetStyle($ButtonInputAdd, $BS_ICON)
	GUICtrlSetImage($ButtonInputAdd, @ScriptDir & "\icons\cue_file_add.ico", -1, 1)
	GUICtrlSetStyle($ButtonDataFile, $BS_ICON)
	GUICtrlSetImage($ButtonDataFile, @ScriptDir & "\icons\media_file_add.ico", -1, 1)
	GUICtrlSetStyle($ButtonInputDelete, $BS_ICON)
	GUICtrlSetImage($ButtonInputDelete, @ScriptDir & "\icons\cue_file_remove.ico", -1, 1)
	GUICtrlSetStyle($ButtonMakeMask, $BS_ICON)
	GUICtrlSetImage($ButtonMakeMask, @ScriptDir & "\icons\file_mask.ico", -1, 1)
EndIf
set_default_options()
GUICtrlSetData($InputOuputDir, @MyDocumentsDir)
_GUICtrlComboBox_SetCurSel($ComboOutput, 0)
_GUICtrlComboBox_SetCurSel($ComboInfoSubject, 0)
set_output_mode(0)
_GUICtrlListBox_SetHorizontalExtent($ListLog, 5000)
GUICtrlSetState($TreeViewInputFiles, $GUI_DROPACCEPTED)
GUICtrlSetState($CheckBoxSwitchToOutput, $GUI_CHECKED)
GUICtrlSetState($PaneInputOutput, $GUI_SHOW)
GUISetState(@SW_SHOW)

While True
	$nMsg = GUIGetMsg()
	Switch $nMsg
		Case $GUI_EVENT_CLOSE
			Exit

		Case $GUI_EVENT_DROPPED
			If @GUI_DropId = $TreeViewInputFiles Then
				$s = @GUI_DragFile
				If StringLen($s) > 0 Then
					add_main_item($s)
				EndIf
			EndIf

		Case $ButtonInputAdd
			$s = FileOpenDialog("Specify input file", @WorkingDir, $CUE_FILTER, 1 + 2 + 4, "", $FormMain)
			If Not @error Then
				$as = get_full_file_select($s)
				If Not @error Then
					_GUICtrlTreeView_BeginUpdate(GUICtrlGetHandle($TreeViewInputFiles))
					For $s In $as
						add_main_item($s)
					Next
					_GUICtrlTreeView_EndUpdate(GUICtrlGetHandle($TreeViewInputFiles))
				EndIf
			EndIf

		Case $DummyOutput
			$s = output_dir_or_file_dlg(_GUICtrlComboBox_GetCurSel($ComboOutput))
			If Not @error Then
				GUICtrlSetData($InputOuputDir, $s)
			EndIf

		Case $ButtonOutputDirectory
			GUICtrlSendToDummy($DummyOutput)

		Case $ComboOutput
			set_output_mode(_GUICtrlComboBox_GetCurSel($ComboOutput))

		Case $CheckBoxCn
			frame_offset_enable(_Iif(GUICtrlRead($CheckBoxCn) = $GUI_CHECKED, True, False))

		Case $ButtonGo
			$s = get_cmd_line()
			If Not @error Then
				If GUICtrlRead($CheckBoxSwitchToOutput) = $GUI_CHECKED Then
					GUICtrlSetState($OutputPane, $GUI_SHOW)
				EndIf
				$nRet = run_wait($s)
				If @error Then
					log_msg("Error executing command.")
				Else
					log_msg(StringFormat($APP_EXIT_CODE, $nRet))
				EndIf
			EndIf

		Case $ButtonInfo
			$sCmd = StringFormat("""%s"" --help", $CUE2MKC_EXE)
			$nRet = run_wait($sCmd)
			If @error Then
				log_msg("Error executing command.")
			Else
				log_msg(StringFormat($APP_EXIT_CODE, $nRet))
			EndIf

		Case $ButtonInf
			$sCmd = StringFormat("""%s"" --info=%s", $CUE2MKC_EXE, GUICtrlRead($ComboInfoSubject))
			$nRet = run_wait($sCmd)
			If @error Then
				log_msg("Error executing command.")
			Else
				log_msg(StringFormat($APP_EXIT_CODE, $nRet))
			EndIf

		Case $ButtonMsgCopy
			copy_log_to_clipboard()

		Case $ButtonClearLog
			clear_log()

		Case $ButtonMakeMask
			$hItem = _GUICtrlTreeView_GetSelection(GUICtrlGetHandle($TreeViewInputFiles))
			If $hItem <> 0 Then
				_GUICtrlTreeView_SetText(GUICtrlGetHandle($TreeViewInputFiles), $hItem, _
						make_mask(_GUICtrlTreeView_GetText(GUICtrlGetHandle($TreeViewInputFiles), $hItem)))
			EndIf

		Case $ButtonInputDelete
			$hItem = _GUICtrlTreeView_GetSelection(GUICtrlGetHandle($TreeViewInputFiles))
			If $hItem <> 0 Then
				_GUICtrlTreeView_Delete(GUICtrlGetHandle($TreeViewInputFiles), $hItem)
			EndIf

		Case $ComboOutputFormat
			generate_tags_enable(_GUICtrlComboBox_GetCurSel($ComboOutputFormat))

		Case $ButtonDataFile
			$hItem = _GUICtrlTreeView_GetSelection(GUICtrlGetHandle($TreeViewInputFiles))
			If $hItem <> 0 And _GUICtrlTreeView_Level(GUICtrlGetHandle($TreeViewInputFiles), $hItem) = 0 Then
				$s = FileOpenDialog("Specify media file", @WorkingDir, $MEDIA_FILTER, 1 + 2 + 4, "", $FormMain)
				If Not @error Then
					$as = get_full_file_select($s)
					If Not @error Then
						_GUICtrlTreeView_BeginUpdate(GUICtrlGetHandle($TreeViewInputFiles))
						For $s In $as
							add_child_item($hItem, $s)
						Next
						_GUICtrlTreeView_EndUpdate(GUICtrlGetHandle($TreeViewInputFiles))
					EndIf
				EndIf
			EndIf

		Case $ButtonMkvmergeDir
			$s = FileSelectFolder("Specify directory where mkvmerge tool resides", "", 1 + 2 + 4, GUICtrlRead($InputMkvmergeDir), $FormMain)
			If Not @error Then
				GUICtrlSetData($InputMkvmergeDir, $s)
			EndIf

	EndSwitch
WEnd

Exit 0

;
; $Id$
;
