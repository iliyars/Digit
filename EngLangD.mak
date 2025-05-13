# Microsoft Developer Studio Generated NMAKE File, Based on EngLangD.dsp
!IF "$(CFG)" == ""
CFG=EngLangD - Win32 Debug
!MESSAGE No configuration specified. Defaulting to EngLangD - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "EngLangD - Win32 Release" && "$(CFG)" != "EngLangD - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "EngLangD.mak" CFG="EngLangD - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "EngLangD - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "EngLangD - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "EngLangD - Win32 Release"

OUTDIR=.\..\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\..\Release
# End Custom Macros

ALL : "$(OUTDIR)\EngLangD.dll"


CLEAN :
	-@erase "$(INTDIR)\EngLangD.obj"
	-@erase "$(INTDIR)\EngLangD.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\EngLangD.dll"
	-@erase "$(OUTDIR)\EngLangD.exp"
	-@erase "$(OUTDIR)\EngLangD.lib"

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
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\EngLangD.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\EngLangD.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /dll /incremental:no /pdb:"$(OUTDIR)\EngLangD.pdb" /machine:I386 /def:".\EngLangD.def" /out:"$(OUTDIR)\EngLangD.dll" /implib:"$(OUTDIR)\EngLangD.lib" 
DEF_FILE= \
	".\EngLangD.def"
LINK32_OBJS= \
	"$(INTDIR)\EngLangD.obj" \
	"$(INTDIR)\EngLangD.res"

"$(OUTDIR)\EngLangD.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "EngLangD - Win32 Debug"

OUTDIR=.\..\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\..\Debug
# End Custom Macros

ALL : "$(OUTDIR)\EngLangD.dll"


CLEAN :
	-@erase "$(INTDIR)\EngLangD.obj"
	-@erase "$(INTDIR)\EngLangD.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\EngLangD.dll"
	-@erase "$(OUTDIR)\EngLangD.exp"
	-@erase "$(OUTDIR)\EngLangD.ilk"
	-@erase "$(OUTDIR)\EngLangD.lib"
	-@erase "$(OUTDIR)\EngLangD.pdb"

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
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\EngLangD.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\EngLangD.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /dll /incremental:yes /pdb:"$(OUTDIR)\EngLangD.pdb" /debug /machine:I386 /def:".\EngLangD.def" /out:"$(OUTDIR)\EngLangD.dll" /implib:"$(OUTDIR)\EngLangD.lib" /pdbtype:sept 
DEF_FILE= \
	".\EngLangD.def"
LINK32_OBJS= \
	"$(INTDIR)\EngLangD.obj" \
	"$(INTDIR)\EngLangD.res"

"$(OUTDIR)\EngLangD.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("EngLangD.dep")
!INCLUDE "EngLangD.dep"
!ELSE 
!MESSAGE Warning: cannot find "EngLangD.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "EngLangD - Win32 Release" || "$(CFG)" == "EngLangD - Win32 Debug"
SOURCE=.\EngLangD.cpp

"$(INTDIR)\EngLangD.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\EngLangD.rc

"$(INTDIR)\EngLangD.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

