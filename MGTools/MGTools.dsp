# Microsoft Developer Studio Project File - Name="MGTools" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=MGTools - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "MGTools.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MGTools.mak" CFG="MGTools - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MGTools - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "MGTools - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=xicl6.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "MGTools - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MGTOOLS_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MGTOOLS_EXPORTS" /D "_WINDLL" /D "_AFXDLL" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 opengl32.lib glu32.lib glut32.lib /nologo /dll /map /machine:I386

!ELSEIF  "$(CFG)" == "MGTools - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MGTOOLS_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MGTOOLS_EXPORTS" /D "_WINDLL" /D "_AFXDLL" /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 opengl32.lib glu32.lib glut32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# SUBTRACT LINK32 /map

!ENDIF 

# Begin Target

# Name "MGTools - Win32 Release"
# Name "MGTools - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "Utils"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Source\Utils\BaseDataType.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Utils\BookMark.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Utils\DibApi.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Utils\Dibfile.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Utils\filesys.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Utils\LogoBmp.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Utils\ModulVer.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Utils\Msgqueue.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Utils\OXCoolToolBar.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Utils\SecMatrix.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Utils\Splash.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Utils\Utils.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Utils\Xstatus4.cpp
# End Source File
# End Group
# Begin Group "Graph"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Source\Graph\2DGraph.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Graph\3DGraph.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Graph\contour.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Graph\DataType.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Graph\GraphFrame.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Graph\GraphTools.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Graph\IsolineGraph.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Graph\MapGraph.cpp
# End Source File
# End Group
# Begin Group "Image"

# PROP Default_Filter ""
# Begin Group "Jpeg"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Source\Image\JPEG\JCAPIMIN.CPP
# End Source File
# Begin Source File

SOURCE=.\Source\Image\JPEG\JCAPISTD.CPP
# End Source File
# Begin Source File

SOURCE=.\Source\Image\JPEG\JCARITH.CPP
# End Source File
# Begin Source File

SOURCE=.\Source\Image\JPEG\Jccoefct.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Image\JPEG\Jccolor.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Image\JPEG\JCDCTMGR.CPP
# End Source File
# Begin Source File

SOURCE=.\Source\Image\JPEG\Jchuff.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Image\JPEG\Jcinit.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Image\JPEG\JCMAINCT.CPP
# End Source File
# Begin Source File

SOURCE=.\Source\Image\JPEG\Jcmarker.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Image\JPEG\Jcmaster.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Image\JPEG\JCOMAPI.CPP
# End Source File
# Begin Source File

SOURCE=.\Source\Image\JPEG\JCPARAM.CPP
# End Source File
# Begin Source File

SOURCE=.\Source\Image\JPEG\Jcphuff.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Image\JPEG\JCPREPCT.CPP
# End Source File
# Begin Source File

SOURCE=.\Source\Image\JPEG\Jcsample.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Image\JPEG\Jctrans.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Image\JPEG\JDAPIMIN.CPP
# End Source File
# Begin Source File

SOURCE=.\Source\Image\JPEG\JDAPISTD.CPP
# End Source File
# Begin Source File

SOURCE=.\Source\Image\JPEG\JDARITH.CPP
# End Source File
# Begin Source File

SOURCE=.\Source\Image\JPEG\Jdatadst.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Image\JPEG\Jdatasrc.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Image\JPEG\Jdcoefct.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Image\JPEG\Jdcolor.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Image\JPEG\JDDCTMGR.CPP
# End Source File
# Begin Source File

SOURCE=.\Source\Image\JPEG\JDHUFF.CPP
# End Source File
# Begin Source File

SOURCE=.\Source\Image\JPEG\JDINPUT.CPP
# End Source File
# Begin Source File

SOURCE=.\Source\Image\JPEG\JDMAINCT.CPP
# End Source File
# Begin Source File

SOURCE=.\Source\Image\JPEG\Jdmarker.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Image\JPEG\Jdmaster.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Image\JPEG\Jdmerge.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Image\JPEG\Jdphuff.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Image\JPEG\Jdpostct.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Image\JPEG\Jdsample.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Image\JPEG\JDTRANS.CPP
# End Source File
# Begin Source File

