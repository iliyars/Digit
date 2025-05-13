# Microsoft Developer Studio Generated NMAKE File, Based on Digit.dsp
!IF "$(CFG)" == ""
CFG=Digit - Win32 Debug
!MESSAGE No configuration specified. Defaulting to Digit - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Digit - Win32 Release" && "$(CFG)" != "Digit - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
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
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "Digit - Win32 Release"

OUTDIR=.\..\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\..\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : ".\..\Release\Digit.hlp" ".\..\Release\Digit.cnt" "$(OUTDIR)\Digit.exe" ".\Release\Digit.pch" "$(OUTDIR)\Digit.bsc"

!ELSE 

ALL : "RusLangD - Win32 Release" "EngLangD - Win32 Release" ".\..\Release\Digit.hlp" ".\..\Release\Digit.cnt" "$(OUTDIR)\Digit.exe" ".\Release\Digit.pch" "$(OUTDIR)\Digit.bsc"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"EngLangD - Win32 ReleaseCLEAN" "RusLangD - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\3DGraphDoc.obj"
	-@erase "$(INTDIR)\3DGraphDoc.sbr"
	-@erase "$(INTDIR)\ApproxSetDlg.obj"
	-@erase "$(INTDIR)\ApproxSetDlg.sbr"
	-@erase "$(INTDIR)\BaseImageDoc.obj"
	-@erase "$(INTDIR)\BaseImageDoc.sbr"
	-@erase "$(INTDIR)\BaseImageView.obj"
	-@erase "$(INTDIR)\BaseImageView.sbr"
	-@erase "$(INTDIR)\BaseTextDoc.obj"
	-@erase "$(INTDIR)\BaseTextDoc.sbr"
	-@erase "$(INTDIR)\BaseTextFrm.obj"
	-@erase "$(INTDIR)\BaseTextFrm.sbr"
	-@erase "$(INTDIR)\BaseTextView.obj"
	-@erase "$(INTDIR)\BaseTextView.sbr"
	-@erase "$(INTDIR)\BoundCtrls.obj"
	-@erase "$(INTDIR)\BoundCtrls.sbr"
	-@erase "$(INTDIR)\Cmdlg.obj"
	-@erase "$(INTDIR)\Cmdlg.sbr"
	-@erase "$(INTDIR)\CommentsFileDlg.obj"
	-@erase "$(INTDIR)\CommentsFileDlg.sbr"
	-@erase "$(INTDIR)\contour.obj"
	-@erase "$(INTDIR)\contour.sbr"
	-@erase "$(INTDIR)\controls.obj"
	-@erase "$(INTDIR)\controls.sbr"
	-@erase "$(INTDIR)\Digit.obj"
	-@erase "$(INTDIR)\Digit.pch"
	-@erase "$(INTDIR)\Digit.res"
	-@erase "$(INTDIR)\Digit.sbr"
	-@erase "$(INTDIR)\DigitInfo.obj"
	-@erase "$(INTDIR)\DigitInfo.sbr"
	-@erase "$(INTDIR)\DotInfo.obj"
	-@erase "$(INTDIR)\DotInfo.sbr"
	-@erase "$(INTDIR)\GraphChildFrm.obj"
	-@erase "$(INTDIR)\GraphChildFrm.sbr"
	-@erase "$(INTDIR)\GraphDoc.obj"
	-@erase "$(INTDIR)\GraphDoc.sbr"
	-@erase "$(INTDIR)\ImageChildFrm.obj"
	-@erase "$(INTDIR)\ImageChildFrm.sbr"
	-@erase "$(INTDIR)\ImageCtrls.obj"
	-@erase "$(INTDIR)\ImageCtrls.sbr"
	-@erase "$(INTDIR)\ImageDoc.obj"
	-@erase "$(INTDIR)\ImageDoc.sbr"
	-@erase "$(INTDIR)\ImageView.obj"
	-@erase "$(INTDIR)\ImageView.sbr"
	-@erase "$(INTDIR)\MainFrm.obj"
	-@erase "$(INTDIR)\MainFrm.sbr"
	-@erase "$(INTDIR)\MeasureCtrls.obj"
	-@erase "$(INTDIR)\MeasureCtrls.sbr"
	-@erase "$(INTDIR)\middle.obj"
	-@erase "$(INTDIR)\middle.sbr"
	-@erase "$(INTDIR)\mutils.obj"
	-@erase "$(INTDIR)\mutils.sbr"
	-@erase "$(INTDIR)\SectionFrame.obj"
	-@erase "$(INTDIR)\SectionFrame.sbr"
	-@erase "$(INTDIR)\SectionInfo.obj"
	-@erase "$(INTDIR)\SectionInfo.sbr"
	-@erase "$(INTDIR)\SpecFileDlg.obj"
	-@erase "$(INTDIR)\SpecFileDlg.sbr"
	-@erase "$(INTDIR)\StateListBox.obj"
	-@erase "$(INTDIR)\StateListBox.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\TextChildFrm.obj"
	-@erase "$(INTDIR)\TextChildFrm.sbr"
	-@erase "$(INTDIR)\ToolBarsDlg.obj"
	-@erase "$(INTDIR)\ToolBarsDlg.sbr"
	-@erase "$(INTDIR)\Tracker.obj"
	-@erase "$(INTDIR)\Tracker.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\ZapLineInfo.obj"
	-@erase "$(INTDIR)\ZapLineInfo.sbr"
	-@erase "$(INTDIR)\ZoomDlg.obj"
	-@erase "$(INTDIR)\ZoomDlg.sbr"
	-@erase "$(INTDIR)\ZOOMVIEW.OBJ"
	-@erase "$(INTDIR)\ZOOMVIEW.SBR"
	-@erase "$(OUTDIR)\Digit.bsc"
	-@erase "$(OUTDIR)\Digit.exe"
	-@erase ".\..\Release\Digit.cnt"
	-@erase ".\..\Release\Digit.hlp"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MD /W3 /GX /O2 /I "..\..\MGTools" /I "..\..\MGTools\Include" /I "..\InterfSolver" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\Digit.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Digit.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\CommentsFileDlg.sbr" \
	"$(INTDIR)\MainFrm.sbr" \
	"$(INTDIR)\BoundCtrls.sbr" \
	"$(INTDIR)\controls.sbr" \
	"$(INTDIR)\ImageCtrls.sbr" \
	"$(INTDIR)\BaseTextDoc.sbr" \
	"$(INTDIR)\BaseTextFrm.sbr" \
	"$(INTDIR)\BaseTextView.sbr" \
	"$(INTDIR)\TextChildFrm.sbr" \
	"$(INTDIR)\3DGraphDoc.sbr" \
	"$(INTDIR)\GraphChildFrm.sbr" \
	"$(INTDIR)\GraphDoc.sbr" \
	"$(INTDIR)\Cmdlg.sbr" \
	"$(INTDIR)\contour.sbr" \
	"$(INTDIR)\MeasureCtrls.sbr" \
	"$(INTDIR)\middle.sbr" \
	"$(INTDIR)\mutils.sbr" \
	"$(INTDIR)\SpecFileDlg.sbr" \
	"$(INTDIR)\StateListBox.sbr" \
	"$(INTDIR)\Tracker.sbr" \
	"$(INTDIR)\ZOOMVIEW.SBR" \
	"$(INTDIR)\BaseImageDoc.sbr" \
	"$(INTDIR)\BaseImageView.sbr" \
	"$(INTDIR)\ImageChildFrm.sbr" \
	"$(INTDIR)\ImageDoc.sbr" \
	"$(INTDIR)\ImageView.sbr" \
	"$(INTDIR)\ApproxSetDlg.sbr" \
	"$(INTDIR)\ToolBarsDlg.sbr" \
	"$(INTDIR)\SectionFrame.sbr" \
	"$(INTDIR)\ZoomDlg.sbr" \
	"$(INTDIR)\DigitInfo.sbr" \
	"$(INTDIR)\DotInfo.sbr" \
	"$(INTDIR)\SectionInfo.sbr" \
	"$(INTDIR)\ZapLineInfo.sbr" \
	"$(INTDIR)\Digit.sbr" \
	"$(INTDIR)\StdAfx.sbr"

