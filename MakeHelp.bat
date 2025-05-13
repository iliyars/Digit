@echo off
REM -- First make map file from Microsoft Visual C++ generated resource.h
echo // MAKEHELP.BAT generated Help Map file.  Used by DIGIT.HPJ. >"hlp\Digit.hm"
echo. >>"hlp\Digit.hm"
echo // Commands (ID_* and IDM_*) >>"hlp\Digit.hm"
makehm ID_,HID_,0x10000 IDM_,HIDM_,0x10000 resource.h >>"hlp\Digit.hm"
echo. >>"hlp\Digit.hm"
echo // Prompts (IDP_*) >>"hlp\Digit.hm"
makehm IDP_,HIDP_,0x30000 resource.h >>"hlp\Digit.hm"
echo. >>"hlp\Digit.hm"
echo // Resources (IDR_*) >>"hlp\Digit.hm"
makehm IDR_,HIDR_,0x20000 resource.h >>"hlp\Digit.hm"
echo. >>"hlp\Digit.hm"
echo // Dialogs (IDD_*) >>"hlp\Digit.hm"
makehm IDD_,HIDD_,0x20000 resource.h >>"hlp\Digit.hm"
echo. >>"hlp\Digit.hm"
echo // Frame Controls (IDW_*) >>"hlp\Digit.hm"
makehm IDW_,HIDW_,0x50000 resource.h >>"hlp\Digit.hm"
REM -- Make help for Project DIGIT


echo Building Win32 Help files
start /wait hcw /C /E /M "hlp\Digit.hpj"
if errorlevel 1 goto :Error
if not exist "hlp\Digit.hlp" goto :Error
if not exist "hlp\Digit.cnt" goto :Error
echo.
if exist Debug\nul copy "hlp\Digit.hlp" Debug
if exist Debug\nul copy "hlp\Digit.cnt" Debug
if exist Release\nul copy "hlp\Digit.hlp" Release
if exist Release\nul copy "hlp\Digit.cnt" Release
echo.
goto :done

:Error
echo hlp\Digit.hpj(1) : error: Problem encountered creating help file

:done
echo.
