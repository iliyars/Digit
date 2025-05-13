////////////////////////////////////////////////////////////////
// CSplash implements a splash window
//

#include "..\..\stdafx.h"
#include "..\..\Include\Utils\Splash.h"

IMPLEMENT_DYNAMIC(CSplashWnd, CWnd)
BEGIN_MESSAGE_MAP(CSplashWnd, CWnd)
  ON_WM_CREATE()
  ON_WM_PAINT()
  ON_WM_TIMER()
END_MESSAGE_MAP()

// WS_EX_TOOLWINDOW prevents the splash from getting a button in the task bar
// WS_EX_TOPMOST corrects problem in NT4 where main window overrides the splash
//
DWORD CSplashWnd::g_dwStyleDefaultEx = WS_EX_TOOLWINDOW|WS_EX_TOPMOST;
DWORD CSplashWnd::g_dwStyleDefault   = WS_POPUP | WS_VISIBLE;

////////////////////////////////////////////////////////////////
// CSplashWnd : the splash window

CSplashWnd::CSplashWnd(CSplash* pThread)
  {
  m_pSplash = pThread;
  }

CSplashWnd::~CSplashWnd()
  {
  }

//////////////////
// Create splash window: load bitmap and create the window
//
BOOL CSplashWnd::Create(UINT nIDRes, UINT duration, WORD flags)
  {
  // Code for ordinary bitmap (assumes m_bitmap is a CBitmap)
  if (!m_bitmap.LoadBitmap(nIDRes))
    return FALSE;
  BITMAP bm;
  m_bitmap.GetBitmap(&bm);
  CSize sz(bm.bmWidth, bm.bmHeight);
  
  m_duration = duration;
  m_flags = flags;
  return CreateEx(g_dwStyleDefaultEx,
                  AfxRegisterWndClass(0, AfxGetApp()->LoadStandardCursor(IDC_ARROW)),
                  NULL,
                  g_dwStyleDefault,
                  0, 0, sz.cx, sz.cy,
                  NULL, // parent wnd
                  NULL);
  }

//////////////////
// Splash window created: center it, move to foreground and set a timer
//
int CSplashWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
  {
  if (CWnd::OnCreate(lpCreateStruct) == -1)
    return -1;
  CenterWindow();
  UpdateWindow();
  SetForegroundWindow();
  if (m_duration!=-1)
    SetTimer(1, m_duration, NULL);
  return 0;
  }

//////////////////
// The window has been destroyed: put main app in foreground
// and post a message to quit this thread.
//
void CSplashWnd::PostNcDestroy()
  {
  CWnd* pMainWnd = AfxGetApp()->m_pMainWnd;
  if (IsWindow(pMainWnd->GetSafeHwnd()))
    ::SetForegroundWindow(pMainWnd->GetSafeHwnd());
  delete this;
  }

//////////////////
// Draw the bitmap.
//
void CSplashWnd::OnPaint()
  {
  CPaintDC dc(this);
  // Code for ordinary bitmap:
  CDC dcImage;
  if (!dcImage.CreateCompatibleDC(&dc))
    return;
  BITMAP bm;
  m_bitmap.GetBitmap(&bm);
  
  // Paint the image.
  CBitmap* pOldBitmap = dcImage.SelectObject(&m_bitmap);
  dc.BitBlt(0, 0, bm.bmWidth, bm.bmHeight, &dcImage, 0, 0, SRCCOPY);
  dcImage.SelectObject(pOldBitmap);
  }

//////////////////
// Timer expired: kill myself--unless the app has
// not created a main window yet.
//
void CSplashWnd::OnTimer(UINT nIDEvent)
  {
  CWnd* pMainWnd = AfxGetApp()->m_pMainWnd;
  if ((m_flags & CSplash::NoWaitForMainWnd) || IsWindow(pMainWnd->GetSafeHwnd())) 
    {
    // have main window: OK to die
    CSplash::Kill(m_pSplash);
    } 
  else
    // no main window: keep splashing
    SetTimer(1,100,NULL);
  }