"$(OUTDIR)\Digit.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=..\..\MGTools\Release\MGTools.lib ..\Release\InterfSolver.lib /nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\Digit.pdb" /machine:I386 /out:"$(OUTDIR)\Digit.exe" 
LINK32_OBJS= \
	"$(INTDIR)\CommentsFileDlg.obj" \
	"$(INTDIR)\MainFrm.obj" \
	"$(INTDIR)\BoundCtrls.obj" \
	"$(INTDIR)\controls.obj" \
	"$(INTDIR)\ImageCtrls.obj" \
	"$(INTDIR)\BaseTextDoc.obj" \
	"$(INTDIR)\BaseTextFrm.obj" \
	"$(INTDIR)\BaseTextView.obj" \
	"$(INTDIR)\TextChildFrm.obj" \
	"$(INTDIR)\3DGraphDoc.obj" \
	"$(INTDIR)\GraphChildFrm.obj" \
	"$(INTDIR)\GraphDoc.obj" \
	"$(INTDIR)\Cmdlg.obj" \
	"$(INTDIR)\contour.obj" \
	"$(INTDIR)\MeasureCtrls.obj" \
	"$(INTDIR)\middle.obj" \
	"$(INTDIR)\mutils.obj" \
	"$(INTDIR)\SpecFileDlg.obj" \
	"$(INTDIR)\StateListBox.obj" \
	"$(INTDIR)\Tracker.obj" \
	"$(INTDIR)\ZOOMVIEW.OBJ" \
	"$(INTDIR)\BaseImageDoc.obj" \
	"$(INTDIR)\BaseImageView.obj" \
	"$(INTDIR)\ImageChildFrm.obj" \
	"$(INTDIR)\ImageDoc.obj" \
	"$(INTDIR)\ImageView.obj" \
	"$(INTDIR)\ApproxSetDlg.obj" \
	"$(INTDIR)\ToolBarsDlg.obj" \
	"$(INTDIR)\SectionFrame.obj" \
	"$(INTDIR)\ZoomDlg.obj" \
	"$(INTDIR)\DigitInfo.obj" \
	"$(INTDIR)\DotInfo.obj" \
	"$(INTDIR)\SectionInfo.obj" \
	"$(INTDIR)\ZapLineInfo.obj" \
	"$(INTDIR)\Digit.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\Digit.res" \
	"$(OUTDIR)\EngLangD.lib" \
	"$(OUTDIR)\RusLangD.lib"

