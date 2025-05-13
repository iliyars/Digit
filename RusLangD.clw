; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CDigitApp
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "ruslangd.h"
LastPage=0

ClassCount=3
Class1=CDigitApp
Class2=CAboutDlg

ResourceCount=15
Resource1=IDD_ZOOM_DLG
Resource2=IDD_FILESPECIAL
Resource3=IDR_TEXT_WINDOW
Resource4=IDR_IMAGETYPE
Resource5=IDD_TOOLBARS_DLG
Resource6=IDR_IMAGEINFO
Resource7=IDR_EDITTOOLS
Resource8=IDR_DIGITTOOLS
Resource9=IDR_MAINFRAME
Resource10=IDD_ABOUTBOX
Resource11=IDR_VIEWTOOLS
Resource12=IDD_APPROXSET_DLG
Resource13=IDR_KITTOOL
Resource14=IDR_RULER
Class3=CApproxSetDlg
Resource15=IDR_MAINFRAME (English (U.S.))

[CLS:CDigitApp]
Type=0
BaseClass=CWinApp
HeaderFile=Digit.h
ImplementationFile=Digit.cpp
Filter=N
VirtualFilter=AC
LastObject=CDigitApp

[CLS:CAboutDlg]
Type=0
BaseClass=CDialog
HeaderFile=Digit.cpp
ImplementationFile=Digit.cpp
LastObject=CAboutDlg

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_APPROXSET_DLG]
Type=1
Class=CApproxSetDlg
ControlCount=6
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,static,1342308352
Control4=IDE_approx_pow,edit,1350631552
Control5=IDC_STATIC,static,1342308352
Control6=IDE_approx_dist,edit,1350631552