//////////////////
// Before translating keystroke or mouse: die
//
BOOL CSplashWnd::PreTranslateMessage(MSG* pMsg)
  {
  if (m_flags & CSplash::KillOnClick) 
    {
    UINT msg = pMsg->message;
    if (msg == WM_KEYDOWN ||
      msg == WM_SYSKEYDOWN ||
      msg == WM_LBUTTONDOWN ||
      msg == WM_RBUTTONDOWN ||
      msg == WM_MBUTTONDOWN)
      {
      CSplash::Kill(m_pSplash);
      return TRUE;			  // (eat current message)
      }
    }
  return CWnd::PreTranslateMessage(pMsg);
  }

////////////////////////////////////////////////////////////////
// CSplash, a thread object

IMPLEMENT_DYNAMIC(CSplash, CWinThread)

//////////////////
// Create a new splash thread
//
CSplash::CSplash(UINT nIDRes, UINT duration, WORD flags, CSplash** ppBackPtr)
  {
  m_ppBackPtr = ppBackPtr;
  m_nIDRes = nIDRes;
  m_duration = duration;
  m_flags = flags;
  CreateThread(); // off we go...
  }

/////////////////
// Destruction: Set caller's pointer to NULL, so he knows I'm gone.
//
CSplash::~CSplash()
  {
  }

//////////////////
// Thread initialization.
// Returns TRUE to keep running, otherwise FALSE
// if I determine I'm not supposed to run the splash
//
BOOL CSplash::InitInstance()
  {
  // Check for -nologo switch
  CWinApp* pApp = AfxGetApp();
  ASSERT(pApp);

  // Look for -nologo switch, or any others that MFC thinks should
  // prohibit a splash screen (such as OLE embedding, etc.)
  //
  if (!(m_flags & IgnoreCmdLine)) 
    {
    CCommandLineInfo cmdInfo;
    pApp->ParseCommandLine(cmdInfo);
    if (cmdInfo.m_nShellCommand == CCommandLineInfo::FileOpen ||
        !cmdInfo.m_bShowSplash)
      return FALSE;
    }

  if (!AfxOleGetUserCtrl())  // running without UI: to be safe
    return FALSE;
  
  // Create the splash window
  m_pMainWnd = OnCreateSplashWnd(m_nIDRes, m_duration, m_flags);
  return m_pMainWnd != NULL;
  }

//////////////////
// Create the splash window. This is virtual so you can override to create
// somet other kind of window if you like. 
//
CWnd* CSplash::OnCreateSplashWnd(UINT nIDRes, UINT duration, WORD flags)
  {
  CSplashWnd *pSplashWnd = new CSplashWnd(this);
  if (pSplashWnd)
    pSplashWnd->Create(nIDRes, duration, flags);
  return pSplashWnd;
  }

/////////////////
// Kill the splash screen. Be careful to lock data.
// This is only safe if called from either the code that holds
// the one-and-only CSplash pointer, or CSplashWnd. This code is
// protected in a critical section to synchronize control if both
// the app and the splash window try to kill the splash at (nearly) the
// same time. ***Thanks to reader Eric Roe for pointing out the
// thread-unsafeness of my original implementation in MSJ.***
//
void CSplash::Kill(CSplash* pSplash)
  {
  static CCriticalSection here;
  CLockData lock(here);
  if (pSplash) 
    {
    // splash not already dead: set pointers to NULL
    // in both the app and window.
    CSplashWnd* pSplashWnd = (CSplashWnd*)pSplash->m_pMainWnd;
    ASSERT(pSplashWnd);
    ASSERT_KINDOF(CSplashWnd, pSplashWnd);
    pSplashWnd->m_pSplash=NULL;
    if (pSplash->m_ppBackPtr) 
      {
      *pSplash->m_ppBackPtr = NULL;
      pSplash->m_ppBackPtr = NULL;
      }
    pSplashWnd->PostMessage(WM_CLOSE);
    
    } // else already dead
  }
