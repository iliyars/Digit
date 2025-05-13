# Microsoft Developer Studio Project File - Name="Digit" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Digit - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Digit.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Digit.mak" CFG="Digit - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Digit - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Digit - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Digit - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /G6 /MD /W3 /GX /O2 /I "..\..\MGTools" /I "..\..\MGTools\Include" /I "..\InterfSolver" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x419 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 ..\..\MGTools\Release\MGTools.lib ..\Release\InterfSolver.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "Digit - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /G6 /MDd /W3 /Gm /GX /ZI /Od /I "..\..\MGTools" /I "..\..\MGTools\Include" /I "..\InterfSolver" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x419 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ..\..\MGTools\Debug\MGTools.lib ..\Debug\InterfSolver.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "Digit - Win32 Release"
# Name "Digit - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "MainFrame"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\MainFrame\CommentsFileDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrame\MainFrm.cpp
# End Source File
# End Group
# Begin Group "Controls"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Controls\BoundCtrls.cpp
# End Source File
# Begin Source File

SOURCE=.\Controls\controls.cpp
# End Source File
# Begin Source File

SOURCE=.\Controls\ImageCtrls.cpp
# End Source File
# End Group
# Begin Group "Utils"

# PROP Default_Filter ""
# Begin Group "Edit"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Utils\Edit\BaseTextDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\Utils\Edit\BaseTextFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\Utils\Edit\BaseTextView.cpp
# End Source File
# Begin Source File

SOURCE=.\Utils\Edit\TextChildFrm.cpp
# End Source File
# End Group
# Begin Group "Graph"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Utils\Graph\3DGraphDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\Utils\Graph\GraphChildFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\Utils\Graph\GraphDoc.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\Utils\Cmdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Utils\contour.cpp
# End Source File
# Begin Source File

SOURCE=.\Controls\MeasureCtrls.cpp
# End Source File
# Begin Source File

SOURCE=.\Utils\middle.cpp
# End Source File
# Begin Source File

SOURCE=.\Utils\mutils.cpp
# End Source File
# Begin Source File

SOURCE=.\Utils\SpecFileDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Utils\StateListBox.cpp
# End Source File
# Begin Source File

SOURCE=.\Utils\Tracker.cpp
# End Source File
# Begin Source File

SOURCE=.\Utils\ZOOMVIEW.CPP
# End Source File
# End Group
# Begin Group "ImageTempl"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ImageTempl\BaseImageDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\ImageTempl\BaseImageView.cpp
# End Source File
# Begin Source File

SOURCE=.\ImageTempl\ImageChildFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\ImageTempl\ImageDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\ImageTempl\ImageView.cpp
# End Source File
# End Group
# Begin Group "Options"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Options\ApproxSetDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Options\ToolBarsDlg.cpp
# End Source File
# End Group
# Begin Group "ImageFeatures"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ImageFeatures\SectionFrame.cpp
# End Source File
# Begin Source File

SOURCE=.\ImageFeatures\ZoomDlg.cpp
# End Source File
# End Group
# Begin Group "DigitMode"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DigitMode\DigitInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\DigitMode\DotInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\DigitMode\SectionInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\DigitMode\ZapLineInfo.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\Digit.cpp
# End Source File
# Begin Source File

SOURCE=.\hlp\Digit.hpj

!IF  "$(CFG)" == "Digit - Win32 Release"

# PROP Ignore_Default_Tool 1
USERDEP__DIGIT="hlp\AfxCore.rtf"	"hlp\$(TargetName).hm"	
# Begin Custom Build - Making help file...
OutDir=.\..\Release
TargetName=Digit
InputPath=.\hlp\Digit.hpj
InputName=Digit

"$(OutDir)\$(InputName).hlp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	start /wait /C /E /M "hlp\$(InputName).hpj" 
	if errorlevel 1 goto :Error 
	if not exist "hlp\$(InputName).hlp" goto :Error 
	copy "hlp\$(InputName).hlp" $(OutDir) 
	goto :done 
	:Error 
	echo hlp\$(InputName).hpj(1) : error: 
	type "hlp\$(InputName).log" 
	:done 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Digit - Win32 Debug"