"$(OUTDIR)\Digit.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Digit - Win32 Debug"

OUTDIR=.\..\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\..\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : ".\..\Debug\Digit.hlp" ".\..\Debug\Digit.cnt" "$(OUTDIR)\Digit.exe" ".\Debug\Digit.pch" "$(OUTDIR)\Digit.bsc"

!ELSE 

ALL : "RusLangD - Win32 Debug" "EngLangD - Win32 Debug" ".\..\Debug\Digit.hlp" ".\..\Debug\Digit.cnt" "$(OUTDIR)\Digit.exe" ".\Debug\Digit.pch" "$(OUTDIR)\Digit.bsc"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"EngLangD - Win32 DebugCLEAN" "RusLangD - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\3DGraphDoc.obj"
	-@erase "$(INTDIR)\3DGraphDoc.sbr"
	-@erase "$(INTDIR)\ApproxSetDlg.obj"
	-@erase "$(INTDIR)\ApproxSetDlg.sbr"
	-@erase "$(INTDIR)\BaseImageDoc.obj"
	-@erase "$(INTDIR)\BaseImageDoc.sbr"
	-@erase "$(INTDIR)\BaseImageView.obj"
	-@erase "$(INTDIR)\BaseImageView.sbr"
	-@erase "$(INTDIR)\BaseTextDoc.obj"
	-@erase "$(INTDIR)\BaseTextDoc.sbr"
	-@erase "$(INTDIR)\BaseTextFrm.obj"
	-@erase "$(INTDIR)\BaseTextFrm.sbr"
	-@erase "$(INTDIR)\BaseTextView.obj"
	-@erase "$(INTDIR)\BaseTextView.sbr"
	-@erase "$(INTDIR)\BoundCtrls.obj"
	-@erase "$(INTDIR)\BoundCtrls.sbr"
	-@erase "$(INTDIR)\Cmdlg.obj"
	-@erase "$(INTDIR)\Cmdlg.sbr"
	-@erase "$(INTDIR)\CommentsFileDlg.obj"
	-@erase "$(INTDIR)\CommentsFileDlg.sbr"
	-@erase "$(INTDIR)\contour.obj"
	-@erase "$(INTDIR)\contour.sbr"
	-@erase "$(INTDIR)\controls.obj"
	-@erase "$(INTDIR)\controls.sbr"
	-@erase "$(INTDIR)\Digit.obj"
	-@erase "$(INTDIR)\Digit.pch"
	-@erase "$(INTDIR)\Digit.res"
	-@erase "$(INTDIR)\Digit.sbr"
	-@erase "$(INTDIR)\DigitInfo.obj"
	-@erase "$(INTDIR)\DigitInfo.sbr"
	-@erase "$(INTDIR)\DotInfo.obj"
	-@erase "$(INTDIR)\DotInfo.sbr"
	-@erase "$(INTDIR)\GraphChildFrm.obj"
	-@erase "$(INTDIR)\GraphChildFrm.sbr"
	-@erase "$(INTDIR)\GraphDoc.obj"
	-@erase "$(INTDIR)\GraphDoc.sbr"
	-@erase "$(INTDIR)\ImageChildFrm.obj"
	-@erase "$(INTDIR)\ImageChildFrm.sbr"
	-@erase "$(INTDIR)\ImageCtrls.obj"
	-@erase "$(INTDIR)\ImageCtrls.sbr"
	-@erase "$(INTDIR)\ImageDoc.obj"
	-@erase "$(INTDIR)\ImageDoc.sbr"
	-@erase "$(INTDIR)\ImageView.obj"
	-@erase "$(INTDIR)\ImageView.sbr"
	-@erase "$(INTDIR)\MainFrm.obj"
	-@erase "$(INTDIR)\MainFrm.sbr"
	-@erase "$(INTDIR)\MeasureCtrls.obj"
	-@erase "$(INTDIR)\MeasureCtrls.sbr"
	-@erase "$(INTDIR)\middle.obj"
	-@erase "$(INTDIR)\middle.sbr"
	-@erase "$(INTDIR)\mutils.obj"
	-@erase "$(INTDIR)\mutils.sbr"
	-@erase "$(INTDIR)\SectionFrame.obj"
	-@erase "$(INTDIR)\SectionFrame.sbr"
	-@erase "$(INTDIR)\SectionInfo.obj"
	-@erase "$(INTDIR)\SectionInfo.sbr"
	-@erase "$(INTDIR)\SpecFileDlg.obj"
	-@erase "$(INTDIR)\SpecFileDlg.sbr"
	-@erase "$(INTDIR)\StateListBox.obj"
	-@erase "$(INTDIR)\StateListBox.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\TextChildFrm.obj"
	-@erase "$(INTDIR)\TextChildFrm.sbr"
	-@erase "$(INTDIR)\ToolBarsDlg.obj"
	-@erase "$(INTDIR)\ToolBarsDlg.sbr"
	-@erase "$(INTDIR)\Tracker.obj"
	-@erase "$(INTDIR)\Tracker.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\ZapLineInfo.obj"
	-@erase "$(INTDIR)\ZapLineInfo.sbr"
	-@erase "$(INTDIR)\ZoomDlg.obj"
	-@erase "$(INTDIR)\ZoomDlg.sbr"
	-@erase "$(INTDIR)\ZOOMVIEW.OBJ"
	-@erase "$(INTDIR)\ZOOMVIEW.SBR"
	-@erase "$(OUTDIR)\Digit.bsc"
	-@erase "$(OUTDIR)\Digit.exe"
	-@erase "$(OUTDIR)\Digit.ilk"
	-@erase "$(OUTDIR)\Digit.pdb"
	-@erase ".\..\Debug\Digit.cnt"
	-@erase ".\..\Debug\Digit.hlp"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G6 /MDd /W3 /Gm /GX /ZI /Od /I "..\..\MGTools" /I "..\..\MGTools\Include" /I "..\InterfSolver" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\Digit.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Digit.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\CommentsFileDlg.sbr" \
	"$(INTDIR)\MainFrm.sbr" \
	"$(INTDIR)\BoundCtrls.sbr" \
	"$(INTDIR)\controls.sbr" \
	"$(INTDIR)\ImageCtrls.sbr" \
	"$(INTDIR)\BaseTextDoc.sbr" \
	"$(INTDIR)\BaseTextFrm.sbr" \
	"$(INTDIR)\BaseTextView.sbr" \
	"$(INTDIR)\TextChildFrm.sbr" \
	"$(INTDIR)\3DGraphDoc.sbr" \
	"$(INTDIR)\GraphChildFrm.sbr" \
	"$(INTDIR)\GraphDoc.sbr" \
	"$(INTDIR)\Cmdlg.sbr" \
	"$(INTDIR)\contour.sbr" \
	"$(INTDIR)\MeasureCtrls.sbr" \
	"$(INTDIR)\middle.sbr" \
	"$(INTDIR)\mutils.sbr" \
	"$(INTDIR)\SpecFileDlg.sbr" \
	"$(INTDIR)\StateListBox.sbr" \
	"$(INTDIR)\Tracker.sbr" \
	"$(INTDIR)\ZOOMVIEW.SBR" \
	"$(INTDIR)\BaseImageDoc.sbr" \
	"$(INTDIR)\BaseImageView.sbr" \
	"$(INTDIR)\ImageChildFrm.sbr" \
	"$(INTDIR)\ImageDoc.sbr" \
	"$(INTDIR)\ImageView.sbr" \
	"$(INTDIR)\ApproxSetDlg.sbr" \
	"$(INTDIR)\ToolBarsDlg.sbr" \
	"$(INTDIR)\SectionFrame.sbr" \
	"$(INTDIR)\ZoomDlg.sbr" \
	"$(INTDIR)\DigitInfo.sbr" \
	"$(INTDIR)\DotInfo.sbr" \
	"$(INTDIR)\SectionInfo.sbr" \
	"$(INTDIR)\ZapLineInfo.sbr" \
	"$(INTDIR)\Digit.sbr" \
	"$(INTDIR)\StdAfx.sbr"

