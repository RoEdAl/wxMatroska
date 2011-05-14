@ECHO OFF
SET BEAUTIFER=%CD%\uncrustify.exe
SET BEAUTIFER_CFG=%CD%\uncrustify.cfg

FOR /R inc %%i IN (*.h) DO CALL :uncrustify "%%i"

FOR /R wxCueChapters %%i IN (*.cpp) DO CALL :uncrustify "%%i"
FOR /R wxCueFile %%i IN (*.cpp) DO CALL :uncrustify "%%i"
FOR /R wxEncodingDetection %%i IN (*.cpp) DO CALL :uncrustify "%%i"

FOR /R wxCueChapters %%i IN (*.h) DO CALL :uncrustify "%%i"
FOR /R wxCueFile %%i IN (*.h) DO CALL :uncrustify "%%i"
FOR /R wxEncodingDetection %%i IN (*.h) DO CALL :uncrustify "%%i"
GOTO :EOF

:uncrustify
"%BEAUTIFER%" -c "%BEAUTIFER_CFG%" --no-backup %1
GOTO :EOF
