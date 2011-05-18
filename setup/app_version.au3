#NoTrayIcon
#region ;**** Directives created by AutoIt3Wrapper_GUI ****
#AutoIt3Wrapper_Compression=4
#AutoIt3Wrapper_Change2CUI=y
#AutoIt3Wrapper_Run_Tidy=y
#endregion ;**** Directives created by AutoIt3Wrapper_GUI ****
;
; app_version.au3
;
AutoItSetOption("MustDeclareVars", 1)

Global Const $CONFIG_FILE = @WorkingDir & "\app_version.ini"
Global Const $ENTRY_BINARY = "Binary"
Global Const $ENTRY_TEMPLATE = "Template"
Global Const $ENTRY_OUTPUT = "Output"
Global Const $ENTRY_CORRECT_VERSION = "CorrectVersion"
Global Const $FV_PARAMS[12] = ["Comments", "InternalName", "ProductName", "CompanyName", "LegalCopyright", "ProductVersion", "FileDescription", "LegalTrademarks", "PrivateBuild", "FileVersion", "OriginalFilename", "SpecialBuild"]

Dim $sSection = "default"

Func repl_keyword($s)
	Return StringFormat("{%s}", $s)
EndFunc   ;==>repl_keyword

Func correct_version($s)
	Return StringReplace($s, ", ", ".")
EndFunc   ;==>correct_version

If $CmdLine[0] > 0 Then
	Switch StringLower($CmdLine[1])
		Case "help"
			ConsoleWrite("Version keywords:" & @CRLF)
			For $s In $FV_PARAMS
				ConsoleWrite( repl_keyword($s) & @CRLF)
			Next
			Exit 0

		Case Else
			$sSection = $CmdLine[1]
	EndSwitch
EndIf

If Not FileExists($CONFIG_FILE) Then
	ConsoleWriteError("Configuration file doesn't exists." & @CRLF)
	Exit 1
EndIf

Dim $sBinary = IniRead($CONFIG_FILE, $sSection, $ENTRY_BINARY, "")
If StringLen($sBinary) = 0 Then
	ConsoleWriteError("Binary file not specified." & @CRLF)
	Exit 1
EndIf

If Not FileExists($sBinary) Then
	ConsoleWriteError("Binary file " & $sBinary & " doesn't exists." & @CRLF)
	Exit 1
EndIf

Dim $sTemplate = IniRead($CONFIG_FILE, $sSection, $ENTRY_TEMPLATE, "app_version.tmpl")
If StringLen($sTemplate) = 0 Then
	ConsoleWriteError("Template file not specified." & @CRLF)
	Exit 1
EndIf

Dim $sOutput = IniRead($CONFIG_FILE, $sSection, $ENTRY_OUTPUT, "app_version.iss")
If StringLen($sOutput) = 0 Then
	ConsoleWriteError("Output file not specified." & @CRLF)
	Exit 1
EndIf

Dim $nCorrectVersion = IniRead($CONFIG_FILE, $sSection, $ENTRY_CORRECT_VERSION, 0)
If StringLen($nCorrectVersion) = 0 Then
	$nCorrectVersion = 0
Else
	$nCorrectVersion = Number($nCorrectVersion)
EndIf


Dim $s = FileRead($sTemplate)
If @error Then
	ConsoleWriteError("Fail to read template file " & $sTemplate & "." & @CRLF)
	Exit 2
EndIf

Dim $ver, $verr, $replaced
For $ver In $FV_PARAMS
	$verr = FileGetVersion($sBinary, $ver)
	If Not @error Then
		If StringInStr($ver, "Version", 1) > 0 And $nCorrectVersion <> 0 Then
			$verr = correct_version($verr)
		EndIf
		$s = StringReplace($s, repl_keyword($ver), $verr, 0, 1)
		$replaced = @extended
		If $replaced > 0 Then
			ConsoleWrite(StringFormat("%d : Replacing {%s} to ""%s"".", $replaced, $ver, $verr) & @CRLF)
		EndIf
	EndIf
Next

Dim $f = FileOpen($sOutput, 2)
If $f = -1 Then
	ConsoleWriteError("Fail to open output file " & $sOutput & "." & @CRLF)
	Exit 3
EndIf

FileWrite($f, $s)
FileClose($f)
Exit 0

;
; End of script
;