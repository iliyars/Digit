#if !defined(AFX_SAWANA_DEFS_H__558E5844_389D_11D4_8A51_83C94F0AD91B__INCLUDED_)
#define AFX_SAWANA_DEFS_H__558E5844_389D_11D4_8A51_83C94F0AD91B__INCLUDED_

#include "..\..\stdafx.h"

#define PALETTEVIEW_WND (WM_USER+10)
#define DESKTOPVIEW_WND (WM_USER+11)

#define IDRM_DelConnec  (WM_USER+20)
#define IDRM_DelElem    (WM_USER+21)
#define IDRM_Properties (WM_USER+22)
#define IDRM_Rlc        (WM_USER+23)
#define IDRM_Admit      (WM_USER+24)
#define IDRM_Ampl       (WM_USER+25)
#define IDRM_Unidir     (WM_USER+26)
#define IDRM_Apodiz     (WM_USER+27)
#define IDRM_Copy       (WM_USER+28)
#define IDRM_Paste      (WM_USER+29)

#define PLOT_ATF_WND (WM_USER+50)
#define PLOT_INPUT_ADMITTANCE_WND (WM_USER+51)
#define PLOT_OUTPUT_ADMITTANCE_WND (WM_USER+52)
#define BAND_PASS_WND (WM_USER+53)
#define PLOT_TR_ATF_WND (WM_USER+54)
#define PLOT_TR_ADMITTANCE_WND (WM_USER+55)
#define PLOT_TR_UNIDIR_WND (WM_USER+56)
#define PLOT_TR_APODIZ_WND (WM_USER+57)

const int I_EMPTY = -100000;
const double D_EMPTY = -100000.;

const float min_level = 0.00005f;

#endif // !defined(AFX_SAWANA_DEFS_H__558E5844_389D_11D4_8A51_83C94F0AD91B__INCLUDED_)