"$(OUTDIR)\Digit.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=..\..\MGTools\Debug\MGTools.lib ..\Debug\InterfSolver.lib /nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\Digit.pdb" /debug /machine:I386 /out:"$(OUTDIR)\Digit.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\CommentsFileDlg.obj" \
	"$(INTDIR)\MainFrm.obj" \
	"$(INTDIR)\BoundCtrls.obj" \
	"$(INTDIR)\controls.obj" \
	"$(INTDIR)\ImageCtrls.obj" \
	"$(INTDIR)\BaseTextDoc.obj" \
	"$(INTDIR)\BaseTextFrm.obj" \
	"$(INTDIR)\BaseTextView.obj" \
	"$(INTDIR)\TextChildFrm.obj" \
	"$(INTDIR)\3DGraphDoc.obj" \
	"$(INTDIR)\GraphChildFrm.obj" \
	"$(INTDIR)\GraphDoc.obj" \
	"$(INTDIR)\Cmdlg.obj" \
	"$(INTDIR)\contour.obj" \
	"$(INTDIR)\MeasureCtrls.obj" \
	"$(INTDIR)\middle.obj" \
	"$(INTDIR)\mutils.obj" \
	"$(INTDIR)\SpecFileDlg.obj" \
	"$(INTDIR)\StateListBox.obj" \
	"$(INTDIR)\Tracker.obj" \
	"$(INTDIR)\ZOOMVIEW.OBJ" \
	"$(INTDIR)\BaseImageDoc.obj" \
	"$(INTDIR)\BaseImageView.obj" \
	"$(INTDIR)\ImageChildFrm.obj" \
	"$(INTDIR)\ImageDoc.obj" \
	"$(INTDIR)\ImageView.obj" \
	"$(INTDIR)\ApproxSetDlg.obj" \
	"$(INTDIR)\ToolBarsDlg.obj" \
	"$(INTDIR)\SectionFrame.obj" \
	"$(INTDIR)\ZoomDlg.obj" \
	"$(INTDIR)\DigitInfo.obj" \
	"$(INTDIR)\DotInfo.obj" \
	"$(INTDIR)\SectionInfo.obj" \
	"$(INTDIR)\ZapLineInfo.obj" \
	"$(INTDIR)\Digit.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\Digit.res" \
	"$(OUTDIR)\EngLangD.lib" \
	"$(OUTDIR)\RusLangD.lib"

