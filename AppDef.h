#if !defined(AFX_APPDEF_H__D05A4DA0_5396_4A71_A4F4_BBA7D5907C2F__INCLUDED_)
#define AFX_APPDEF_H__D05A4DA0_5396_4A71_A4F4_BBA7D5907C2F__INCLUDED_

#define ENG_LANG 0
#define RUS_LANG 1

const COLORREF InvColor = RGB(153,255,204);
const COLORREF DefBkColor = RGB(65,138,179);

#define E_ADD_DOT        0
#define E_DELETE_DOT     1
#define E_DELETE_FRINGE  2
#define E_ADD_SECTION    3
#define E_DELETE_SECTION 4
#define E_SECTION_LEFT   5
#define E_SECTION_RIGHT  6
#define E_RENUM_DOT      7
#define E_RENUM_FRINGE   8

#define O_NO_UNDO             -1
#define O_BRITGHTNESS_CONTRAST 0
#define O_ROTATION             1
#define O_GAUSS                2
#define O_INCLINE_CORRECT      3
#define O_ZOOM                 4
#define O_GAMMA                5
#define O_RATIO_SIDE           6
#define O_CROP                 7
#define O_EXPAND               8
#define O_ROTATION_90          9
#define O_ROTATION_A90         10
#define O_FLIP_H               11
#define O_FLIP_V               12
#define O_INVERT               13
#define O_IMAGE_SCENARIO       14

#define I_FOTO_SECTIONS    0x01
#define I_MEASURELINE      0x02
#define I_MEASURE_ACTIVE   0x04
#define I_MEASURE_DRAW     0x08
#define I_BOUNDS_EXT       0x10
#define I_BOUNDS_INS       0x20

#define V_INTERFEROGRAM    0x01
#define V_EXTREMUMS        0x02
#define V_ZAPSECTIONS      0x04
#define V_DOTLINES         0x08
#define V_DOTS             0x10
#define V_APPROXIMATION    0x20

#define T_EMP 0
#define T_BMP 1
#define T_PCX 2
#define T_JPG 3
#define T_GIF 4
#define T_TGA 5
#define T_TIF 6

#define T_PIC 7
#define T_FRN 8
#define T_ZAP 9
#define T_POL 10
#define T_MTR 11
#define T_DAT 12
#define T_AVE 13
#define T_DSP 14

#define BOUND_NONE   -1
#define BOUND_ROUND   0
#define BOUND_ELLIPSE 1
#define BOUND_RECT    2
#define BOUND_POLYGON 3

#define FC_MAX    0
#define FC_MIN    1
#define FC_MINMAX 2

#define S_INTERNAL 0
#define S_EXTERNAL 1

#define NUMB_NONE    -1
#define NUMB_AUTO     0
#define NUMB_FRINGE   1
#define NUMB_SECTION  2

#define ACT_NONE         -1
#define ACT_ADD_DOT       0
#define ACT_DEL_DOT       1
#define ACT_DEL_FRINGE    2
#define ACT_ALIGN_SEC     3
#define ACT_RENUM_DOT     4
#define ACT_RENUM_FRINGE  5
#define ACT_ADD_SEC       6
#define ACT_DEL_SEC       7

#define K_EXT_BOUND           0x01
#define K_INT_BOUND           0x02
#define K_IMAGE               0x04
#define K_FRINGE_CENTER       0x08
#define K_FRINGE_DOTS         0x10
#define K_FRINGE_LINE_DOTS    0x20
#define K_SEC_ALIGN_LEFT      0x40
#define K_SEC_ALIGN_RIGHT     0x80
#define K_SECTIONS            0x100


#define MEASURE_TRACKER    0
#define MEASURE_DOTS       1

#define P_GLOBAL   0
#define P_LOCAL    1
#define P_SECTION  2

#define S_NONE    -1
#define S_PLANE  0
#define S_SPHERE   1

#define U_MICRON     0
#define U_WAVELENGHT 1

#define Z_POWER      0
#define Z_ZERNIKE    1

#define FRINGE_MAX     0
#define FRINGE_MIN_MAX 1

#define A_COMA         0x01
#define A_ASTIGMATISM  0x02
#define A_CLAMPCOMA    0x04
#define A_ZONERROR     0x08

#define O_PUPIL     0x01
#define O_SCREEN    0x02
#define O_3G        0x04

const int defMarkerSide = 11;
const int defDotSide = 7;

#endif // !defined(AFX_APPDEF_H__D05A4DA0_5396_4A71_A4F4_BBA7D5907C2F__INCLUDED_)
