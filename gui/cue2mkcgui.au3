#NoTrayIcon
#Region ;**** Directives created by AutoIt3Wrapper_GUI ****
#AutoIt3Wrapper_Compression=4
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
#include <ListViewConstants.au3>
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
#include <GuiListView.au3>

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

Func get_directory($sPath)
	Local $sDrive, $sDir, $sFName, $sExt
	_PathSplit($sPath, $sDrive, $sDir, $sFName, $sExt)
	Local $sDirectory = _PathMake($sDrive, $sDir, "", "")
	$sDirectory = StringTrimRight($sDirectory, 1)
	Return SetError(0, 0, $sDirectory)
EndFunc   ;==>get_directory

#Region ### START Koda GUI section ### Form=C:\Documents and Settings\VBox\My Documents\Visual Studio 2010\Projects\wxMatroska\gui\cue2mkcgui.kxf
$FormMain = GUICreate("cue2mkc GUI", 488, 419, -1, -1, BitOR($WS_MAXIMIZEBOX, $WS_MINIMIZEBOX, $WS_SIZEBOX, $WS_THICKFRAME, $WS_SYSMENU, $WS_CAPTION, $WS_OVERLAPPEDWINDOW, $WS_TILEDWINDOW, $WS_POPUP, $WS_POPUPWINDOW, $WS_GROUP, $WS_TABSTOP, $WS_BORDER, $WS_CLIPSIBLINGS), BitOR($WS_EX_ACCEPTFILES, $WS_EX_WINDOWEDGE))
GUISetFont(8, 400, 0, "Microsoft Sans Serif")
$MainTab = GUICtrlCreateTab(4, 4, 481, 409, BitOR($TCS_FLATBUTTONS, $TCS_BUTTONS))
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKRIGHT + $GUI_DOCKTOP + $GUI_DOCKHEIGHT)
$PaneInputOutput = GUICtrlCreateTabItem("Input and output files")
$LabelFiles = GUICtrlCreateLabel("&File list: ", 9, 32, 40, 21, $SS_CENTERIMAGE)
GUICtrlSetFont(-1, 8, 800, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$ListViewInputFiles = GUICtrlCreateListView("File path", 9, 56, 469, 273, BitOR($LVS_REPORT, $LVS_SHOWSELALWAYS))
GUICtrlSendMsg(-1, $LVM_SETCOLUMNWIDTH, 0, 450)
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKRIGHT + $GUI_DOCKTOP + $GUI_DOCKBOTTOM)
$ButtonInputDelete = GUICtrlCreateButton("�", 48, 333, 29, 21, 0)
GUICtrlSetFont(-1, 12, 800, 0, "Wingdings")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKBOTTOM + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Delete selected items from list")
$ButtonInputAdd = GUICtrlCreateButton("�", 13, 333, 29, 21, 0)
GUICtrlSetFont(-1, 10, 800, 0, "Wingdings")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKBOTTOM + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Add specified file or file mask to list")
$CheckBoxOutputDir = GUICtrlCreateCheckbox(" &Output:", 9, 358, 65, 21, BitOR($BS_CHECKBOX, $BS_AUTOCHECKBOX, $BS_LEFT, $BS_VCENTER, $BS_PUSHLIKE, $WS_TABSTOP))
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKBOTTOM + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$InputOuputDir = GUICtrlCreateInput("", 76, 358, 345, 21)
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKRIGHT + $GUI_DOCKBOTTOM + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Destination directory")
$ButtonOutputFile = GUICtrlCreateButton("�", 423, 358, 29, 21, 0)
GUICtrlSetFont(-1, 10, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKRIGHT + $GUI_DOCKBOTTOM + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Specify output file")
$ButtonOutputDirectory = GUICtrlCreateButton("1", 453, 358, 29, 21, 0)
GUICtrlSetFont(-1, 10, 800, 0, "Wingdings")
GUICtrlSetResizing(-1, $GUI_DOCKRIGHT + $GUI_DOCKBOTTOM + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "SpecifyOutputDirectory")
$ButtonGo = GUICtrlCreateButton("&Run", 397, 384, 85, 25, 0)
GUICtrlSetFont(-1, 8, 800, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKRIGHT + $GUI_DOCKBOTTOM + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Run cue2mkc tool")
$ButtonMakeMask = GUICtrlCreateButton("l", 84, 333, 29, 21, 0)
GUICtrlSetFont(-1, 10, 800, 0, "Wingdings")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKBOTTOM + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Make file mask with selected items")
$OptionsPane = GUICtrlCreateTabItem("Options")
$CheckBoxCe = GUICtrlCreateCheckbox("Calculate end time of chapters if possible", 11, 32, 217, 17)
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$CheckBoxEc = GUICtrlCreateCheckbox("Embedded cue sheet", 11, 49, 129, 17)
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Try to read embedded cue sheet (requires MediaInfo library)")
$CheckBoxDf = GUICtrlCreateCheckbox("Use data file(s) to calculate end time of chapters", 11, 68, 249, 17)
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$CheckBoxUc = GUICtrlCreateCheckbox("If track's end time is unknown set it to next track position using frame offset", 11, 88, 377, 17)
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$LabelFo = GUICtrlCreateLabel("Offset:", 24, 108, 35, 17, $SS_CENTERIMAGE)
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$InputFo = GUICtrlCreateInput("150", 60, 108, 65, 21, BitOR($ES_AUTOHSCROLL, $ES_NUMBER))
GUICtrlSetLimit(-1, 4)
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Frame offset")
$LabelAlternateExt = GUICtrlCreateLabel("Comma-separated list of alternate extensions of data files:", 11, 132, 273, 21, $SS_CENTERIMAGE)
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$InputAlternateExt = GUICtrlCreateInput("", 288, 132, 113, 21)
GUICtrlSetLimit(-1, 150)
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKRIGHT + $GUI_DOCKTOP + $GUI_DOCKHEIGHT)
$CheckSingleDataFile = GUICtrlCreateCheckbox("Single data file", 11, 157, 105, 21, BitOR($BS_CHECKBOX, $BS_AUTOCHECKBOX, $BS_LEFT, $BS_PUSHLIKE, $WS_TABSTOP))
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Single data file")
$InputSingleDataFile = GUICtrlCreateInput("", 121, 157, 277, 21)
GUICtrlSetLimit(-1, 150)
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKRIGHT + $GUI_DOCKTOP + $GUI_DOCKHEIGHT)
$ButtonSingleDataFile = GUICtrlCreateButton("�", 402, 157, 29, 21, 0)
GUICtrlSetFont(-1, 10, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKRIGHT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Browse for data file")
$LabelTf = GUICtrlCreateLabel("Track title format:", 11, 182, 86, 21)
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$InputTf = GUICtrlCreateInput("%dp% - %dt% - %tt%", 99, 179, 113, 21)
GUICtrlSetLimit(-1, 150)
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$LabelLang = GUICtrlCreateLabel("Chapter's title language:", 11, 200, 117, 21, $SS_CENTERIMAGE)
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$InputLang = GUICtrlCreateInput("eng", 131, 200, 41, 21)
GUICtrlSetLimit(-1, 3)
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$CheckBoxR = GUICtrlCreateCheckbox("Round down track end time to full frames", 11, 225, 217, 17)
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$CheckBoxHi = GUICtrlCreateCheckbox("Convert indexes to hidden chapters", 11, 245, 217, 17)
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$LabelDce = GUICtrlCreateLabel("Cue sheet file extension:", 11, 266, 119, 17, $SS_CENTERIMAGE)
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$InputDce = GUICtrlCreateInput("cue", 132, 266, 53, 21)
GUICtrlSetLimit(-1, 50)
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$LabelDme = GUICtrlCreateLabel("Matroska chapters file extension:", 11, 288, 159, 17, $SS_CENTERIMAGE)
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$InputDme = GUICtrlCreateInput("mkc.xml", 173, 288, 53, 21)
GUICtrlSetLimit(-1, 50)
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$LabelOutputFormat = GUICtrlCreateLabel("Output format:", 11, 312, 71, 17, $SS_CENTERIMAGE)
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$ComboOutputFormat = GUICtrlCreateCombo("", 84, 312, 149, 25, BitOR($CBS_DROPDOWNLIST, $CBS_AUTOHSCROLL))
GUICtrlSetData(-1, "cue sheet|Matroska chapter xml file", "Matroska chapter xml file")
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$LabelTrack01_1 = GUICtrlCreateLabel("For track 01 assume index", 11, 340, 128, 21, $SS_CENTERIMAGE)
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$ComboTrack01 = GUICtrlCreateCombo("", 141, 340, 41, 25, BitOR($CBS_DROPDOWNLIST, $CBS_AUTOHSCROLL))
GUICtrlSetData(-1, "00|01", "01")
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$LabelTrack01_2 = GUICtrlCreateLabel("as beginning of track", 188, 340, 103, 21, $SS_CENTERIMAGE)
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$LabelQuotationMarks = GUICtrlCreateLabel("Quotation marks:", 11, 364, 84, 17, $SS_CENTERIMAGE)
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$ComboQuotationMarks = GUICtrlCreateCombo("", 98, 364, 149, 25, BitOR($CBS_DROPDOWNLIST, $CBS_AUTOHSCROLL))
GUICtrlSetData(-1, "�polish �quotation� marks�|�english �quotation� marks�", "�polish �quotation� marks�")
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Method of converting ""simple 'quotation' marks"" inside strings")
$CheckBoxA = GUICtrlCreateCheckbox("Abort when conversion errors occurs", 11, 387, 201, 17)
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$OutputPane = GUICtrlCreateTabItem("&Output")
$LabelLog = GUICtrlCreateLabel("Messages:", 11, 32, 64, 17, $SS_CENTERIMAGE)
GUICtrlSetFont(-1, 8, 800, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$ListLog = GUICtrlCreateList("", 11, 53, 469, 320, BitOR($LBS_USETABSTOPS, $LBS_NOINTEGRALHEIGHT, $LBS_NOSEL, $WS_HSCROLL, $WS_VSCROLL), $WS_EX_STATICEDGE)
GUICtrlSetFont(-1, 8, 400, 0, "Courier New")
GUICtrlSetBkColor(-1, 0xECE9D8)
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKRIGHT + $GUI_DOCKTOP + $GUI_DOCKBOTTOM)
GUICtrlSetTip(-1, "Application messages")
$ButtonMsgCopy = GUICtrlCreateButton("&Copy", 11, 378, 61, 29, 0)
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKBOTTOM + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Copy messages to clipboard")
$ButtonClearLog = GUICtrlCreateButton("Clear", 75, 378, 61, 29, 0)
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKBOTTOM + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Clear log")
$ButtonInfo = GUICtrlCreateButton("&?", 438, 378, 41, 29, 0)
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKRIGHT + $GUI_DOCKBOTTOM + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Show PDF utility help")
$CheckBoxVerbose = GUICtrlCreateCheckbox("&Verbose mode", 336, 384, 97, 17)
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKRIGHT + $GUI_DOCKBOTTOM + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlCreateTabItem("")
#EndRegion ### END Koda GUI section ###

Func out_dir_enable($bEnable)
	Local $state = _Iif($bEnable, $GUI_ENABLE, $GUI_DISABLE)
	GUICtrlSetState($InputOuputDir, $state)
	GUICtrlSetState($ButtonOutputFile, $state)
	GUICtrlSetState($ButtonOutputDirectory, $state)

	If $bEnable Then
		GUICtrlSetState($InputOuputDir, $GUI_FOCUS)
	EndIf
EndFunc   ;==>out_dir_enable

Func single_file_enable($bEnable)
	Local $state = _Iif($bEnable, $GUI_ENABLE, $GUI_DISABLE)
	GUICtrlSetState($InputSingleDataFile, $state)
	GUICtrlSetState($ButtonSingleDataFile, $state)

	If $bEnable Then
		GUICtrlSetState($InputSingleDataFile, $GUI_FOCUS)
	EndIf
EndFunc   ;==>single_file_enable

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
	GUICtrlSetData($InputAlternateExt, "")
	GUICtrlSetData($InputSingleDataFile, "")
	single_file_enable(False)
	GUICtrlSetData($InputTf, "%dp% - %dt% - %tt%")
	GUICtrlSetData($InputLang, "eng")
	GUICtrlSetState($CheckBoxR, $GUI_UNCHECKED)
	GUICtrlSetState($CheckBoxHi, $GUI_CHECKED)
	GUICtrlSetData($InputDce, "cue")
	GUICtrlSetData($InputDme, "mkc.xml")
	_GUICtrlComboBox_SetCurSel($ComboOutputFormat, 1)
	_GUICtrlComboBox_SetCurSel($ComboTrack01, 1)
	_GUICtrlComboBox_SetCurSel($ComboQuotationMarks, 0)
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

	$w = GUICtrlRead($InputSingleDataFile)
	If StringLen($w) > 0 Then
		$s &= "-f """
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

	$w = _GUICtrlComboBox_GetCurSel($ComboOutputFormat)
	Switch $w
		Case 0
			$s &= " -c "

		Case 1
			$s &= " -m "
	EndSwitch

	$w = _GUICtrlComboBox_GetCurSel($ComboTrack01)
	Switch $w
		Case 0
			$s &= " -t1i0 "

		Case 1
			$s &= " -t1i1 "
	EndSwitch

	$w = _GUICtrlComboBox_GetCurSel($ComboQuotationMarks)
	Switch $w
		Case 0
			$s &= " -pqm "

		Case 1
			$s &= " -eqm "
	EndSwitch

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

	Local $sOutput = GUICtrlRead($InputOuputDir)
	Local $sInput = ""
	Local $i, $w = _GUICtrlListView_GetItemCount($ListViewInputFiles)
	For $i = 0 To $w - 1
		$sInput &= """"
		$sInput &= _GUICtrlListView_GetItemText($ListViewInputFiles, $i)
		$sInput &= """ "
	Next

	$s = """" & $sExe & """ " & $sOptions
	If StringLen($sOutput) > 0 Then
		$s &= " -o """
		$s &= $sOutput
		$s &= """"
	EndIf

	If $w > 0 Then
		$s &= " "
		$s &= $sInput
	EndIf

	Return SetError(0, 0, $s)
EndFunc   ;==>get_cmd_line

Func is_directory($s)
	If Not FileExists($s) Then
		Return False
	EndIf

	Local $a = FileGetAttrib($s)
	Return (StringInStr($s, "D") > 0)
EndFunc   ;==>is_directory

Func output_dir_or_file_dlg($bFile)
	Local $s
	If $bFile Then
		$s = FileSaveDialog("Specify output file", @WorkingDir, $XML_FILTER, 2 + 16, "", $FormMain)
		If Not @error Then
			Return SetError(0, 1, $s)
		Else
			Return SetError(1, 1)
		EndIf
	Else
		$s = GUICtrlRead($InputOuputDir)
		If StringLen($s) = 0 Or Not is_directory($s) Then
			$s = @WorkingDir
		EndIf
		$s = FileSelectFolder("Specify output directory", "", 1 + 2 + 4, $s, $FormMain)
		If Not @error Then
			Return SetError(0, 1, $s)
		Else
			Return SetError(1, 1)
		EndIf
	EndIf
EndFunc   ;==>output_dir_or_file_dlg

Func make_mask($sPath)
	Local $sDrive, $sDir, $sFName, $sExt
	_PathSplit($sPath, $sDrive, $sDir, $sFName, $sExt)
	Local $sMask = _PathMake($sDrive, $sDir, "*", $sExt)
	Return SetError(0, 0, $sMask)
EndFunc   ;==>make_mask

log_msg("This is simple frontend to cue2mkc utility.")
log_msg("Use this application convert cue sheet files to Matroska chapters files.")
log_msg("AutoIt version: " & @AutoItVersion & ".")
log_msg("Author: roed@onet.eu.")
log_msg("")

If Not $CUE2MKC_EXE_EXISTS Then
	;GUICtrlSetState($LabelPdftoppm, $GUI_DISABLE)
	log_msg("Warning: cue2mkc tool not found.")
EndIf

set_default_options()
GUICtrlSetData($InputOuputDir, @MyDocumentsDir)
GUICtrlSetTip($CheckBoxOutputDir, "Specify output directory or file." & @CRLF & "By default files are created in directories where input file resides.", "Output directory or file")
out_dir_enable(False)
_GUICtrlListBox_SetHorizontalExtent($ListLog, 2500)
GUICtrlSetState($ListViewInputFiles, $GUI_DROPACCEPTED)
GUISetState(@SW_SHOW)

While True
	$nMsg = GUIGetMsg()
	Switch $nMsg
		Case $GUI_EVENT_CLOSE
			Exit

		Case $GUI_EVENT_DROPPED
			If @GUI_DropId = $ListViewInputFiles Then
				$s = @GUI_DragFile
				If StringLen($s) > 0 Then
					_GUICtrlListView_AddItem($ListViewInputFiles, $s)
				EndIf
			EndIf

		Case $ButtonInputAdd
			$s = FileOpenDialog("Specify input file", @WorkingDir, $CUE_FILTER, 1 + 2, "", $FormMain)
			If Not @error Then
				_GUICtrlListView_AddItem($ListViewInputFiles, $s)
			EndIf

		Case $ButtonOutputFile
			$s = output_dir_or_file_dlg(True)
			If Not @error Then
				GUICtrlSetData($InputOuputDir, $s)
			EndIf

		Case $ButtonOutputDirectory
			$s = output_dir_or_file_dlg(False)
			If Not @error Then
				GUICtrlSetData($InputOuputDir, $s)
			EndIf

		Case $CheckBoxOutputDir
			out_dir_enable(_Iif(GUICtrlRead($CheckBoxOutputDir) = $GUI_CHECKED, True, False))

		Case $CheckSingleDataFile
			single_file_enable(_Iif(GUICtrlRead($CheckSingleDataFile) = $GUI_CHECKED, True, False))

		Case $ButtonGo
			$s = get_cmd_line()
			If Not @error Then
				GUICtrlSetState($OutputPane, $GUI_SHOW)
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

		Case $ButtonMsgCopy
			copy_log_to_clipboard()

		Case $ButtonClearLog
			clear_log()

		Case $ButtonSingleDataFile
			$s = FileOpenDialog("Specify audio file", @WorkingDir, $MEDIA_FILTER, 1 + 2, "", $FormMain)
			If Not @error Then
				GUICtrlSetData($InputSingleDataFile, $s)
			EndIf

		Case $ButtonInputDelete
			_GUICtrlListView_DeleteItemsSelected(GUICtrlGetHandle($ListViewInputFiles))

		Case $ButtonMakeMask
			$idx = _GUICtrlListView_GetSelectedIndices(GUICtrlGetHandle($ListViewInputFiles), True)
			For $i = 1 To $idx[0]
				_GUICtrlListView_SetItemText( _
						GUICtrlGetHandle($ListViewInputFiles), _
						$idx[$i], _
						make_mask(_GUICtrlListView_GetItemText(GUICtrlGetHandle($ListViewInputFiles), $idx[$i])) _
						)
			Next
	EndSwitch
WEnd

Exit 0

;
; $Id$
;