"$(OUTDIR)\Digit.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("Digit.dep")
!INCLUDE "Digit.dep"
!ELSE 
!MESSAGE Warning: cannot find "Digit.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "Digit - Win32 Release" || "$(CFG)" == "Digit - Win32 Debug"
SOURCE=.\MainFrame\CommentsFileDlg.cpp

"$(INTDIR)\CommentsFileDlg.obj"	"$(INTDIR)\CommentsFileDlg.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\MainFrame\MainFrm.cpp

"$(INTDIR)\MainFrm.obj"	"$(INTDIR)\MainFrm.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\Controls\BoundCtrls.cpp

"$(INTDIR)\BoundCtrls.obj"	"$(INTDIR)\BoundCtrls.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\Controls\controls.cpp

"$(INTDIR)\controls.obj"	"$(INTDIR)\controls.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\Controls\ImageCtrls.cpp

"$(INTDIR)\ImageCtrls.obj"	"$(INTDIR)\ImageCtrls.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\Utils\Edit\BaseTextDoc.cpp

"$(INTDIR)\BaseTextDoc.obj"	"$(INTDIR)\BaseTextDoc.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\Utils\Edit\BaseTextFrm.cpp

"$(INTDIR)\BaseTextFrm.obj"	"$(INTDIR)\BaseTextFrm.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\Utils\Edit\BaseTextView.cpp

