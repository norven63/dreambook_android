#pragma once

#include <map>

typedef struct tagMSG 
{
	HWND        hwnd;
	UINT        message;
	WPARAM      wParam;
	LPARAM      lParam;
	DWORD       time;		// 调用SendMessage或者PostMessage的时候的tickcount

	CEvent*		hEvent;
	LRESULT*	pResult;
} MSG, *PMSG, *NPMSG, *LPMSG;

typedef LRESULT (*WNDPROC)(HWND hwnd,
						   UINT uMsg, 
						   WPARAM wParam, 
						   LPARAM lParam 
						   ); 


class CMouse : protected CArray<CPoint>
{
public:
	UINT nMsg ;
	BOOL bPressed;		//按钮是否处于按下
	DWORD dwFlag;		//其他信息

	CMouse() ;
	CMouse(const CMouse& mouse);


	CMouse& operator= (const CMouse& mouse)
	{
		nMsg = mouse.nMsg;
		bPressed = mouse.bPressed;
		dwFlag = mouse.dwFlag;
		Append(mouse);
		return *this;
	}

	unsigned GetPtCount() const
	{ 
		return (unsigned)GetCount();
	}
	CPoint GetPt(int index)
	{
		if( index>=0 && index<GetCount() )
			return GetAt(index);
		ASSERT(FALSE);
		return CPoint(0,0);
	}

	void AddPt(int x,int y)
	{
		Add(CPoint(x,y));
	}
	void Clear() { RemoveAll(); }
};


class CMessageQueue
{
	class CMouseCache
	{
		CList<CMouse*> mlCache;
	public:
		~CMouseCache();
		CMouse* AllocMouse() ;
		void ReleaseMouse(CMouse* pMouse);
	};
private:
	CMutex		m_mutex;
	CEvent		m_queueChangeEvent;	//表明自从上一次访问链表之后是否改动过
	CList<MSG>	m_lMsg;
	DWORD		m_dwThreadID;
	WNDPROC		m_pfnWindowProc;
	CMouseCache m_mouseCache;
	MSG*		m_pCurrentMsg;

	static std::map<DWORD,CMessageQueue*> mThreadMsgQueue;
	static CMutex mThreadMutex;

	void Close();

public:
	CMessageQueue();
	~CMessageQueue(){ Close(); }

	CMouse* AllocMouseMsg() { return m_mouseCache.AllocMouse(); }

	// must be called in the message queue thread
	void RegistMsgQueue(WNDPROC pfn) ;

	void UnRegistMsgQueue();

	BOOL GetMessage(LPMSG lpMsg,HWND hWnd,UINT wMsgFilterMin,UINT wMsgFilterMax);

	BOOL PeekMessage(LPMSG lpMsg,HWND hWnd,UINT wMsgFilterMin,UINT wMsgFilterMax,UINT wRemoveMsg);

	LRESULT SendMessage(HWND hWnd,UINT Msg,WPARAM wParam,LPARAM lParam);

	LRESULT PostMessage(HWND hWnd,UINT Msg,WPARAM wParam,LPARAM lParam);

	// 发送鼠标消息
	LRESULT PostMouseMsg(UINT nMsg,CMouse* pMouse);

	// 重新发送当前正在处理的消息
	LRESULT RePostCurrentMsg();

	BOOL TranslateMessage(const MSG *lpMsg){return FALSE;}

	LRESULT DispatchMessage(MSG *lpMsg);

	static CMessageQueue* GetThreadMsgQueue(DWORD dwThreadID);
};

BOOL XPostMessage(HWND hWnd,UINT Msg,WPARAM wParam,LPARAM lParam);
LRESULT XSendMessage(HWND hWnd,UINT Msg,WPARAM wParam,LPARAM lParam);
BOOL XPeekMessage(LPMSG lpMsg,HWND hWnd,UINT wMsgFilterMin,UINT wMsgFilterMax,UINT wRemoveMsg);
BOOL XGetMessage(LPMSG lpMsg,HWND hWnd,UINT wMsgFilterMin,UINT wMsgFilterMax);
BOOL XTranslateMessage(const MSG *lpMsg);
LRESULT XDispatchMessage(MSG *lpMsg);

// send message to another thread
BOOL XPostThreadMessage(DWORD dwDstThreadID,HWND hWnd,UINT Msg,WPARAM wParam,LPARAM lParam);
LRESULT XSendThreadMessage(DWORD dwDstThreadID,HWND hWnd,UINT Msg,WPARAM wParam,LPARAM lParam);

void XPostQuitMessage(int nExitCode);

class IProgressReceiver
{
public:
	virtual ~IProgressReceiver(){}

	virtual void SetStepCount(int nCount) {}
	virtual void SetCurStep(int nStep,LPCTSTR szStepTitle) {}
	virtual void SetStepProgressRange(int nTotalRange) = 0;
	virtual void SetStepProgressPos(int nPos) = 0;
	virtual void Finish(bool bSuccess) {}
};