# PROP Ignore_Default_Tool 1
USERDEP__DIGIT="hlp\AfxCore.rtf"	"hlp\$(TargetName).hm"	
# Begin Custom Build - Making help file...
OutDir=.\..\Debug
TargetName=Digit
InputPath=.\hlp\Digit.hpj
InputName=Digit

"$(OutDir)\$(InputName).hlp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	start /wait hcw /C /E /M "hlp\$(InputName).hpj" 
	if errorlevel 1 goto :Error 
	if not exist "hlp\$(InputName).hlp" goto :Error 
	copy "hlp\$(InputName).hlp" $(OutDir) 
	goto :done 
	:Error 
	echo hlp\$(InputName).hpj(1) : error: 
	type "hlp\$(InputName).log" 
	:done 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Digit.rc
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "MainFrame_H"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\MainFrame\CommentsFileDlg.h
# End Source File
# Begin Source File

SOURCE=.\MainFrame\MainFrm.h
# End Source File
# End Group
# Begin Group "Controls_H"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Controls\BoundCtrls.h
# End Source File
# Begin Source File

SOURCE=.\Controls\controls.h
# End Source File
# Begin Source File

SOURCE=.\Controls\ImageCtrls.h
# End Source File
# Begin Source File

SOURCE=.\Controls\MeasureCtrls.h
# End Source File
# End Group
# Begin Group "Utils_H"

# PROP Default_Filter ""
# Begin Group "Edit_H"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Utils\Edit\BaseTextDoc.h
# End Source File
# Begin Source File

SOURCE=.\Utils\Edit\BaseTextFrm.h
# End Source File
# Begin Source File

SOURCE=.\Utils\Edit\BaseTextView.h
# End Source File
# Begin Source File

SOURCE=.\Utils\Edit\TextChildFrm.h
# End Source File
# End Group
# Begin Group "Graph_H"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Utils\Graph\3DGraphDoc.h
# End Source File
# Begin Source File

SOURCE=.\Utils\Graph\GraphChildFrm.h
# End Source File
# Begin Source File

SOURCE=.\Utils\Graph\GraphDoc.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Utils\Cmdlg.h
# End Source File
# Begin Source File

SOURCE=.\Utils\middle.h
# End Source File
# Begin Source File

SOURCE=.\Utils\mutils.h
# End Source File
# Begin Source File

SOURCE=.\Utils\SECPANVW.H
# End Source File
# Begin Source File

SOURCE=.\Utils\SpecFileDlg.h
# End Source File
# Begin Source File

SOURCE=.\Utils\StateListBox.h
# End Source File
# Begin Source File

SOURCE=.\Utils\Tracker.h
# End Source File
# End Group
# Begin Group "ImageTempl_H"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ImageTempl\BaseImageDoc.h
# End Source File
# Begin Source File

SOURCE=.\ImageTempl\BaseImageView.h
# End Source File
# Begin Source File

SOURCE=.\ImageTempl\ImageChildFrm.h
# End Source File
# Begin Source File

SOURCE=.\ImageTempl\ImageDoc.h
# End Source File
# Begin Source File

SOURCE=.\ImageTempl\ImageView.h
# End Source File
# End Group
# Begin Group "Options_H"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Options\ApproxSetDlg.h
# End Source File
# Begin Source File

SOURCE=.\Options\ToolBarsDlg.h
# End Source File
# End Group
# Begin Group "ImageFeatures_H"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ImageFeatures\SectionFrame.h
# End Source File
# Begin Source File

SOURCE=.\ImageFeatures\ZoomDlg.h
# End Source File
# End Group
# Begin Group "DigitMode_H"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DigitMode\DigitInfo.h
# End Source File
# Begin Source File

SOURCE=.\DigitMode\DotInfo.h
# End Source File
# Begin Source File

SOURCE=.\DigitMode\SectionInfo.h
# End Source File
# Begin Source File

SOURCE=.\DigitMode\ZapLineInfo.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Digit.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h

!IF  "$(CFG)" == "Digit - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Making help include file...
TargetName=Digit
InputPath=.\Resource.h

