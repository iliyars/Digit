# Microsoft Developer Studio Generated NMAKE File, Based on RusLangD.dsp
!IF "$(CFG)" == ""
CFG=RusLangD - Win32 Debug
!MESSAGE No configuration specified. Defaulting to RusLangD - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "RusLangD - Win32 Release" && "$(CFG)" != "RusLangD - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "RusLangD.mak" CFG="RusLangD - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "RusLangD - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "RusLangD - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "RusLangD - Win32 Release"

OUTDIR=.\..\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\..\Release
# End Custom Macros

ALL : "$(OUTDIR)\RusLangD.dll"


CLEAN :
	-@erase "$(INTDIR)\ApproxSetDlg.obj"
	-@erase "$(INTDIR)\RusLangD.obj"
	-@erase "$(INTDIR)\RusLangD.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\RusLangD.dll"
	-@erase "$(OUTDIR)\RusLangD.exp"
	-@erase "$(OUTDIR)\RusLangD.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\RusLangD.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\RusLangD.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /dll /incremental:no /pdb:"$(OUTDIR)\RusLangD.pdb" /machine:I386 /def:".\RusLangD.def" /out:"$(OUTDIR)\RusLangD.dll" /implib:"$(OUTDIR)\RusLangD.lib" 
DEF_FILE= \
	".\RusLangD.def"
LINK32_OBJS= \
	"$(INTDIR)\ApproxSetDlg.obj" \
	"$(INTDIR)\RusLangD.obj" \
	"$(INTDIR)\RusLangD.res"

"$(OUTDIR)\RusLangD.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "RusLangD - Win32 Debug"

OUTDIR=.\..\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\..\Debug
# End Custom Macros

ALL : "$(OUTDIR)\RusLangD.dll"


CLEAN :
	-@erase "$(INTDIR)\ApproxSetDlg.obj"
	-@erase "$(INTDIR)\RusLangD.obj"
	-@erase "$(INTDIR)\RusLangD.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\RusLangD.dll"
	-@erase "$(OUTDIR)\RusLangD.exp"
	-@erase "$(OUTDIR)\RusLangD.ilk"
	-@erase "$(OUTDIR)\RusLangD.lib"
	-@erase "$(OUTDIR)\RusLangD.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\RusLangD.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\RusLangD.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /dll /incremental:yes /pdb:"$(OUTDIR)\RusLangD.pdb" /debug /machine:I386 /def:".\RusLangD.def" /out:"$(OUTDIR)\RusLangD.dll" /implib:"$(OUTDIR)\RusLangD.lib" /pdbtype:sept 
DEF_FILE= \
	".\RusLangD.def"
LINK32_OBJS= \
	"$(INTDIR)\ApproxSetDlg.obj" \
	"$(INTDIR)\RusLangD.obj" \
	"$(INTDIR)\RusLangD.res"

"$(OUTDIR)\RusLangD.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("RusLangD.dep")
!INCLUDE "RusLangD.dep"
!ELSE 
!MESSAGE Warning: cannot find "RusLangD.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "RusLangD - Win32 Release" || "$(CFG)" == "RusLangD - Win32 Debug"
SOURCE=.\Options\ApproxSetDlg.cpp

"$(INTDIR)\ApproxSetDlg.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\RusLangD.cpp

"$(INTDIR)\RusLangD.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\RusLangD.rc

"$(INTDIR)\RusLangD.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

