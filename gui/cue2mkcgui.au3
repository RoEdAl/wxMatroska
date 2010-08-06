#NoTrayIcon
#Region ;**** Directives created by AutoIt3Wrapper_GUI ****
#AutoIt3Wrapper_Icon=icons/cd_mka.ico
#AutoIt3Wrapper_Compression=4
#AutoIt3Wrapper_Res_Comment=This is frontend to cue2mkc tool
#AutoIt3Wrapper_Res_Description=Graphical user interface for cue2mkc command line tool
#AutoIt3Wrapper_Res_Fileversion=0.1.0.11
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

#Region ### START Koda GUI section ### Form=D:\Temp\wxMatroska\gui\cue2mkcgui.kxf
$FormMain = GUICreate("cue2mkc GUI", 543, 403, -1, -1, BitOR($WS_MAXIMIZEBOX, $WS_MINIMIZEBOX, $WS_SIZEBOX, $WS_THICKFRAME, $WS_SYSMENU, $WS_CAPTION, $WS_OVERLAPPEDWINDOW, $WS_TILEDWINDOW, $WS_POPUP, $WS_POPUPWINDOW, $WS_GROUP, $WS_TABSTOP, $WS_BORDER, $WS_CLIPSIBLINGS), BitOR($WS_EX_ACCEPTFILES, $WS_EX_WINDOWEDGE))
GUISetFont(8, 400, 0, "Microsoft Sans Serif")
$DummyOutput = GUICtrlCreateDummy()
$MainTab = GUICtrlCreateTab(4, 2, 533, 397, BitOR($TCS_FLATBUTTONS, $TCS_BUTTONS))
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKRIGHT + $GUI_DOCKTOP + $GUI_DOCKHEIGHT)
$PaneInputOutput = GUICtrlCreateTabItem("Input and output files")
$LabelFiles = GUICtrlCreateLabel("&File list: ", 9, 29, 40, 21, $SS_CENTERIMAGE)
GUICtrlSetFont(-1, 8, 800, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$TreeViewInputFiles = GUICtrlCreateTreeView(8, 58, 525, 230, BitOR($TVS_HASBUTTONS, $TVS_HASLINES, $TVS_DISABLEDRAGDROP, $TVS_SHOWSELALWAYS, $TVS_FULLROWSELECT, $WS_GROUP, $WS_TABSTOP), $WS_EX_STATICEDGE)
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKRIGHT + $GUI_DOCKTOP + $GUI_DOCKBOTTOM)
$ButtonInputAdd = GUICtrlCreateButton("+", 9, 291, 29, 21, 0)
GUICtrlSetFont(-1, 14, 800, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKBOTTOM + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Add specified file or file mask to list")
$ButtonInputDelete = GUICtrlCreateButton("-", 44, 291, 29, 21, 0)
GUICtrlSetFont(-1, 14, 800, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKBOTTOM + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Delete selected items from list")
$ButtonMakeMask = GUICtrlCreateButton("•", 80, 291, 29, 21, 0)
GUICtrlSetFont(-1, 14, 800, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKBOTTOM + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Make file mask with selected items")
$ButtonDataFile = GUICtrlCreateButton("+", 114, 291, 29, 21, 0)
GUICtrlSetFont(-1, 12, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKBOTTOM + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Add data file to selected cue sheet")
$LabelOutput = GUICtrlCreateLabel("&Output:", 11, 312, 48, 21, $SS_CENTERIMAGE)
GUICtrlSetFont(-1, 8, 800, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$ComboOutput = GUICtrlCreateCombo("", 8, 336, 137, 25, BitOR($CBS_DROPDOWNLIST, $CBS_AUTOHSCROLL))
GUICtrlSetData(-1, "Input directory|Directory|File")
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKBOTTOM + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$InputOuputDir = GUICtrlCreateInput("", 148, 336, 353, 21)
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKRIGHT + $GUI_DOCKBOTTOM + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Destination directory")
$ButtonOutputDirectory = GUICtrlCreateButton("1", 505, 336, 29, 21, 0)
GUICtrlSetFont(-1, 10, 400, 0, "Wingdings")
GUICtrlSetResizing(-1, $GUI_DOCKRIGHT + $GUI_DOCKBOTTOM + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Specify output directory or file")
$CheckBoxSwitchToOutput = GUICtrlCreateCheckbox("&Switch to output pane", 308, 372, 137, 17)
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKRIGHT + $GUI_DOCKBOTTOM + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Switch to output pane before running")
$ButtonGo = GUICtrlCreateButton("&Run", 449, 366, 85, 29, 0)
GUICtrlSetFont(-1, 8, 800, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKRIGHT + $GUI_DOCKBOTTOM + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Run cue2mkc tool")
$OptionsPane = GUICtrlCreateTabItem("Options")
$GroupGeneral = GUICtrlCreateGroup("&General options", 8, 32, 293, 161, -1, $WS_EX_TRANSPARENT)
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$CheckBoxEc = GUICtrlCreateCheckbox("Embedded cue sheet", 12, 48, 129, 17)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Try to read embedded cue sheet (requires MediaInfo library)")
$CheckBoxDf = GUICtrlCreateCheckbox("Use data file(s)", 12, 66, 97, 17)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Use data file(s) to calculate end time of chapters")
$LabelAlternateExt = GUICtrlCreateLabel("Alternate data file extensions:", 12, 83, 145, 21, $SS_CENTERIMAGE)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$InputAlternateExt = GUICtrlCreateInput("", 158, 83, 133, 21)
GUICtrlSetLimit(-1, 150)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Comma-separated list of alternate extensions of data files")
$LabelOutputFormat = GUICtrlCreateLabel("Output format:", 12, 105, 71, 21, $SS_CENTERIMAGE)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$ComboOutputFormat = GUICtrlCreateCombo("", 85, 105, 149, 25, BitOR($CBS_DROPDOWNLIST, $CBS_AUTOHSCROLL))
GUICtrlSetData(-1, "cue sheet|Matroska chapter xml file", "Matroska chapter xml file")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$CheckBoxT = GUICtrlCreateCheckbox("Generate tags file also", 28, 127, 137, 17)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$CheckBoxCq = GUICtrlCreateCheckbox("Correct ""simple 'quotation' marks"" inside strings", 12, 147, 261, 17)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$CheckBoxA = GUICtrlCreateCheckbox("Abort when conversion errors occurs", 12, 167, 201, 17)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlCreateGroup("", -99, -99, 1, 1)
$GroupChapterOptions = GUICtrlCreateGroup("&Chapter file generation options", 8, 200, 293, 193, -1, $WS_EX_TRANSPARENT)
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$CheckBoxCe = GUICtrlCreateCheckbox("Calculate end time of chapters if possible", 12, 218, 217, 17)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$CheckBoxUc = GUICtrlCreateCheckbox("Calculate chapter's end time from next chapter", 12, 234, 276, 17)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "If track's end time is unknown set it to next track position using frame offset")
$LabelFo = GUICtrlCreateLabel("Offset:", 35, 255, 35, 17, $SS_CENTERIMAGE)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$InputFo = GUICtrlCreateInput("150", 71, 255, 65, 21, BitOR($ES_AUTOHSCROLL, $ES_NUMBER))
GUICtrlSetLimit(-1, 4)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Frame offset")
$LabelTrack01_1 = GUICtrlCreateLabel("For track 01 assume index", 12, 278, 128, 21, $SS_CENTERIMAGE)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$ComboTrack01 = GUICtrlCreateCombo("", 144, 278, 41, 25, BitOR($CBS_DROPDOWNLIST, $CBS_AUTOHSCROLL))
GUICtrlSetData(-1, "00|01", "01")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$LabelTrack01_2 = GUICtrlCreateLabel("as beginning of track", 189, 278, 103, 21, $SS_CENTERIMAGE)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$CheckBoxR = GUICtrlCreateCheckbox("Round down track end time to full frames", 12, 302, 217, 17)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$CheckBoxHi = GUICtrlCreateCheckbox("Convert indexes to hidden chapters", 12, 320, 217, 17)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$LabelTf = GUICtrlCreateLabel("Track title format:", 12, 340, 86, 21, $SS_CENTERIMAGE)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$InputTf = GUICtrlCreateInput("%dp% - %dt% - %tt%", 100, 341, 113, 21)
GUICtrlSetLimit(-1, 150)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$LabelLang = GUICtrlCreateLabel("Chapter's title language:", 12, 365, 117, 21, $SS_CENTERIMAGE)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$InputLang = GUICtrlCreateInput("eng", 134, 365, 41, 21)
GUICtrlSetLimit(-1, 3)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "This option also affects tags file generation")
GUICtrlCreateGroup("", -99, -99, 1, 1)
$GroupFileExtensions = GUICtrlCreateGroup("&Files extensions", 308, 32, 225, 93, -1, $WS_EX_TRANSPARENT)
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$LabelDce = GUICtrlCreateLabel("Cue sheet file extension:", 312, 51, 119, 21, $SS_CENTERIMAGE)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$InputDce = GUICtrlCreateInput("cue", 473, 51, 53, 21)
GUICtrlSetLimit(-1, 50)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$LabelDme = GUICtrlCreateLabel("Matroska chapters file extension:", 312, 74, 159, 21, $SS_CENTERIMAGE)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$InputDme = GUICtrlCreateInput("mkc.xml", 473, 74, 53, 21)
GUICtrlSetLimit(-1, 50)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$LabelDte = GUICtrlCreateLabel("Matroska tags file extension:", 312, 97, 159, 21, $SS_CENTERIMAGE)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$InputDte = GUICtrlCreateInput("mkt.xml", 473, 97, 53, 21)
GUICtrlSetLimit(-1, 50)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlCreateGroup("", -99, -99, 1, 1)
$GroupTagFile = GUICtrlCreateGroup("&Tag file generation options", 308, 200, 225, 65, -1, $WS_EX_TRANSPARENT)
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$CheckBoxEu = GUICtrlCreateCheckbox("Generate edition UID", 312, 220, 129, 17)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$CheckBoxTc = GUICtrlCreateCheckbox("Parse tags from cue sheet comments", 312, 238, 197, 17)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Try to parse tags from cue sheet comments")
GUICtrlCreateGroup("", -99, -99, 1, 1)
$GroupCueSheet = GUICtrlCreateGroup("Cue &sheet generation options", 308, 268, 225, 49, -1, $WS_EX_TRANSPARENT)
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$CheckBoxC8 = GUICtrlCreateCheckbox("Set cue sheet file encoding to UTF-8", 312, 289, 201, 17)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlCreateGroup("", -99, -99, 1, 1)
$OutputPane = GUICtrlCreateTabItem("&Output")
$LabelLog = GUICtrlCreateLabel("&Messages:", 11, 29, 71, 17, $SS_CENTERIMAGE)
GUICtrlSetFont(-1, 8, 800, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$ListLog = GUICtrlCreateList("", 11, 51, 517, 308, BitOR($LBS_USETABSTOPS, $LBS_NOINTEGRALHEIGHT, $LBS_NOSEL, $WS_HSCROLL, $WS_VSCROLL), $WS_EX_STATICEDGE)
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetBkColor(-1, 0xECE9D8)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKRIGHT + $GUI_DOCKTOP + $GUI_DOCKBOTTOM)
GUICtrlSetTip(-1, "Application messages")
$ButtonMsgCopy = GUICtrlCreateButton("&Copy", 11, 364, 61, 29, 0)
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKBOTTOM + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Copy messages to clipboard")
$ButtonClearLog = GUICtrlCreateButton("Clear", 75, 364, 61, 29, 0)
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKBOTTOM + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Clear log")
$ButtonInfo = GUICtrlCreateButton("&Help", 490, 364, 41, 29, 0)
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKRIGHT + $GUI_DOCKBOTTOM + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Runs cue2mkc with --help option")
$CheckBoxVerbose = GUICtrlCreateCheckbox("&Verbose mode", 316, 370, 97, 17)
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKRIGHT + $GUI_DOCKBOTTOM + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$ButtonLicense = GUICtrlCreateButton("&License", 418, 364, 65, 29, 0)
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKRIGHT + $GUI_DOCKBOTTOM + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Runs cue2mkc with --licese option")
GUICtrlCreateTabItem("")
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

		Case 1
			GUICtrlSetState($CheckBoxT, $GUI_ENABLE)
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
	GUICtrlSetData($InputLang, "eng")
	GUICtrlSetState($CheckBoxR, $GUI_UNCHECKED)
	GUICtrlSetState($CheckBoxHi, $GUI_CHECKED)
	GUICtrlSetData($InputDce, "cue")
	GUICtrlSetData($InputDme, "mkc.xml")
	GUICtrlSetData($InputDte, "mkt.xml")
	_GUICtrlComboBox_SetCurSel($ComboOutputFormat, 1)
	GUICtrlSetState($CheckBoxT, $GUI_UNCHECKED)
	GUICtrlSetState($CheckBoxC8, $GUI_UNCHECKED)
	generate_tags_enable(1)
	_GUICtrlComboBox_SetCurSel($ComboTrack01, 1)
	GUICtrlSetState($CheckBoxCq, $GUI_CHECKED)
	GUICtrlSetState($CheckBoxEu, $GUI_UNCHECKED)
	GUICtrlSetState($CheckBoxTc, $GUI_CHECKED)
	GUICtrlSetState($CheckBoxA, $GUI_UNCHECKED)
	GUICtrlSetState($CheckBoxVerbose, $GUI_UNCHECKED)
EndFunc   ;==>set_default_options

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

	$w = _GUICtrlComboBox_GetCurSel($ComboOutputFormat)
	Switch $w
		Case 0
			$s &= " -c "
			$s &= _Iif(GUICtrlRead($CheckBoxC8) = $GUI_CHECKED, "-c8", "-nc8")
			$s &= " "
		Case 1
			$s &= " -m "
			$s &= _Iif(GUICtrlRead($CheckBoxT) = $GUI_CHECKED, "-t", "-nt")
			$s &= " "
			$s &= _Iif(GUICtrlRead($CheckBoxEu) = $GUI_CHECKED, "-eu", "-neu")
			$s &= " "
			$s &= _Iif(GUICtrlRead($CheckBoxTc) = $GUI_CHECKED, "-tc", "-ntc")
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
		@TAB & "„polish ‚quotation’ marks”" & @CRLF & _
		@TAB & "“english (U.S.) ‘quotation’ marks”" & @CRLF & _
		@TAB & "« french ‹ angle › marks »" & @CRLF & _
		@TAB & "„german ‚quotation‘ marks“"
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