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
#include <SliderConstants.au3>
#include <TabConstants.au3>
#include <WindowsConstants.au3>
#include <Misc.au3>
#include <GuiListBox.au3>
#include <GuiComboBox.au3>
#include <GuiEdit.au3>
#include <WinAPI.au3>
#include <File.au3>

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

Dim Const $CUE_FILTER = "CUE files (*.cue)|Text files (*.txt)|All files (*)"
Dim Const $XML_FILTER = "XML files (*.xml)|Matroska chapters XML files (*.mkc.xml)|Text files (*.txt)|All files (*)"
Dim Const $STILL_ACTIVE = 259
Dim Const $APP_EXIT_CODE = "Exit code: %d."

Func get_directory($sPath)
	Local $sDrive, $sDir, $sFName, $sExt
	_PathSplit($sPath, $sDrive, $sDir, $sFName, $sExt)
	Local $sDirectory = _PathMake($sDrive, $sDir, "", "")
	$sDirectory = StringTrimRight($sDirectory, 1)
	Return SetError(0, 0, $sDirectory)
EndFunc   ;==>get_directory

Func get_output_file($sPath)
	Local $sDrive, $sDir, $sFName, $sExt
	_PathSplit($sPath, $sDrive, $sDir, $sFName, $sExt)
	Local $sOutFile = _PathMake($sDrive, $sDir, $sFName, "txt")
	Return SetError(0, 0, $sOutFile)
EndFunc   ;==>get_output_file