"$(INTDIR)\BaseTextView.obj"	"$(INTDIR)\BaseTextView.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\Utils\Edit\TextChildFrm.cpp

"$(INTDIR)\TextChildFrm.obj"	"$(INTDIR)\TextChildFrm.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\Utils\Graph\3DGraphDoc.cpp

"$(INTDIR)\3DGraphDoc.obj"	"$(INTDIR)\3DGraphDoc.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\Utils\Graph\GraphChildFrm.cpp

"$(INTDIR)\GraphChildFrm.obj"	"$(INTDIR)\GraphChildFrm.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\Utils\Graph\GraphDoc.cpp

"$(INTDIR)\GraphDoc.obj"	"$(INTDIR)\GraphDoc.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\Utils\Cmdlg.cpp

"$(INTDIR)\Cmdlg.obj"	"$(INTDIR)\Cmdlg.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\Utils\contour.cpp

"$(INTDIR)\contour.obj"	"$(INTDIR)\contour.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\Controls\MeasureCtrls.cpp

"$(INTDIR)\MeasureCtrls.obj"	"$(INTDIR)\MeasureCtrls.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\Utils\middle.cpp

"$(INTDIR)\middle.obj"	"$(INTDIR)\middle.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\Utils\mutils.cpp

"$(INTDIR)\mutils.obj"	"$(INTDIR)\mutils.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\Utils\SpecFileDlg.cpp