SOURCE=.\Source\Image\JPEG\JERROR.CPP
# End Source File
# Begin Source File

SOURCE=.\Source\Image\JPEG\JFDCTFLT.CPP
# End Source File
# Begin Source File

SOURCE=.\Source\Image\JPEG\JFDCTFST.CPP
# End Source File
# Begin Source File

SOURCE=.\Source\Image\JPEG\JFDCTINT.CPP
# End Source File
# Begin Source File

SOURCE=.\Source\Image\JPEG\JIDCTFLT.CPP
# End Source File
# Begin Source File

SOURCE=.\Source\Image\JPEG\JIDCTFST.CPP
# End Source File
# Begin Source File

SOURCE=.\Source\Image\JPEG\JIDCTINT.CPP
# End Source File
# Begin Source File

SOURCE=.\Source\Image\JPEG\JIDCTRED.CPP
# End Source File
# Begin Source File

SOURCE=.\Source\Image\JPEG\Jmemansi.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Image\JPEG\Jmemmgr.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Image\JPEG\Jquant1.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Image\JPEG\Jquant2.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Image\JPEG\JUTILS.CPP
# End Source File
# End Group
# Begin Group "Tiff"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Source\Image\TIFF\TIF_AUX.CPP
# End Source File
# Begin Source File

SOURCE=.\Source\Image\TIFF\TIF_CCIT.CPP
# End Source File
# Begin Source File

SOURCE=.\Source\Image\TIFF\TIF_CLOS.CPP
# End Source File
# Begin Source File

SOURCE=.\Source\Image\TIFF\TIF_COMP.CPP
# End Source File
# Begin Source File

SOURCE=.\Source\Image\TIFF\TIF_DIR.CPP
# End Source File
# Begin Source File

SOURCE=.\Source\Image\TIFF\TIF_DIRI.CPP
# End Source File
# Begin Source File

SOURCE=.\Source\Image\TIFF\TIF_DIRR.CPP
# End Source File
# Begin Source File

SOURCE=.\Source\Image\TIFF\TIF_DIRW.CPP
# End Source File
# Begin Source File

SOURCE=.\Source\Image\TIFF\TIF_DUMP.CPP
# End Source File
# Begin Source File

SOURCE=.\Source\Image\TIFF\TIF_ERRO.CPP
# End Source File
# Begin Source File

SOURCE=.\Source\Image\TIFF\TIF_FAX3.CPP
# End Source File
# Begin Source File

SOURCE=.\Source\Image\TIFF\TIF_FAX4.CPP
# End Source File
# Begin Source File

SOURCE=.\Source\Image\TIFF\TIF_FLUS.CPP
# End Source File
# Begin Source File

SOURCE=.\Source\Image\TIFF\TIF_GETI.CPP
# End Source File
# Begin Source File

SOURCE=.\Source\Image\TIFF\TIF_JPEG.CPP
# End Source File
# Begin Source File

SOURCE=.\Source\Image\TIFF\TIF_LZW.CPP
# End Source File
# Begin Source File

SOURCE=.\Source\Image\TIFF\TIF_MSDO.CPP
# End Source File
# Begin Source File

SOURCE=.\Source\Image\TIFF\TIF_OPEN.CPP
# End Source File
# Begin Source File

SOURCE=.\Source\Image\TIFF\TIF_PACK.CPP
# End Source File
# Begin Source File

SOURCE=.\Source\Image\TIFF\TIF_PRIN.CPP
# End Source File
# Begin Source File

SOURCE=.\Source\Image\TIFF\TIF_READ.CPP
# End Source File
# Begin Source File

SOURCE=.\Source\Image\TIFF\TIF_STRI.CPP
# End Source File
# Begin Source File

SOURCE=.\Source\Image\TIFF\TIF_SWAB.CPP
# End Source File
# Begin Source File

SOURCE=.\Source\Image\TIFF\TIF_THUN.CPP
# End Source File
# Begin Source File