"hlp\$(TargetName).hm" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo. >"hlp\$(TargetName).hm" 
	echo // Commands (ID_* and IDM_*) >>"hlp\$(TargetName).hm" 
	makehm ID_,HID_,0x10000 IDM_,HIDM_,0x10000 resource.h >>"hlp\$(TargetName).hm" 
	echo. >>"hlp\$(TargetName).hm" 
	echo // Prompts (IDP_*) >>"hlp\$(TargetName).hm" 
	makehm IDP_,HIDP_,0x30000 resource.h >>"hlp\$(TargetName).hm" 
	echo. >>"hlp\$(TargetName).hm" 
	echo // Resources (IDR_*) >>"hlp\$(TargetName).hm" 
	makehm IDR_,HIDR_,0x20000 resource.h >>"hlp\$(TargetName).hm" 
	echo. >>"hlp\$(TargetName).hm" 
	echo // Dialogs (IDD_*) >>"hlp\$(TargetName).hm" 
	makehm IDD_,HIDD_,0x20000 resource.h >>"hlp\$(TargetName).hm" 
	echo. >>"hlp\$(TargetName).hm" 
	echo // Frame Controls (IDW_*) >>"hlp\$(TargetName).hm" 
	makehm IDW_,HIDW_,0x50000 resource.h >>"hlp\$(TargetName).hm" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Digit - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Making help include file...
TargetName=Digit
InputPath=.\Resource.h

"hlp\$(TargetName).hm" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo. >"hlp\$(TargetName).hm" 
	echo // Commands (ID_* and IDM_*) >>"hlp\$(TargetName).hm" 
	makehm ID_,HID_,0x10000 IDM_,HIDM_,0x10000 resource.h >>"hlp\$(TargetName).hm" 
	echo. >>"hlp\$(TargetName).hm" 
	echo // Prompts (IDP_*) >>"hlp\$(TargetName).hm" 
	makehm IDP_,HIDP_,0x30000 resource.h >>"hlp\$(TargetName).hm" 
	echo. >>"hlp\$(TargetName).hm" 
	echo // Resources (IDR_*) >>"hlp\$(TargetName).hm" 
	makehm IDR_,HIDR_,0x20000 resource.h >>"hlp\$(TargetName).hm" 
	echo. >>"hlp\$(TargetName).hm" 
	echo // Dialogs (IDD_*) >>"hlp\$(TargetName).hm" 
	makehm IDD_,HIDD_,0x20000 resource.h >>"hlp\$(TargetName).hm" 
	echo. >>"hlp\$(TargetName).hm" 
	echo // Frame Controls (IDW_*) >>"hlp\$(TargetName).hm" 
	makehm IDW_,HIDW_,0x50000 resource.h >>"hlp\$(TargetName).hm" 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\Digit.ico
# End Source File
# Begin Source File

SOURCE=.\res\Digit.rc2
# End Source File
# Begin Source File

SOURCE=.\res\DigitDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# End Group
# Begin Group "Help Files"

# PROP Default_Filter "cnt;rtf"
# Begin Source File

SOURCE=.\hlp\AfxCore.rtf
# End Source File
# Begin Source File

SOURCE=.\hlp\AppExit.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\Bullet.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\CurArw2.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\CurArw4.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\CurHelp.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\Digit.cnt

!IF  "$(CFG)" == "Digit - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Copying contents file...
OutDir=.\..\Release
InputPath=.\hlp\Digit.cnt
InputName=Digit

"$(OutDir)\$(InputName).cnt" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "hlp\$(InputName).cnt" $(OutDir)

# End Custom Build

!ELSEIF  "$(CFG)" == "Digit - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Copying contents file...
OutDir=.\..\Debug
InputPath=.\hlp\Digit.cnt
InputName=Digit

"$(OutDir)\$(InputName).cnt" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "hlp\$(InputName).cnt" $(OutDir)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\hlp\EditCopy.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\EditCut.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\EditPast.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\EditUndo.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\FileNew.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\FileOpen.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\FilePrnt.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\FileSave.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\HlpSBar.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\HlpTBar.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\RecFirst.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\RecLast.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\RecNext.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\RecPrev.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\Scmax.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\ScMenu.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\Scmin.bmp
# End Source File
# End Group
# End Target
# End Project