"$(INTDIR)\SpecFileDlg.obj"	"$(INTDIR)\SpecFileDlg.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\Utils\StateListBox.cpp

"$(INTDIR)\StateListBox.obj"	"$(INTDIR)\StateListBox.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\Utils\Tracker.cpp

"$(INTDIR)\Tracker.obj"	"$(INTDIR)\Tracker.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\Utils\ZOOMVIEW.CPP

"$(INTDIR)\ZOOMVIEW.OBJ"	"$(INTDIR)\ZOOMVIEW.SBR" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\ImageTempl\BaseImageDoc.cpp

"$(INTDIR)\BaseImageDoc.obj"	"$(INTDIR)\BaseImageDoc.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\ImageTempl\BaseImageView.cpp

"$(INTDIR)\BaseImageView.obj"	"$(INTDIR)\BaseImageView.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\ImageTempl\ImageChildFrm.cpp

"$(INTDIR)\ImageChildFrm.obj"	"$(INTDIR)\ImageChildFrm.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\ImageTempl\ImageDoc.cpp

"$(INTDIR)\ImageDoc.obj"	"$(INTDIR)\ImageDoc.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\ImageTempl\ImageView.cpp

"$(INTDIR)\ImageView.obj"	"$(INTDIR)\ImageView.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\Options\ApproxSetDlg.cpp

"$(INTDIR)\ApproxSetDlg.obj"	"$(INTDIR)\ApproxSetDlg.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\Options\ToolBarsDlg.cpp

"$(INTDIR)\ToolBarsDlg.obj"	"$(INTDIR)\ToolBarsDlg.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\ImageFeatures\SectionFrame.cpp

"$(INTDIR)\SectionFrame.obj"	"$(INTDIR)\SectionFrame.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\ImageFeatures\ZoomDlg.cpp

"$(INTDIR)\ZoomDlg.obj"	"$(INTDIR)\ZoomDlg.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\DigitMode\DigitInfo.cpp

"$(INTDIR)\DigitInfo.obj"	"$(INTDIR)\DigitInfo.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\DigitMode\DotInfo.cpp

"$(INTDIR)\DotInfo.obj"	"$(INTDIR)\DotInfo.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\DigitMode\SectionInfo.cpp

"$(INTDIR)\SectionInfo.obj"	"$(INTDIR)\SectionInfo.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\DigitMode\ZapLineInfo.cpp

"$(INTDIR)\ZapLineInfo.obj"	"$(INTDIR)\ZapLineInfo.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\Digit.cpp

"$(INTDIR)\Digit.obj"	"$(INTDIR)\Digit.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\hlp\Digit.hpj

!IF  "$(CFG)" == "Digit - Win32 Release"

OutDir=.\..\Release
TargetName=Digit
InputPath=.\hlp\Digit.hpj
InputName=Digit
USERDEP__DIGIT="hlp\AfxCore.rtf"	"hlp\$(TargetName).hm"	