SOURCE=.\Source\Image\TIFF\TIF_TILE.CPP
# End Source File
# Begin Source File

SOURCE=.\Source\Image\TIFF\TIF_VERS.CPP
# End Source File
# Begin Source File

SOURCE=.\Source\Image\TIFF\TIF_WARN.CPP
# End Source File
# Begin Source File

SOURCE=.\Source\Image\TIFF\TIF_WRIT.CPP
# End Source File
# End Group
# Begin Source File

SOURCE=.\Source\Image\SECDIB.CPP
# End Source File
# Begin Source File

SOURCE=.\Source\Image\SECDSP.CPP
# End Source File
# Begin Source File

SOURCE=.\Source\Image\SECGIF.CPP
# End Source File
# Begin Source File

SOURCE=.\Source\Image\SECIMAGE.CPP
# End Source File
# Begin Source File

SOURCE=.\Source\Image\SECJPEG.CPP
# End Source File
# Begin Source File

SOURCE=.\Source\Image\SECPCX.CPP
# End Source File
# Begin Source File

SOURCE=.\Source\Image\SECTARGA.CPP
# End Source File
# Begin Source File

SOURCE=.\Source\Image\SECTIFF.CPP
# End Source File
# End Group
# Begin Group "WndTabs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Source\WndTabs\secaux.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\WndTabs\tabctrl.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\WndTabs\tabctrl3.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\WndTabs\tabctrlb.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\WndTabs\tabwnd.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\WndTabs\tabwnd3.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\WndTabs\tabwndb.cpp
# End Source File
# End Group
# Begin Group "SysInfo"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Source\SysInfo\Cpuid.c
# End Source File
# Begin Source File

SOURCE=.\Source\SysInfo\OXSysInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\SysInfo\Speed.c
# End Source File
# End Group
# Begin Source File

SOURCE=.\MGTools.cpp
# End Source File
# Begin Source File

SOURCE=.\MGTools.def
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "Utils_H"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Include\Utils\BookMark.h
# End Source File
# Begin Source File

SOURCE=.\Source\Utils\BookMark.h
# End Source File
# Begin Source File

SOURCE=.\Include\Utils\DataType.h
# End Source File
# Begin Source File

SOURCE=.\Include\Utils\DibApi.h
# End Source File
# Begin Source File

SOURCE=.\Include\Utils\filesys.h
# End Source File
# Begin Source File

SOURCE=.\Include\Utils\Fixmem.h
# End Source File
# Begin Source File

SOURCE=.\Include\Utils\LogoBmp.h
# End Source File
# Begin Source File

SOURCE=.\Include\Utils\memdc.h
# End Source File
# Begin Source File

SOURCE=.\Include\Utils\ModulVer.h
# End Source File
# Begin Source File

SOURCE=.\Include\Utils\MsgQueue.h
# End Source File
# Begin Source File

SOURCE=.\Include\Utils\NotificationCode.h
# End Source File
# Begin Source File

SOURCE=.\Include\Utils\OXCoolToolBar.h
# End Source File
# Begin Source File

SOURCE=.\Include\Utils\OXDllExt.h
# End Source File
# Begin Source File

SOURCE=.\Include\Utils\SecMatrix.h
# End Source File
# Begin Source File

SOURCE=.\Include\Utils\Splash.h
# End Source File
# Begin Source File

SOURCE=.\Include\Utils\Utils.h
# End Source File
# Begin Source File

SOURCE=.\Include\Utils\XSTATUS4.h
# End Source File
# End Group
# Begin Group "Graph_H"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Include\Graph\2DGraph.h
# End Source File
# Begin Source File

SOURCE=.\Include\Graph\3DGraph.h
# End Source File
# Begin Source File

SOURCE=.\Include\Utils\BaseDataType.h
# End Source File
# Begin Source File

SOURCE=.\Include\Graph\contour.h
# End Source File
# Begin Source File

SOURCE=.\Include\Graph\def.h
# End Source File
# Begin Source File

