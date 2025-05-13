# Microsoft Developer Studio Project File - Name="InterfSolver" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=InterfSolver - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "InterfSolver.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "InterfSolver.mak" CFG="InterfSolver - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "InterfSolver - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "InterfSolver - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=xicl6.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "InterfSolver - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /G6 /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x419 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386

!ELSEIF  "$(CFG)" == "InterfSolver - Win32 Debug"

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
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /G6 /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x419 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# SUBTRACT LINK32 /map

!ENDIF 

# Begin Target

# Name "InterfSolver - Win32 Release"
# Name "InterfSolver - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "Tools"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Tools\CalcContour.cpp
# End Source File
# Begin Source File

SOURCE=.\Tools\CalcLimits.cpp
# End Source File
# Begin Source File

SOURCE=.\Tools\CTIOFile.cpp
# End Source File
# Begin Source File

SOURCE=.\Tools\GetTimeDate.cpp
# End Source File
# Begin Source File

SOURCE=.\Tools\isPupil.cpp
# End Source File
# Begin Source File

SOURCE=.\Tools\Matin1.cpp
# End Source File
# Begin Source File

SOURCE=.\Tools\Matrix.cpp
# End Source File
# Begin Source File

SOURCE=.\Tools\NunberInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\Tools\PolApproximation.cpp
# End Source File
# Begin Source File

SOURCE=.\Tools\ReadWriteData.cpp
# End Source File
# Begin Source File

SOURCE=.\Tools\SamplDat.cpp
# End Source File
# Begin Source File

SOURCE=.\Tools\Sort.cpp
# End Source File
# Begin Source File

SOURCE=.\Tools\Vector.cpp
# End Source File
# Begin Source File

SOURCE=.\Tools\XYBrokenLine.cpp
# End Source File
# Begin Source File

SOURCE=.\Tools\XYEllipse.cpp
# End Source File
# Begin Source File

SOURCE=.\Tools\XYPolygon.cpp
# End Source File
# Begin Source File

SOURCE=.\Tools\XYPolynom.cpp
# End Source File
# Begin Source File

SOURCE=.\Tools\XYRect.cpp
# End Source File
# End Group
# Begin Group "Numbering"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Numbering\CalcAprImageMatr.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\InterfSolver.cpp
# End Source File
# Begin Source File

SOURCE=.\InterfSolver.def
# End Source File
# Begin Source File

SOURCE=.\InterfSolver.rc
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "Tools_H"

# PROP Default_Filter ""
# End Group
# Begin Group "Include_H"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Tools\CalcContour.h
# End Source File
# Begin Source File

SOURCE=.\Tools\CalcLimits.h
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\CArrDef.h
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\complex.h
# End Source File
# Begin Source File

SOURCE=.\Tools\CTIOFile.h
# End Source File
# Begin Source File

SOURCE=.\Tools\GetTimeDate.h
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\Int_Cons.h
# End Source File
# Begin Source File

SOURCE=.\Tools\isPupil.h
# End Source File
# Begin Source File

SOURCE=.\Tools\Matin1.h
# End Source File
# Begin Source File

SOURCE=.\Tools\Matrix.h
# End Source File
# Begin Source File

SOURCE=.\Tools\NunberInfo.h
# End Source File
# Begin Source File

SOURCE=.\Tools\PolApproximation.h
# End Source File
# Begin Source File

SOURCE=.\Tools\SamplDat.h
# End Source File
# Begin Source File

SOURCE=.\Tools\Vector.h
# End Source File
# Begin Source File

SOURCE=.\Tools\XYBounds.h
# End Source File
# Begin Source File

SOURCE=.\Tools\XYBrokenLine.h
# End Source File
# Begin Source File

SOURCE=.\Tools\XYEllipse.h
# End Source File
# Begin Source File

SOURCE=.\Tools\XYPoint.h
# End Source File
# Begin Source File

SOURCE=.\Tools\XYPolygon.h
# End Source File
# Begin Source File

SOURCE=.\Tools\XYPolynom.h
# End Source File
# Begin Source File

SOURCE=.\Tools\XYRect.h
# End Source File
# End Group
# Begin Group "Numbering_H"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Numbering\CalcAprImageMatr.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\InterfSolver.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\InterfSolver.rc2
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
