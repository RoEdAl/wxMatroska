#NoTrayIcon
#Region ;**** Directives created by AutoIt3Wrapper_GUI ****
#AutoIt3Wrapper_Icon=icons/cd_mka.ico
#AutoIt3Wrapper_Compression=4
#AutoIt3Wrapper_Res_Comment=This is frontend to cue2mkc tool
#AutoIt3Wrapper_Res_Description=Graphical user interface for cue2mkc command line tool
#AutoIt3Wrapper_Res_Fileversion=0.1.0.35
#AutoIt3Wrapper_Res_FileVersion_AutoIncrement=y
#AutoIt3Wrapper_Res_LegalCopyright=Simplified BSD License - http://www.opensource.org/licenses/bsd-license.html
#AutoIt3Wrapper_Res_SaveSource=y
#AutoIt3Wrapper_Res_Field=Company|Edmunt Pienkowsky - roed@onet.eu
#AutoIt3Wrapper_Res_Icon_Add=icons/cue_file.ico
#AutoIt3Wrapper_Res_Icon_Add=icons/cue_file_add.ico
#AutoIt3Wrapper_Res_Icon_Add=icons/cue_file_remove.ico
#AutoIt3Wrapper_Res_Icon_Add=icons/media_file.ico
#AutoIt3Wrapper_Res_Icon_Add=icons/media_file_add.ico
#AutoIt3Wrapper_Res_Icon_Add=icons/file_mask.ico
#AutoIt3Wrapper_Run_Tidy=y
#EndRegion ;**** Directives created by AutoIt3Wrapper_GUI ****
;
; cue2mkcgui - simple frontend cue2mkc utility.
;

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

Func get_tool_full_path($sExeName)
	Local $sExe = $sExeName & ".exe"
	Local $sPath = @ScriptDir & "\" & $sExe
	If FileExists($sPath) Then
		Return SetError(0, 1, $sPath)
	Else
		Return SetError(0, 0, $sExe)
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
	Exit 1
EndIf

Dim Const $CUE_FILTER = "CUE files (*.cue)|Text files (*.txt)|Audio files (*.flac;*.ape;*.wv;*.wav;*.aiff;*.tta)|All files (*)"
Dim Const $XML_FILTER = "XML files (*.xml)|Matroska chapters XML files (*.mkc.xml)|Text files (*.txt)|All files (*)"
Dim Const $MEDIA_FILTER = "Audio files (*.flac;*.ape;*.wv;*.wav;*.aiff;*.tta)|All files (*)"
Dim Const $STILL_ACTIVE = 259
Dim Const $APP_EXIT_CODE = "Exit code: %d."

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