[MNU:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_FILE_OPEN
Command2=ID_FILE_SAVE_AS
Command3=ID_FILE_MRU_FILE1
Command4=ID_APP_EXIT
Command5=IDD_ADV_PARAMETERS
Command6=IDD_CHANGE_LANG
Command7=IDD_TOOLBARS
Command8=ID_APP_ABOUT
Command9=ID_HELP
CommandCount=9

[MNU:IDR_IMAGETYPE]
Type=1
Class=?
Command1=ID_FILE_OPEN
Command2=ID_FILE_SAVE
Command3=ID_FILE_SAVE_AS
Command4=ID_FILE_MRU_FILE1
Command5=ID_APP_EXIT
Command6=IDD_ZOOM_IN
Command7=IDD_ZOOM_OUT
Command8=IDD_ZOOM_FIT
Command9=IDD_INTERFEROGRAM_VIEW
Command10=IDD_EXTRFRINGE_VIEW
Command11=IDD_SECTIONS_VIEW
Command12=IDD_DOTS_VIEW
Command13=IDD_ADV_PARAMETERS
Command14=IDD_AUTO_D
Command15=IDD_FC_MAX
Command16=IDD_FC_MIN
Command17=IDD_FC_MINMAX
Command18=IDD_CLEAR_D
Command19=IDD_CALC_APROX
Command20=IDD_ADD_DOT_D
Command21=IDD_DEL_DOT_D
Command22=IDD_RENUM_DOT_D
Command23=IDD_RENUM_FR_D
Command24=IDD_NUM_OFF_MINUS
Command25=IDD_NUM_OFF_PLUS
Command26=IDD_DEL_FR_D
Command27=IDD_ADD_SEC_D
Command28=IDD_DEL_SEC_D
Command29=IDD_SEC_LEFT_D
Command30=IDD_SEC_RIGHT_D
Command31=IDD_TOOLBARS
Command32=IDD_FOTO_SECTIONS
Command33=IDD_MEASURE
Command34=IDD_BOUNDS_EXT
Command35=IDD_BOUNDS_INS
Command36=IDD_COPY_SCENARIO
Command37=IDD_PASTE_SCENARIO
Command38=IDD_OPEN_SCENARIO
Command39=IDD_SAVE_SCENARIO
Command40=ID_APP_ABOUT
Command41=ID_HELP
CommandCount=41

[MNU:IDR_TEXT_WINDOW]
Type=1
Class=?
Command1=ID_FILE_PRINT
Command2=ID_FILE_PRINT_SETUP
Command3=ID_FILE_SAVE
Command4=ID_FILE_SAVE_AS
Command5=ID_EDIT_UNDO
Command6=ID_EDIT_CUT
Command7=ID_EDIT_COPY
Command8=ID_EDIT_PASTE
CommandCount=8

[ACL:IDR_MAINFRAME]
Type=1
Class=?
Command1=IDD_COPY
Command2=IDD_CLONE
Command3=ID_FILE_OPEN
Command4=ID_FILE_SAVE
Command5=IDD_UNDO
CommandCount=5

[DLG:IDD_ZOOM_DLG]
Type=1
Class=?
ControlCount=6
Control1=IDOK,button,1342242816
Control2=IDCANCEL,button,1342242816
Control3=IDS_zoom_scale,msctls_trackbar32,1342242836
Control4=IDC_STATIC,static,1342308352
Control5=IDCB_zoom_scale,combobox,1344339971
Control6=IDC_STATIC,static,1342308352

[DLG:IDD_FILESPECIAL]
Type=1
Class=?
ControlCount=3
Control1=IDC_DELETE,button,1073807360
Control2=stc32,button,1073741831
Control3=IDBM_PREOPEN_PICTURE,static,1342308352

[DLG:IDD_TOOLBARS_DLG]
Type=1
Class=?
ControlCount=3
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDCB_Toolbar_list,listbox,1352728849

[DLG:IDR_IMAGEINFO]
Type=1
Class=?
ControlCount=6
Control1=IDC_STATIC,static,1342308352
Control2=IDE_comments_info,edit,1350631552
Control3=IDE_scalefactor,edit,1350631552
Control4=IDE_rotation,edit,1350631552
Control5=IDC_STATIC,static,1342308352
Control6=IDC_STATIC,static,1342308352

[DLG:IDR_RULER]
Type=1
Class=?
ControlCount=13
Control1=IDB_clear_measure,button,1073807360
Control2=IDC_STATIC,static,1342308352
Control3=IDT_X_measure,static,1342308354
Control4=IDC_STATIC,static,1342308352
Control5=IDT_Y_measure,static,1342308354
Control6=IDC_STATIC,static,1342308352
Control7=IDT_W_measure,static,1342308354
Control8=IDC_STATIC,static,1342308352
Control9=IDT_H_measure,static,1342308354
Control10=IDC_STATIC,static,1342308352
Control11=IDT_A_measure,static,1342308354
Control12=IDC_STATIC,static,1342308352
Control13=IDT_D_measure,static,1342308354

[DLG:IDR_DIGITTOOLS]
Type=1
Class=?
ControlCount=3
Control1=IDE_FringeNum,edit,1350631552
Control2=IDE_FringeNumCtrl,msctls_updown32,1342177312
Control3=IDCB_FringeAs,combobox,1344339971

[TB:IDR_KITTOOL]
Type=1
Class=?
Command1=IDD_FOTO_SECTIONS
Command2=IDD_MEASURE
CommandCount=2

[TB:IDR_EDITTOOLS]
Type=1
Class=?
Command1=IDD_ADD_DOT_D
Command2=IDD_DEL_DOT_D
Command3=IDD_DEL_FR_D
Command4=IDD_ADD_SEC_D
Command5=IDD_DEL_SEC_D
Command6=IDD_SEC_LEFT_D
Command7=IDD_SEC_RIGHT_D
Command8=IDD_RENUM_DOT_D
Command9=IDD_RENUM_FR_D
Command10=IDD_NUM_OFF_MINUS
Command11=IDD_NUM_OFF_PLUS
CommandCount=11

[TB:IDR_VIEWTOOLS]
Type=1
Class=?
Command1=IDD_INTERFEROGRAM_VIEW
Command2=IDD_EXTRFRINGE_VIEW
Command3=IDD_SECTIONS_VIEW
Command4=IDD_DOTLINES_VIEW
Command5=IDD_DOTS_VIEW
Command6=IDD_APPROX_VIEW
CommandCount=6

[TB:IDR_DIGITTOOLS]
Type=1
Class=?
Command1=IDD_BOUNDS_EXT
Command2=IDD_BOUNDS_INS
Command3=IDD_AUTO_D
Command4=IDD_FC_MAX
Command5=IDD_FC_MIN
Command6=IDD_FC_MINMAX
Command7=IDD_CLEAR_D
Command8=IDD_CALC_APROX
CommandCount=8

[TB:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=?
Command1=ID_FILE_OPEN
Command2=ID_FILE_SAVE
Command3=ID_FILE_SAVE_AS
Command4=IDD_ZOOM_FIT
Command5=IDD_ZOOM_IN
Command6=IDD_ZOOM_OUT
CommandCount=6

[CLS:CApproxSetDlg]
Type=0
HeaderFile=options\ApproxSetDlg.h
ImplementationFile=options\ApproxSetDlg.cpp
BaseClass=CDialog
Filter=D
LastObject=IDOK
VirtualFilter=dWC