"..\Release\Digit.hlp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__DIGIT)
	<<tempfile.bat 
	@echo off 
	start /wait hcw /C /E /M "hlp\$(InputName).hpj" 
	if errorlevel 1 goto :Error 
	if not exist "hlp\$(InputName).hlp" goto :Error 
	copy "hlp\$(InputName).hlp" $(OutDir) 
	goto :done 
	:Error 
	echo hlp\$(InputName).hpj(1) : error: 
	type "hlp\$(InputName).log" 
	:done 
<< 
	

!ELSEIF  "$(CFG)" == "Digit - Win32 Debug"

OutDir=.\..\Debug
TargetName=Digit
InputPath=.\hlp\Digit.hpj
InputName=Digit
USERDEP__DIGIT="hlp\AfxCore.rtf"	"hlp\$(TargetName).hm"	

"..\Debug\Digit.hlp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)" $(USERDEP__DIGIT)
	<<tempfile.bat 
	@echo off 
	start /wait hcw /C /E /M "hlp\$(InputName).hpj" 
	if errorlevel 1 goto :Error 
	if not exist "hlp\$(InputName).hlp" goto :Error 
	copy "hlp\$(InputName).hlp" $(OutDir) 
	goto :done 
	:Error 
	echo hlp\$(InputName).hpj(1) : error: 
	type "hlp\$(InputName).log" 
	:done 
<< 
	

!ENDIF 

SOURCE=.\Digit.rc

"$(INTDIR)\Digit.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "Digit - Win32 Release"

CPP_SWITCHES=/nologo /G6 /MD /W3 /GX /O2 /I "..\..\MGTools" /I "..\..\MGTools\Include" /I "..\InterfSolver" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\Digit.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\StdAfx.sbr"	"$(INTDIR)\Digit.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Digit - Win32 Debug"

CPP_SWITCHES=/nologo /G6 /MDd /W3 /Gm /GX /ZI /Od /I "..\..\MGTools" /I "..\..\MGTools\Include" /I "..\InterfSolver" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\Digit.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\StdAfx.sbr"	"$(INTDIR)\Digit.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\Resource.h

!IF  "$(CFG)" == "Digit - Win32 Release"

TargetName=Digit
InputPath=.\Resource.h

".\hlp\Digit.hm" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
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
<< 
	

!ELSEIF  "$(CFG)" == "Digit - Win32 Debug"

TargetName=Digit
InputPath=.\Resource.h

".\hlp\Digit.hm" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
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
<< 
	

!ENDIF 

SOURCE=.\hlp\Digit.cnt

!IF  "$(CFG)" == "Digit - Win32 Release"

OutDir=.\..\Release
InputPath=.\hlp\Digit.cnt
InputName=Digit

"..\Release\Digit.cnt" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy "hlp\$(InputName).cnt" $(OutDir)
<< 
	

!ELSEIF  "$(CFG)" == "Digit - Win32 Debug"

OutDir=.\..\Debug
InputPath=.\hlp\Digit.cnt
InputName=Digit

"..\Debug\Digit.cnt" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy "hlp\$(InputName).cnt" $(OutDir)
<< 
	

!ENDIF 

!IF  "$(CFG)" == "Digit - Win32 Release"

"EngLangD - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\EngLangD.mak" CFG="EngLangD - Win32 Release" 
   cd "."

"EngLangD - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\EngLangD.mak" CFG="EngLangD - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "Digit - Win32 Debug"

"EngLangD - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\EngLangD.mak" CFG="EngLangD - Win32 Debug" 
   cd "."

"EngLangD - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\EngLangD.mak" CFG="EngLangD - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 

!IF  "$(CFG)" == "Digit - Win32 Release"

"RusLangD - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\RusLangD.mak" CFG="RusLangD - Win32 Release" 
   cd "."

"RusLangD - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\RusLangD.mak" CFG="RusLangD - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "Digit - Win32 Debug"

"RusLangD - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\RusLangD.mak" CFG="RusLangD - Win32 Debug" 
   cd "."

"RusLangD - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\RusLangD.mak" CFG="RusLangD - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 


!ENDIF 