#Region ### START Koda GUI section ### Form=C:\Documents and Settings\VBox\My Documents\Visual Studio 2010\Projects\wxMatroska.bitbucket\gui\cue2mkcgui.kxf
$FormMain = GUICreate("cue2mkc GUI", 540, 416, -1, -1, BitOR($WS_MAXIMIZEBOX, $WS_MINIMIZEBOX, $WS_SIZEBOX, $WS_THICKFRAME, $WS_SYSMENU, $WS_CAPTION, $WS_OVERLAPPEDWINDOW, $WS_TILEDWINDOW, $WS_POPUP, $WS_POPUPWINDOW, $WS_GROUP, $WS_TABSTOP, $WS_BORDER, $WS_CLIPSIBLINGS), BitOR($WS_EX_ACCEPTFILES, $WS_EX_WINDOWEDGE))
GUISetFont(8, 400, 0, "Microsoft Sans Serif")
$DummyOutput = GUICtrlCreateDummy()
$MainTab = GUICtrlCreateTab(0, 0, 541, 377, $TCS_MULTILINE)
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKRIGHT + $GUI_DOCKTOP + $GUI_DOCKBOTTOM)
$PaneInputOutput = GUICtrlCreateTabItem("&Source && destination")
$LabelFiles = GUICtrlCreateLabel("&Sources list:", 4, 42, 80, 21, $SS_CENTERIMAGE)
GUICtrlSetFont(-1, 8, 800, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$TreeViewInputFiles = GUICtrlCreateTreeView(4, 67, 529, 234, BitOR($TVS_HASBUTTONS, $TVS_HASLINES, $TVS_DISABLEDRAGDROP, $TVS_SHOWSELALWAYS, $TVS_FULLROWSELECT, $WS_GROUP, $WS_TABSTOP), $WS_EX_STATICEDGE)
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKRIGHT + $GUI_DOCKTOP + $GUI_DOCKBOTTOM)
$ButtonInputAdd = GUICtrlCreateButton("+", 5, 304, 29, 21, 0)
GUICtrlSetFont(-1, 14, 800, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKBOTTOM + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Add specified file or file mask to list")
$ButtonInputDelete = GUICtrlCreateButton("-", 40, 304, 29, 21, 0)
GUICtrlSetFont(-1, 14, 800, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKBOTTOM + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Delete selected items from list")
$ButtonMakeMask = GUICtrlCreateButton("�", 76, 304, 29, 21, 0)
GUICtrlSetFont(-1, 14, 800, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKBOTTOM + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Make file mask with selected items")
$ButtonDataFile = GUICtrlCreateButton("+", 110, 304, 29, 21, 0)
GUICtrlSetFont(-1, 12, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKBOTTOM + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Add media file to selected cue sheet")
$LabelOutput = GUICtrlCreateLabel("&Destination:", 4, 329, 80, 17, $SS_CENTERIMAGE)
GUICtrlSetFont(-1, 8, 800, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKBOTTOM + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$ComboOutput = GUICtrlCreateCombo("", 4, 349, 137, 25, BitOR($CBS_DROPDOWNLIST, $CBS_AUTOHSCROLL))
GUICtrlSetData(-1, "Input directory|Directory|File")
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKBOTTOM + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$InputOuputDir = GUICtrlCreateInput("", 144, 349, 353, 21)
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKRIGHT + $GUI_DOCKBOTTOM + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Destination directory")
$ButtonOutputDirectory = GUICtrlCreateButton("1", 501, 349, 29, 21, 0)
GUICtrlSetFont(-1, 10, 400, 0, "Wingdings")
GUICtrlSetResizing(-1, $GUI_DOCKRIGHT + $GUI_DOCKBOTTOM + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Specify output directory or file")
$OutputPane = GUICtrlCreateTabItem("&Messages")
$LabelLog = GUICtrlCreateLabel("&Messages:", 4, 42, 71, 21, $SS_CENTERIMAGE)
GUICtrlSetFont(-1, 8, 800, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$ListLog = GUICtrlCreateList("", 4, 64, 529, 276, BitOR($LBS_USETABSTOPS, $LBS_NOINTEGRALHEIGHT, $LBS_NOSEL, $WS_HSCROLL, $WS_VSCROLL), $WS_EX_STATICEDGE)
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetBkColor(-1, 0xECE9D8)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKRIGHT + $GUI_DOCKTOP + $GUI_DOCKBOTTOM)
GUICtrlSetTip(-1, "Application messages")
$ButtonMsgCopy = GUICtrlCreateButton("&Copy", 4, 343, 61, 29, 0)
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKBOTTOM + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Copy messages to clipboard")
$ButtonClearLog = GUICtrlCreateButton("Clear", 68, 343, 61, 29, 0)
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKBOTTOM + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Clear log")
$OptionsPane = GUICtrlCreateTabItem("&General options")
GUICtrlSetState(-1, $GUI_SHOW)
$GroupGeneral = GUICtrlCreateGroup("&General", 4, 45, 265, 89, -1, $WS_EX_TRANSPARENT)
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$CheckBoxMerge = GUICtrlCreateCheckbox("Merge mode", 8, 63, 85, 17)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Merge all input files into one cue sheet (requires MediaInfo lubrary)")
$CheckBoxCq = GUICtrlCreateCheckbox("Correct ""simple 'quotation' marks"" inside strings", 8, 80, 261, 17)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$CheckBoxA = GUICtrlCreateCheckbox("Abort when conversion errors occurs", 8, 96, 201, 17)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$CheckBoxTc = GUICtrlCreateCheckbox("Parse tags from cue sheet comments", 8, 112, 197, 17)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Try to parse tags from cue sheet comments")
GUICtrlCreateGroup("", -99, -99, 1, 1)
$GroupInputOptions = GUICtrlCreateGroup("&Input", 5, 134, 265, 105)
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$CheckBoxEc = GUICtrlCreateCheckbox("Embedded cue sheet", 9, 151, 129, 17)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Try to read embedded cue sheet (requires MediaInfo library)")
$CheckBoxSingleMediaFile = GUICtrlCreateCheckbox("Media file(s) without cuesheet", 9, 171, 169, 17)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Assume input file as media file without cue sheet")
$CheckBoxDf = GUICtrlCreateCheckbox("Use media file(s)", 9, 190, 97, 17)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Use media file(s) to calculate end time of chapters")
$LabelAlternateExt = GUICtrlCreateLabel("Alternate media files extensions:", 9, 208, 153, 21, $SS_CENTERIMAGE)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Comma separated list of alternate media extensions")
$InputAlternateExt = GUICtrlCreateInput("", 163, 208, 101, 21)
GUICtrlSetLimit(-1, 150)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Comma-separated list of alternate extensions of media files")
GUICtrlCreateGroup("", -99, -99, 1, 1)
$GroupOutputOptions = GUICtrlCreateGroup("&Output", 4, 239, 265, 85)
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$LabelOutputFormat = GUICtrlCreateLabel("Format:", 12, 254, 43, 21, $SS_CENTERIMAGE)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$ComboOutputFormat = GUICtrlCreateCombo("", 56, 254, 177, 25, BitOR($CBS_DROPDOWNLIST, $CBS_AUTOHSCROLL))
GUICtrlSetData(-1, "cue sheet|Matroska chapters XML file")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$CheckBoxT = GUICtrlCreateCheckbox("Generate tags XML file", 26, 277, 157, 17)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$CheckBoxOf = GUICtrlCreateCheckbox("Generate options file for mkvmerge", 26, 297, 213, 17)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlCreateGroup("", -99, -99, 1, 1)
$ChapterOptionsPane = GUICtrlCreateTabItem("&Chapters file generation options")
$GroupChapterOptions = GUICtrlCreateGroup("&Chapters file", 4, 42, 293, 141, -1, $WS_EX_TRANSPARENT)
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$CheckBoxCe = GUICtrlCreateCheckbox("Calculate end time of chapters if possible", 8, 56, 217, 17)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$CheckBoxUc = GUICtrlCreateCheckbox("Calculate chapter's end time from next chapter", 8, 72, 276, 17)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "If track's end time is unknown set it to next track position using frame offset")
$LabelFo = GUICtrlCreateLabel("Offset:", 24, 93, 35, 21, $SS_CENTERIMAGE)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$InputFo = GUICtrlCreateInput("150", 61, 93, 65, 21, BitOR($ES_AUTOHSCROLL, $ES_NUMBER))
GUICtrlSetLimit(-1, 4)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Frame offset")
$LabelTrack01_1 = GUICtrlCreateLabel("For track 01 assume index", 8, 116, 128, 21, $SS_CENTERIMAGE)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$ComboTrack01 = GUICtrlCreateCombo("", 140, 116, 41, 25, BitOR($CBS_DROPDOWNLIST, $CBS_AUTOHSCROLL), $WS_EX_CLIENTEDGE)
GUICtrlSetData(-1, "00|01", "01")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$LabelTrack01_2 = GUICtrlCreateLabel("as beginning of track", 185, 116, 103, 21, $SS_CENTERIMAGE)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$CheckBoxR = GUICtrlCreateCheckbox("Round down track end time to full frames", 8, 140, 217, 17)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$CheckBoxHi = GUICtrlCreateCheckbox("Convert indexes to hidden chapters", 8, 158, 217, 17)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlCreateGroup("", -99, -99, 1, 1)
$GroupFormatting = GUICtrlCreateGroup("&Formatting", 4, 184, 289, 89)
$LabelTf = GUICtrlCreateLabel("Track title format:", 9, 200, 86, 21, $SS_CENTERIMAGE)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$InputTf = GUICtrlCreateInput("%dp% - %dt% - %tt%", 140, 200, 145, 21)
GUICtrlSetLimit(-1, 150)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$LabelMf = GUICtrlCreateLabel("MKA tilte format:", 8, 224, 86, 21, $SS_CENTERIMAGE)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$InputMf = GUICtrlCreateInput("%dp% - %dt%", 140, 224, 145, 21)
GUICtrlSetLimit(-1, 150)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$LabelLang = GUICtrlCreateLabel("Language:", 9, 247, 117, 21, $SS_CENTERIMAGE)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$InputLang = GUICtrlCreateInput("eng", 140, 247, 41, 21)
GUICtrlSetLimit(-1, 3)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "This option also affects tags file generation")
GUICtrlCreateGroup("", -99, -99, 1, 1)
$TagOptionsPane = GUICtrlCreateTabItem("&Tags file generation options")
$GroupTagFile = GUICtrlCreateGroup("&Tags file", 4, 41, 225, 37, -1, $WS_EX_TRANSPARENT)
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$CheckBoxEu = GUICtrlCreateCheckbox("Generate edition UID", 8, 57, 129, 17)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlCreateGroup("", -99, -99, 1, 1)
$GroupIgnoredTags = GUICtrlCreateGroup("&Ignored tags", 4, 79, 225, 77, -1, $WS_EX_TRANSPARENT)
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$CheckBoxTagIgnoreCdText = GUICtrlCreateCheckbox("Ignore CD-TEXT tags", 8, 95, 125, 17)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Ignore all CD-TEXT tags")
$CheckBoxTagIgnoreCueComments = GUICtrlCreateCheckbox("Ignore tags from cuesheet comments", 8, 113, 205, 17)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Ignore all tags taken from cuesheet comments")
$CheckBoxTagIgnoreFromMedia = GUICtrlCreateCheckbox("Ignore tags from media file(s)", 8, 132, 161, 17)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Ignore all tags taken from media's metadata")
GUICtrlCreateGroup("", -99, -99, 1, 1)
$OtherOptionsPane = GUICtrlCreateTabItem("&Advanced options")
$GroupFileExtensions = GUICtrlCreateGroup("&File extensions", 4, 42, 225, 113, -1, $WS_EX_TRANSPARENT)
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$LabelDce = GUICtrlCreateLabel("Cue sheet:", 8, 58, 67, 21, $SS_CENTERIMAGE)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$InputDce = GUICtrlCreateInput("cue", 109, 58, 53, 21)
GUICtrlSetLimit(-1, 50)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$LabelDme = GUICtrlCreateLabel("Matroska chapters:", 8, 80, 95, 21, $SS_CENTERIMAGE)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$InputDme = GUICtrlCreateInput("mkc.xml", 109, 80, 53, 21)
GUICtrlSetLimit(-1, 50)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$LabelDte = GUICtrlCreateLabel("Matroska tags:", 8, 103, 95, 21, $SS_CENTERIMAGE)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$InputDte = GUICtrlCreateInput("mkt.xml", 109, 103, 53, 21)
GUICtrlSetLimit(-1, 50)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$LabelDoe = GUICtrlCreateLabel("mkvmerge options:", 8, 125, 95, 21, $SS_CENTERIMAGE)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$InputDoe = GUICtrlCreateInput("mmc.txt", 109, 125, 53, 21)
GUICtrlSetLimit(-1, 50)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlCreateGroup("", -99, -99, 1, 1)
$GroupMLang = GUICtrlCreateGroup("MLang", 4, 157, 225, 45)
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$CheckBoxMLang = GUICtrlCreateCheckbox("Use MLang library", 8, 175, 117, 17)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlCreateGroup("", -99, -99, 1, 1)
$GroupMetadaReadingOptions = GUICtrlCreateGroup("&Metadata reading options", 4, 206, 225, 69)
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$LabelFlacCueSheetReadMode = GUICtrlCreateLabel("FLAC:", 8, 244, 35, 21, $SS_CENTERIMAGE)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Method of reading cue sheet from FLAC container")
$ComboFlacMode = GUICtrlCreateCombo("", 45, 244, 165, 25, BitOR($CBS_DROPDOWNLIST, $CBS_AUTOHSCROLL), $WS_EX_CLIENTEDGE)
GUICtrlSetData(-1, "use MediaInfo only|tag first|comment first|tag only|comment only||")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$CheckBoxReadMetadata = GUICtrlCreateCheckbox("Read metadata", 8, 224, 101, 17)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Read metadata from media file")
GUICtrlCreateGroup("", -99, -99, 1, 1)
$GroupCueSheet = GUICtrlCreateGroup("Cue &sheet generation options", 5, 276, 225, 57, -1, $WS_EX_TRANSPARENT)
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$LabelCueSheetEncoding = GUICtrlCreateLabel("Encoding:", 9, 297, 55, 21, $SS_CENTERIMAGE)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$ComboCueSheetEncoding = GUICtrlCreateCombo("", 70, 297, 141, 25, BitOR($CBS_DROPDOWNLIST, $CBS_AUTOHSCROLL), $WS_EX_CLIENTEDGE)
GUICtrlSetData(-1, "default|UTF-8|UTF-8 with BOM|UTF-16|UTF-16 with BOM")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlCreateGroup("", -99, -99, 1, 1)
GUICtrlCreateTabItem("")
$CheckBoxVerbose = GUICtrlCreateCheckbox("&Verbose mode", 2, 380, 97, 17)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKBOTTOM + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Be more verbose during cu2mkc execution")
$CheckBoxSwitchToOutput = GUICtrlCreateCheckbox("&Switch to messages pane", 2, 397, 141, 17)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKBOTTOM + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Switch to messages pane before running")
$ButtonLicense = GUICtrlCreateButton("&License", 341, 384, 65, 29, 0)
GUICtrlSetFont(-1, 7, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKRIGHT + $GUI_DOCKBOTTOM + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Runs cue2mkc with --licese option")
$ButtonInfo = GUICtrlCreateButton("&Help", 408, 384, 41, 29, 0)
GUICtrlSetFont(-1, 7, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKRIGHT + $GUI_DOCKBOTTOM + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Runs cue2mkc with --help option")
$ButtonGo = GUICtrlCreateButton("&Run", 452, 384, 85, 29, 0)
GUICtrlSetFont(-1, 8, 800, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKRIGHT + $GUI_DOCKBOTTOM + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Run cue2mkc tool")
#EndRegion ### END Koda GUI section ###

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
			GUICtrlSetState($CheckBoxOf, $GUI_DISABLE)

		Case 1
			GUICtrlSetState($CheckBoxT, $GUI_ENABLE)
			GUICtrlSetState($CheckBoxOf, $GUI_ENABLE)
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

Func run_wait($sCmd, $nPriority = 2, $sDir = @WorkingDir)

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
			$as = StringSplit($sOut, @CRLF, 1)
			If $as[0] > 1 Then
				For $i = 1 To $as[0] - 1
					log_msg($as[$i])
				Next
			EndIf
			If $as[0] > 0 Then
				$sOut = $as[$as[0]]
			EndIf
		EndIf

		Sleep(100)
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
	GUICtrlSetState($CheckBoxUc, $GUI_UNCHECKED)
	GUICtrlSetData($InputFo, 150)
	frame_offset_enable(False)
	GUICtrlSetData($InputAlternateExt, "")
	GUICtrlSetData($InputTf, "%dp% - %dt% - %tt%")
	GUICtrlSetData($InputMf, "%dp% - %dt%")
	GUICtrlSetData($InputLang, "eng")
	GUICtrlSetState($CheckBoxR, $GUI_UNCHECKED)
	GUICtrlSetState($CheckBoxHi, $GUI_CHECKED)
	GUICtrlSetData($InputDce, "cue")
	GUICtrlSetData($InputDme, "mkc.xml")
	GUICtrlSetData($InputDte, "mkt.xml")
	GUICtrlSetData($InputDoe, "mmc.txt")
	_GUICtrlComboBox_SetCurSel($ComboOutputFormat, 1)
	GUICtrlSetState($CheckBoxT, $GUI_UNCHECKED)
	GUICtrlSetState($CheckBoxOf, $GUI_UNCHECKED)
	_GUICtrlComboBox_SetCurSel($ComboCueSheetEncoding, 0)
	generate_tags_enable(1)
	_GUICtrlComboBox_SetCurSel($ComboTrack01, 1)
	GUICtrlSetState($CheckBoxCq, $GUI_CHECKED)
	GUICtrlSetState($CheckBoxEu, $GUI_UNCHECKED)
	GUICtrlSetState($CheckBoxTc, $GUI_CHECKED)
	GUICtrlSetState($CheckBoxTagIgnoreCdText, $GUI_UNCHECKED)
	GUICtrlSetState($CheckBoxTagIgnoreCueComments, $GUI_UNCHECKED)
	GUICtrlSetState($CheckBoxTagIgnoreFromMedia, $GUI_UNCHECKED)
	GUICtrlSetState($CheckBoxA, $GUI_UNCHECKED)
	GUICtrlSetState($CheckBoxVerbose, $GUI_UNCHECKED)
	GUICtrlSetState($CheckBoxMerge, $GUI_UNCHECKED)
	GUICtrlSetState($CheckBoxReadMetadata, $GUI_CHECKED)
	_GUICtrlComboBox_SetCurSel($ComboFlacMode, 1)
	GUICtrlSetState($CheckBoxMLang, $GUI_CHECKED)
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
			$sRet = "utf16"

		Case 4
			$sRet = "utf16_bom"
	EndSwitch
	Return SetError(0, 0, $sRet)
EndFunc   ;==>get_encoding_str

Func read_options()
	Local $s = "", $w
	$s &= _Iif(GUICtrlRead($CheckBoxCe) = $GUI_CHECKED, "-ce", "-nce")
	$s &= " "
	$s &= _Iif(GUICtrlRead($CheckBoxEc) = $GUI_CHECKED, "-ec", "-nec")
	$s &= " "
	$s &= _Iif(GUICtrlRead($CheckBoxDf) = $GUI_CHECKED, "-df", "-ndf")
	$s &= " "
	$s &= _Iif(GUICtrlRead($CheckBoxUc) = $GUI_CHECKED, "-uc", "-nuc")
	$s &= " "
	$s &= _Iif(GUICtrlRead($CheckBoxMerge) = $GUI_CHECKED, "--merge", "--dont-merge")
	$s &= " "

	If GUICtrlRead($CheckBoxUc) = $GUI_CHECKED Then
		$s &= "-fo "
		$s &= GUICtrlRead($InputFo)
		$s &= " "
	EndIf

	$w = GUICtrlRead($InputAlternateExt)
	If StringLen($w) > 0 Then
		$s &= "-e """
		$s &= $w
		$s &= """ "
	EndIf

	$w = GUICtrlRead($InputTf)
	If StringLen($w) > 0 Then
		$s &= "-tf """
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

	$s &= _Iif(GUICtrlRead($CheckBoxR) = $GUI_CHECKED, "-r", "-nr")
	$s &= " "

	$s &= _Iif(GUICtrlRead($CheckBoxHi) = $GUI_CHECKED, "-hi", "-nhi")
	$s &= " "

	$w = GUICtrlRead($InputDce)
	If StringLen($w) > 0 Then
		$s &= "-dce "
		$s &= $w
		$s &= " "
	EndIf

	$w = GUICtrlRead($InputDme)
	If StringLen($w) > 0 Then
		$s &= "-dme "
		$s &= $w
		$s &= " "
	EndIf

	$w = GUICtrlRead($InputDte)
	If StringLen($w) > 0 Then
		$s &= "-dte "
		$s &= $w
		$s &= " "
	EndIf

	$w = GUICtrlRead($InputDoe)
	If StringLen($w) > 0 Then
		$s &= "-doe "
		$s &= $w
		$s &= " "
	EndIf

	$w = _GUICtrlComboBox_GetCurSel($ComboOutputFormat)
	Switch $w
		Case 0
			$s &= " -c -oce "
			$s &= get_encoding_str(_GUICtrlComboBox_GetCurSel($ComboCueSheetEncoding))
			$s &= " "

		Case 1
			$s &= " -m "
			$s &= _Iif(GUICtrlRead($CheckBoxT) = $GUI_CHECKED, "-t", "-nt")
			$s &= " "
			$s &= _Iif(GUICtrlRead($CheckBoxOf) = $GUI_CHECKED, "-of", "-nof")
			$s &= " "
			$s &= _Iif(GUICtrlRead($CheckBoxEu) = $GUI_CHECKED, "-eu", "-neu")
			$s &= " "
			$s &= _Iif(GUICtrlRead($CheckBoxTc) = $GUI_CHECKED, "-tc", "-ntc")
			$s &= " "
			$s &= _Iif(GUICtrlRead($CheckBoxTagIgnoreCdText) = $GUI_CHECKED, "--ignore-cdtext-tags", "--use-cdtext-tags")
			$s &= " "
			$s &= _Iif(GUICtrlRead($CheckBoxTagIgnoreCueComments) = $GUI_CHECKED, "--ignore-cue-comments-tags", "--use-cue-comments-tags")
			$s &= " "
			$s &= _Iif(GUICtrlRead($CheckBoxTagIgnoreFromMedia) = $GUI_CHECKED, "--ignore-media-tags", "--use-media-tags")
			$s &= " "
	EndSwitch

	$w = _GUICtrlComboBox_GetCurSel($ComboTrack01)
	Switch $w
		Case 0
			$s &= " -t1i0 "

		Case 1
			$s &= " -t1i1 "
	EndSwitch

	$s &= _Iif(GUICtrlRead($CheckBoxCq) = $GUI_CHECKED, "-cq", "-ncq")
	$s &= " "

	$s &= _Iif(GUICtrlRead($CheckBoxA) = $GUI_CHECKED, "-a", "-na")
	$s &= " "

	$w = _GUICtrlComboBox_GetCurSel($ComboFlacMode)
	Switch $w
		Case 0
			$s &= "--flac-read-none"
			$s &= " "
		Case 1
			$s &= "--flac-read-cuesheet-tag-first"
			$s &= " "
		Case 2
			$s &= "--flac-read-vorbis-comment-first"
			$s &= " "
		Case 3
			$s &= "--flac-read-cuesheet-tag-only"
			$s &= " "
		Case 4
			$s &= "--flac-read-vorbis-comment-only"
			$s &= " "
	EndSwitch

	$s &= _Iif(GUICtrlRead($CheckBoxReadMetadata) = $GUI_CHECKED, "--read-media-tags", "--dont-read-media-tags")
	$s &= " "

	$s &= _Iif(GUICtrlRead($CheckBoxSingleMediaFile) = $GUI_CHECKED, "--single-media-file", "--media-file-with-embedded-cuesheet")
	$s &= " "

	$s &= _Iif(GUICtrlRead($CheckBoxMLang) = $GUI_CHECKED, "--use-mlang", "--dont-use-mlang")
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

log_msg("This is simple frontend to cue2mkc utility.")
log_msg("Use this application convert cue sheet files to Matroska chapters files.")
log_msg("AutoIt version: " & @AutoItVersion & ".")
log_msg("Author: roed@onet.eu.")
log_msg("Icons taken from Primo Icons Set - http://www.webdesignerdepot.com/2009/07/200-free-exclusive-vector-icons-primo/");
log_msg("")

If Not $CUE2MKC_EXE_EXISTS Then
	;GUICtrlSetState($LabelPdftoppm, $GUI_DISABLE)
	log_msg("Warning: cue2mkc tool not found.")
EndIf

$cq = "Try to correct ""simple 'quotation' marks"" inside strings." & @CRLF & _
		"Examples:" & @CRLF & _
		@TAB & "�polish �quotation� marks�" & @CRLF & _
		@TAB & "�english (U.S.) �quotation� marks�" & @CRLF & _
		@TAB & "� french � angle � marks �" & @CRLF & _
		@TAB & "�german �quotation� marks�"
GUICtrlSetTip($CheckBoxCq, $cq, "Correct ""simple 'quotation' marks""", 1)

If @Compiled Then
	GUISetIcon(@ScriptFullPath, -1, $FormMain)
	GUICtrlSetStyle($ButtonInputAdd, $BS_ICON)
	GUICtrlSetImage($ButtonInputAdd, @ScriptFullPath, -6, 0)
	GUICtrlSetStyle($ButtonDataFile, $BS_ICON)
	GUICtrlSetImage($ButtonDataFile, @ScriptFullPath, -9, 0)
	GUICtrlSetStyle($ButtonInputDelete, $BS_ICON)
	GUICtrlSetImage($ButtonInputDelete, @ScriptFullPath, -7, 0)
	GUICtrlSetStyle($ButtonMakeMask, $BS_ICON)
	GUICtrlSetImage($ButtonMakeMask, @ScriptFullPath, -10, 0)
Else
	GUISetIcon(@ScriptDir & "\icons\cd_mka.ico", -1, $FormMain)
	GUICtrlSetStyle($ButtonInputAdd, $BS_ICON)
	GUICtrlSetImage($ButtonInputAdd, @ScriptDir & "\icons\cue_file_add.ico", 0, 0)
	GUICtrlSetStyle($ButtonDataFile, $BS_ICON)
	GUICtrlSetImage($ButtonDataFile, @ScriptDir & "\icons\media_file_add.ico", 0, 0)
	GUICtrlSetStyle($ButtonInputDelete, $BS_ICON)
	GUICtrlSetImage($ButtonInputDelete, @ScriptDir & "\icons\cue_file_remove.ico", 0, 0)
	GUICtrlSetStyle($ButtonMakeMask, $BS_ICON)
	GUICtrlSetImage($ButtonMakeMask, @ScriptDir & "\icons\file_mask.ico", 0, 0)
EndIf
set_default_options()
GUICtrlSetData($InputOuputDir, @MyDocumentsDir)
_GUICtrlComboBox_SetCurSel($ComboOutput, 0)
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

		Case $CheckBoxUc
			frame_offset_enable(_Iif(GUICtrlRead($CheckBoxUc) = $GUI_CHECKED, True, False))

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

		Case $ButtonLicense
			$sCmd = StringFormat("""%s"" --license", $CUE2MKC_EXE)
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

	EndSwitch
WEnd

Exit 0

;
; $Id$
;