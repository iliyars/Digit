// MsgQueue.h: interface for the CMessageQueue class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MSGQUEUE_H__0C2D4187_8F6F_11D2_9103_F7F130D5B410__INCLUDED_)
#define AFX_MSGQUEUE_H__0C2D4187_8F6F_11D2_9103_F7F130D5B410__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define QUEUE_SINGLE	0
#define QUEUE_ALL		1

class CMessageQueue
{
public:
    bool GetState() { return StartCounter>0;}
	BOOL Check();
    BOOL CheckAfx();
	void Stop(int State=QUEUE_SINGLE);
	void Start();
	void SetStopKey(UINT _StopKey);
	CMessageQueue();
	virtual ~CMessageQueue();

protected:
	UINT StopKey;
	int StartCounter;
};

#endif // !defined(AFX_MSGQUEUE_H__0C2D4187_8F6F_11D2_9103_F7F130D5B410__INCLUDED_)