SOURCE=.\Include\Graph\glut.h
# End Source File
# Begin Source File

SOURCE=.\Include\Graph\GraphFrame.h
# End Source File
# Begin Source File

SOURCE=.\Include\Graph\GraphTools.h
# End Source File
# Begin Source File

SOURCE=.\Include\Graph\IsolineGraph.h
# End Source File
# Begin Source File

SOURCE=.\Include\Graph\MapGraph.h
# End Source File
# End Group
# Begin Group "Image_H"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Include\Image\JCHUFF.H
# End Source File
# Begin Source File

SOURCE=.\Include\Image\JCONFIG.H
# End Source File
# Begin Source File

SOURCE=.\Include\Image\JDCT.H
# End Source File
# Begin Source File

SOURCE=.\Include\Image\JDHUFF.H
# End Source File
# Begin Source File

SOURCE=.\Include\Image\JERROR.H
# End Source File
# Begin Source File

SOURCE=.\Include\Image\JINCLUDE.H
# End Source File
# Begin Source File

SOURCE=.\Include\Image\JMEMSYS.H
# End Source File
# Begin Source File

SOURCE=.\Include\Image\JMORECFG.H
# End Source File
# Begin Source File

SOURCE=.\Include\Image\Jpegint.h
# End Source File
# Begin Source File

SOURCE=.\Include\Image\Jpeglib.h
# End Source File
# Begin Source File

SOURCE=.\Include\Image\JVERSION.H
# End Source File
# Begin Source File

SOURCE=.\Include\Image\ot_sectiff.h
# End Source File
# Begin Source File

SOURCE=.\Include\Image\SECDIB.H
# End Source File
# Begin Source File

SOURCE=.\Include\Image\SECGIF.H
# End Source File
# Begin Source File

SOURCE=.\Include\Image\SECIMAGE.H
# End Source File
# Begin Source File

SOURCE=.\Include\Image\SECJPEG.H
# End Source File
# Begin Source File

SOURCE=.\Include\Image\SECPCX.H
# End Source File
# Begin Source File

SOURCE=.\Include\Image\SECTARGA.H
# End Source File
# Begin Source File

SOURCE=.\Include\Image\SECTIFF.H
# End Source File
# Begin Source File

SOURCE=.\Include\Image\T4.H
# End Source File
# Begin Source File

SOURCE=.\Include\Image\TIF_FAX3.H
# End Source File
# Begin Source File

SOURCE=.\Include\Image\TIFF.H
# End Source File
# Begin Source File

SOURCE=.\Include\Image\TIFFCOMP.H
# End Source File
# Begin Source File

SOURCE=.\Include\Image\TIFFCONF.H
# End Source File
# Begin Source File

SOURCE=.\Include\Image\TIFFIO.H
# End Source File
# Begin Source File

SOURCE=.\Include\Image\TIFFIOP.H
# End Source File
# End Group
# Begin Group "WndTabs_H"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Include\WndTabs\osc_cmnres.h
# End Source File
# Begin Source File

SOURCE=.\Include\WndTabs\secaux.h
# End Source File
# Begin Source File

SOURCE=.\Include\WndTabs\SECRES.H
# End Source File
# Begin Source File

SOURCE=.\Include\WndTabs\tabctrl3.h
# End Source File
# Begin Source File

SOURCE=.\Include\WndTabs\tabctrlb.h
# End Source File
# Begin Source File

SOURCE=.\Include\WndTabs\tabwnd.h
# End Source File
# Begin Source File

SOURCE=.\Include\WndTabs\tabwnd3.h
# End Source File
# Begin Source File

SOURCE=.\Include\WndTabs\tabwndb.h
# End Source File
# End Group
# Begin Group "SysInfo_H"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Include\SysInfo\Cpuid.h
# End Source File
# Begin Source File

SOURCE=.\Include\SysInfo\OXDllExt.h
# End Source File
# Begin Source File

SOURCE=.\Include\SysInfo\OXSysInfo.h
# End Source File
# Begin Source File

SOURCE=.\Include\SysInfo\Speed.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\MGTools.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
