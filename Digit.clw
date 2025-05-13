; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=C3DGraphDoc
LastTemplate=CDocument
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "digit.h"
LastPage=0

ClassCount=12
Class1=CDigitApp
Class2=CAboutDlg
Class3=CSectionFrame
Class4=CZoomDlg
Class5=CBaseImageDoc
Class6=CBaseImageView
Class7=CImageChildFrm
Class8=CImageDoc
Class9=CImageView
Class10=CMainFrame

ResourceCount=3
Resource1=IDR_DIGITTYPE (English (U.S.))
Resource2=IDR_MAINFRAME (English (U.S.))
Class11=CCommentsFile
Class12=C3DGraphDoc
Resource3=IDD_ABOUTBOX (English (U.S.))

[CLS:CDigitApp]
Type=0
BaseClass=CWinApp
HeaderFile=Digit.h
ImplementationFile=Digit.cpp

[CLS:CAboutDlg]
Type=0
BaseClass=CDialog
HeaderFile=Digit.cpp
ImplementationFile=Digit.cpp
LastObject=CAboutDlg

[CLS:CSectionFrame]
Type=0
BaseClass=CMDIChildWnd
HeaderFile=ImageFeatures\SectionFrame.h
ImplementationFile=ImageFeatures\SectionFrame.cpp

[CLS:CZoomDlg]
Type=0
BaseClass=CMDialog
HeaderFile=ImageFeatures\ZoomDlg.h
ImplementationFile=ImageFeatures\ZoomDlg.cpp

[CLS:CBaseImageDoc]
Type=0
BaseClass=CDocument
HeaderFile=ImageTempl\BaseImageDoc.h
ImplementationFile=ImageTempl\BaseImageDoc.cpp

[CLS:CBaseImageView]
Type=0
BaseClass=SECZoomView
HeaderFile=ImageTempl\BaseImageView.h
ImplementationFile=ImageTempl\BaseImageView.cpp
LastObject=CBaseImageView

[CLS:CImageChildFrm]
Type=0
BaseClass=CMDIChildWnd
HeaderFile=ImageTempl\ImageChildFrm.h
ImplementationFile=ImageTempl\ImageChildFrm.cpp

[CLS:CImageDoc]
Type=0
BaseClass=CBaseImageDoc
HeaderFile=ImageTempl\ImageDoc.h
ImplementationFile=ImageTempl\ImageDoc.cpp
Filter=N
VirtualFilter=DC
LastObject=CImageDoc

[CLS:CImageView]
Type=0
BaseClass=CBaseImageView
HeaderFile=ImageTempl\ImageView.h
ImplementationFile=ImageTempl\ImageView.cpp
LastObject=CImageView

[CLS:CMainFrame]
Type=0
BaseClass=CMDIFrameWnd
HeaderFile=MainFrame\MainFrm.h
ImplementationFile=MainFrame\MainFrm.cpp
LastObject=CMainFrame
Filter=T
VirtualFilter=fWC

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg

[DLG:IDD_ZOOM_DLG]
Type=1
Class=CZoomDlg

[TB:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_EDIT_CUT
Command5=ID_EDIT_COPY
Command6=ID_EDIT_PASTE
Command7=ID_FILE_PRINT
Command8=ID_APP_ABOUT
Command9=ID_CONTEXT_HELP
CommandCount=9

[MNU:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_MRU_FILE1
Command4=ID_APP_EXIT
Command5=ID_VIEW_TOOLBAR
Command6=ID_VIEW_STATUS_BAR
Command7=ID_HELP_FINDER
Command8=ID_APP_ABOUT
CommandCount=8

[MNU:IDR_DIGITTYPE (English (U.S.))]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_CLOSE
Command4=ID_FILE_SAVE
Command5=ID_FILE_SAVE_AS
Command6=ID_FILE_MRU_FILE1
Command7=ID_APP_EXIT
Command8=ID_EDIT_UNDO
Command9=ID_EDIT_CUT
Command10=ID_EDIT_COPY
Command11=ID_EDIT_PASTE
Command12=ID_VIEW_TOOLBAR
Command13=ID_VIEW_STATUS_BAR
Command14=ID_WINDOW_NEW
Command15=ID_WINDOW_CASCADE
Command16=ID_WINDOW_TILE_HORZ
Command17=ID_WINDOW_ARRANGE
Command18=ID_HELP_FINDER
Command19=ID_APP_ABOUT
CommandCount=19

[ACL:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_EDIT_UNDO
Command5=ID_EDIT_CUT
Command6=ID_EDIT_COPY
Command7=ID_EDIT_PASTE
Command8=ID_EDIT_UNDO
Command9=ID_EDIT_CUT
Command10=ID_EDIT_COPY
Command11=ID_EDIT_PASTE
Command12=ID_NEXT_PANE
Command13=ID_PREV_PANE
Command14=ID_CONTEXT_HELP
Command15=ID_HELP
CommandCount=15

[DLG:IDD_ABOUTBOX (English (U.S.))]
Type=1
Class=CCommentsFile
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[CLS:CCommentsFile]
Type=0
HeaderFile=mainframe\commentsfiledlg.h
ImplementationFile=mainframe\commentsfiledlg.cpp
BaseClass=CDialogBar
Filter=D
VirtualFilter=dWC
LastObject=CCommentsFile

[CLS:C3DGraphDoc]
Type=0
HeaderFile=utils\graph\3DGraphDoc.h
ImplementationFile=utils\graph\3DGraphDoc.cpp
BaseClass=CDocument
Filter=N
LastObject=ID_FILE_OPEN