#Region ### START Koda GUI section ### Form=D:\Temp\wxMatroska\gui\cue2mkcgui.kxf
$FormMain = GUICreate("cue2mkc GUI", 488, 419, -1, -1, BitOR($WS_MAXIMIZEBOX, $WS_MINIMIZEBOX, $WS_SIZEBOX, $WS_THICKFRAME, $WS_SYSMENU, $WS_CAPTION, $WS_OVERLAPPEDWINDOW, $WS_TILEDWINDOW, $WS_POPUP, $WS_POPUPWINDOW, $WS_GROUP, $WS_TABSTOP, $WS_BORDER, $WS_CLIPSIBLINGS), BitOR($WS_EX_ACCEPTFILES, $WS_EX_WINDOWEDGE))
GUISetFont(8, 400, 0, "Microsoft Sans Serif")
$MainTab = GUICtrlCreateTab(4, 4, 481, 409, BitOR($TCS_FLATBUTTONS, $TCS_BUTTONS))
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKRIGHT + $GUI_DOCKTOP + $GUI_DOCKHEIGHT)
$PaneInputOutput = GUICtrlCreateTabItem("Input and output files")
$LabelFiles = GUICtrlCreateLabel("&File list: ", 9, 32, 40, 21, $SS_CENTERIMAGE)
GUICtrlSetFont(-1, 8, 800, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$ListViewInputFiles = GUICtrlCreateListView("File path", 9, 56, 469, 273)
GUICtrlSendMsg(-1, $LVM_SETCOLUMNWIDTH, 0, 450)
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKRIGHT + $GUI_DOCKTOP + $GUI_DOCKBOTTOM)
$LabelSource = GUICtrlCreateLabel("&File: ", 9, 333, 32, 21, $SS_CENTERIMAGE)
GUICtrlSetFont(-1, 8, 800, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKBOTTOM + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$InputSource = GUICtrlCreateInput("", 76, 333, 345, 21)
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKRIGHT + $GUI_DOCKBOTTOM + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Specify ASF, MSV or MSA file tor repair")
$ButtonSource = GUICtrlCreateButton("…", 423, 333, 29, 21, 0)
GUICtrlSetFont(-1, 10, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKRIGHT + $GUI_DOCKBOTTOM + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Browse for input file")
$ButtonInputAdd = GUICtrlCreateButton("1", 453, 333, 29, 21, 0)
GUICtrlSetFont(-1, 10, 800, 0, "Wingdings")
GUICtrlSetResizing(-1, $GUI_DOCKRIGHT + $GUI_DOCKBOTTOM + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Add specified file or file mask to list")
$CheckBoxOutputDir = GUICtrlCreateCheckbox(" &Output:", 9, 358, 65, 21, BitOR($BS_CHECKBOX, $BS_AUTOCHECKBOX, $BS_LEFT, $BS_VCENTER, $BS_PUSHLIKE, $WS_TABSTOP))
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKBOTTOM + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$InputOuputDir = GUICtrlCreateInput("", 76, 358, 345, 21)
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKRIGHT + $GUI_DOCKBOTTOM + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Destination directory")
$ButtonOutputDir = GUICtrlCreateButton("…", 423, 358, 29, 21, 0)
GUICtrlSetFont(-1, 10, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKRIGHT + $GUI_DOCKBOTTOM + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Browse for output directory")
$ButtonOutputOpen = GUICtrlCreateButton("1", 453, 358, 29, 21, 0)
GUICtrlSetFont(-1, 10, 800, 0, "Wingdings")
GUICtrlSetResizing(-1, $GUI_DOCKRIGHT + $GUI_DOCKBOTTOM + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Open destination directory or file")
$ButtonGo = GUICtrlCreateButton("&Run", 397, 384, 85, 25, 0)
GUICtrlSetFont(-1, 8, 800, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKRIGHT + $GUI_DOCKBOTTOM + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Run cue2mkc tool")
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
$LabelSingleDataFile = GUICtrlCreateLabel("Single data file:", 11, 157, 76, 21, $SS_CENTERIMAGE)
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$InputSingleDataFile = GUICtrlCreateInput("", 89, 157, 309, 21)
GUICtrlSetLimit(-1, 150)
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKRIGHT + $GUI_DOCKTOP + $GUI_DOCKHEIGHT)
$ButtonsingleDataFile = GUICtrlCreateButton("…", 402, 157, 29, 21, 0)
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
GUICtrlSetData(-1, "„polish ‚quotation’ marks”|“english ‘quotation’ marks”", "„polish ‚quotation’ marks”")
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
GUICtrlSetTip(-1, "Method of converting ""simple 'quotation' marks"" inside strings")
$CheckBoxA = GUICtrlCreateCheckbox("Abort when conversion errors occurs", 11, 387, 201, 17)
GUICtrlSetFont(-1, 8, 400, 0, "Microsoft Sans Serif")
GUICtrlSetResizing(-1, $GUI_DOCKLEFT + $GUI_DOCKTOP + $GUI_DOCKWIDTH + $GUI_DOCKHEIGHT)
$OutputPane = GUICtrlCreateTabItem("&Output")
$LabelLog = GUICtrlCreateLabel("Log:", 11, 32, 29, 17, $SS_CENTERIMAGE)
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
GUICtrlCreateTabItem("")
#EndRegion ### END Koda GUI section ###

Func out_dir_enable($bEnable)
	Local $state = _Iif($bEnable, $GUI_ENABLE, $GUI_DISABLE)
	GUICtrlSetState($InputOuputDir, $state)
	GUICtrlSetState($ButtonOutputDir, $state)

	If $bEnable Then
		GUICtrlSetState($InputOuputDir, $GUI_FOCUS)
	EndIf
EndFunc   ;==>out_dir_enable

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

Func get_cmd_line()
	Local $sExe = $CUE2MKC_EXE
	Local $sParams = "", $s

	#cs
		$s = GUICtrlRead($InputFirstPage)
		If StringLen($s) > 0 Then
		If StringIsInt($s) Then
		$sParams &= " -f " & $s
		Else
		log_msg("Invalid first page number.")
		GUICtrlSetState($InputFirstPage, $GUI_FOCUS)
		Return SetError(1, 0, "")
		EndIf
		EndIf
		
		$s = GUICtrlRead($InputLastPage)
		If StringLen($s) > 0 Then
		If StringIsInt($s) Then
		$sParams &= " -l " & $s
		Else
		log_msg("Invalid last page number.")
		GUICtrlSetState($InputLastPage, $GUI_FOCUS)
		Return SetError(1, 0, "")
		EndIf
		EndIf
		
		$s = GUICtrlRead($InputPwd)
		If StringLen($s) > 0 Then
		Local $sPwdType = GUICtrlRead($ComboPwType)
		Switch $sPwdType
		Case "user"
		$sParams &= " -upw """ & $s & ""
		
		Case "owner"
		$sParams &= " -opw """ & $s & ""
		
		Case Else
		log_msg("Invalid password type.")
		GUICtrlSetState($ComboPwType, $GUI_FOCUS)
		Return SetError(1, 0, "")
		EndSwitch
		EndIf
		
		If GUICtrlRead($CheckBoxQuiet) = $GUI_CHECKED Then
		$sParams &= " -q"
		EndIf
		
		Switch $nSheet
		Case $PdfRender
		$sParams &= " -aa " & _Iif(GUICtrlRead($CheckBoxAntiAliasing) = $GUI_CHECKED, "yes", "no")
		$sParams &= " -aaVector " & _Iif(GUICtrlRead($CheckBoxVectorAntiAliasing) = $GUI_CHECKED, "yes", "no")
		Switch (GUICtrlRead($ComboImageType))
		Case "mono"
		$sParams &= " -mono"
		
		Case "gray"
		$sParams &= " -gray"
		EndSwitch
		
		$s = GUICtrlRead($ComboDPI)
		If StringLen($s) > 0 Then
		If StringIsInt($s) Then
		$sParams &= " -r " & $s
		Else
		log_msg("Invalid resolution.")
		GUICtrlSetState($ComboDPI, $GUI_FOCUS)
		Return SetError(1, 0, "")
		EndIf
		EndIf
		
		
		Case $PdfExtractImages
		If GUICtrlRead($CheckBoxDCT) = $GUI_CHECKED Then
		$sParams &= " -j"
		EndIf
		
		Case $PdfExtractText
		$sParams &= " -eol dos"
		
		If GUICtrlRead($CheckBoxLayout) = $GUI_CHECKED Then
		$sParams &= " -layout"
		EndIf
		
		If GUICtrlRead($CheckBoxRaw) = $GUI_CHECKED Then
		$sParams &= " -raw"
		EndIf
		
		If GUICtrlRead($CheckBoxHTML) = $GUI_CHECKED Then
		$sParams &= " -htmlmeta"
		EndIf
		
		If GUICtrlRead($CheckBoxNoPgBrk) = $GUI_CHECKED Then
		$sParams &= " -nopgbrk"
		EndIf
		
		$s = GUICtrlRead($ComboTextEncoding)
		If StringLen($s) > 0 Then
		$sParams &= " -enc " & $s
		EndIf
		
		$s = GUICtrlRead($ComboCharacterWidth)
		If StringLen($s) > 0 Then
		If StringIsInt($s) Then
		$sParams &= " -fixed " & $s
		Else
		log_msg("Invalid character width.")
		GUICtrlSetState($ComboCharacterWidth, $GUI_FOCUS)
		Return SetError(1, 0, "")
		EndIf
		EndIf
		EndSwitch
		
		$s = GUICtrlRead($InputSource)
		If StringLen($s) > 0 Then
		$sParams &= " """ & $s & """"
		Else
		log_msg("Input file not specified.")
		GUICtrlSetState($InputSource, $GUI_FOCUS)
		Return SetError(1, 0, "")
		EndIf
		
		Switch $nSheet
		Case $PdfRender, $PdfExtractImages
		$s = GUICtrlRead($InputRoot)
		If StringLen($s) > 0 Then
		Local $as = StringSplit($s, "/\", 0)
		If $as[0] = 1 Then
		$sParams &= " """ & $s & """"
		Else
		log_msg("Invalid root.")
		GUICtrlSetState($InputRoot, $GUI_FOCUS)
		Return SetError(1, 0, "")
		EndIf
		Else
		log_msg("Root not specified.")
		GUICtrlSetState($InputRoot, $GUI_FOCUS)
		Return SetError(1, 0, "")
		EndIf
		
		Case $PdfExtractText
		If GUICtrlRead($CheckBoxOutputDir) = $GUI_CHECKED Then
		$s = GUICtrlRead($InputOuputDir)
		If StringLen($s) > 0 Then
		$sParams &= " """ & $s & """"
		Else
		log_msg("Warning: Output file not specified.")
		log_msg("Outputing to log window.")
		$sParams &= " -"
		EndIf
		Else
		$sParams &= " -"
		EndIf
		EndSwitch
	#ce
	Return SetError(0, 0, """" & $sExe & """ " & $sParams)
EndFunc   ;==>get_cmd_line

Func get_out_dir()
	Local $sDir = GUICtrlRead($InputOuputDir)
	If StringLen($sDir) = 0 Then
		$sDir = get_directory(GUICtrlRead($InputSource))
		If StringLen($sDir) = 0 Then
			$sDir = @WorkingDir
		EndIf
		GUICtrlSetData($InputOuputDir, $sDir)
	EndIf
	Return SetError(0, 0, $sDir)
EndFunc   ;==>get_out_dir

Func output_dir_or_file_dlg()
	Local $s
	$s = FileSaveDialog("Specify output file", @WorkingDir, $XML_FILTER, 2 + 16, "", $FormMain)
	If Not @error Then
		Return SetError(0, 1, $s)
	Else
		Return SetError(1, 1)
	EndIf
EndFunc   ;==>output_dir_or_file_dlg

log_msg("This is simple frontend to cue2mkc utility.")
log_msg("Use this application convert cue sheet to Matroska chapters file.")
log_msg("AutoIt version: " & @AutoItVersion & ".")
log_msg("Author: roed@onet.eu.")
log_msg("")

If Not $CUE2MKC_EXE_EXISTS Then
	;GUICtrlSetState($LabelPdftoppm, $GUI_DISABLE)
	log_msg("Warning: cue2mkc tool not found.")
EndIf

out_dir_enable(False)
_GUICtrlListBox_SetHorizontalExtent($ListLog, 1600)
GUICtrlSetState($InputSource, $GUI_DROPACCEPTED)
GUISetState(@SW_SHOW)

While True
	$nMsg = GUIGetMsg()
	Switch $nMsg
		Case $GUI_EVENT_CLOSE
			Exit

		Case $GUI_EVENT_DROPPED
			If @GUI_DropId = $InputSource Then
				$s = @GUI_DragFile
				If StringLen($s) > 0 Then
					;GUICtrlSendToDummy($DummySource)
				EndIf
			EndIf

		Case $ButtonSource
			$s = FileOpenDialog("Specify input file", @WorkingDir, $CUE_FILTER, 1 + 2, "", $FormMain)
			If Not @error Then
				;GUICtrlSetData($InputSource, $s)
				;GUICtrlSendToDummy($DummySource)
			EndIf

		Case $ButtonOutputDir
			$s = output_dir_or_file_dlg()
			If Not @error Then
				GUICtrlSetData($InputOuputDir, $s)
			EndIf

		Case $CheckBoxOutputDir
			out_dir_enable(_Iif(GUICtrlRead($CheckBoxOutputDir) = $GUI_CHECKED, True, False))

		Case $ButtonGo
			$s = get_cmd_line()
			If Not @error Then
				$nRet = run_wait($s, 2, get_out_dir())
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

		Case $ButtonOutputOpen
			$s = GUICtrlRead($InputOuputDir)
			If StringLen($s) > 0 Then
				ShellExecute($s)
			EndIf

		Case $ButtonMsgCopy
			copy_log_to_clipboard()

		Case $ButtonClearLog
			clear_log()

	EndSwitch
WEnd

Exit 0

;
; $Id$
;