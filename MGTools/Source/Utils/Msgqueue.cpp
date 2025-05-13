// MsgQueue.cpp: implementation of the CMessageQueue class.
//
//////////////////////////////////////////////////////////////////////

#include "..\..\stdafx.h"

#include "..\..\Include\Utils\MsgQueue.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMessageQueue::CMessageQueue()
{
	StartCounter=0;
	StopKey=VK_ESCAPE;
}

CMessageQueue::~CMessageQueue()
{
	Stop(QUEUE_ALL);
}

void CMessageQueue::SetStopKey(UINT _StopKey)
{
	StopKey=_StopKey;
}

void CMessageQueue::Start()
{
	StartCounter++;
}

void CMessageQueue::Stop(int State)
{
	if(State==QUEUE_SINGLE) StartCounter--;
	else StartCounter=0;

	if(StartCounter<0) StartCounter=0;
}

BOOL CMessageQueue::Check()
{
	MSG msg;
	if(StartCounter<=0) return FALSE;

	while(PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) {
		GetMessage(&msg, NULL, 0, 0);
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		if(msg.message==WM_KEYDOWN || msg.message==WM_SYSKEYDOWN) {
			if(msg.wParam==StopKey) {
				Stop();
				return FALSE;
			}
		}
	}
	return TRUE;
}

BOOL CMessageQueue::CheckAfx()
{
	MSG msg;
	if(StartCounter<=0) return FALSE;

    while(::PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) 
      {
      if (!AfxGetApp()->PumpMessage())
        {
        ::PostQuitMessage(0);
        return FALSE;
        }

   	 if(msg.message==WM_KEYDOWN || msg.message==WM_SYSKEYDOWN) 
       {
	   if(msg.wParam==StopKey) 
         {
		 Stop();
		 return FALSE;
         }
       }
      }
    LONG lIdle = 0;
    while (AfxGetApp()->OnIdle(lIdle++));

    return TRUE;
}
