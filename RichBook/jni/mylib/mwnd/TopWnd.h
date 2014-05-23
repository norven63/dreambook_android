#ifndef _TOPWND_HEADER_
#define _TOPWND_HEADER_

#include <stack>
#include "BaseWnd.h"

#define CARETTIMER	(-1)

class CTimerEngine : protected CThread
{
	// 定时器
	struct _TIMER
	{
		HWND hWnd ;			//可以直接转换为CBWTimer
		UINT nIDEvent;		//
		DWORD dwInterval;	//ms
		TIMERPROC fnTimer;
		DWORD dwLastHit;	
	};
	DWORD	mdwMsgThreadID;
	BOOL	mbTimerRunning;
	CMutex	mTmMutex;
	std::vector<_TIMER> mvTimer;
	virtual void Run();

public:
	CTimerEngine() { mbTimerRunning=FALSE; }
	~CTimerEngine() { }

	BOOL StartTimerEngine(DWORD dwDstThread);
	void StopTimerEngine();

	UINT AddTimer(HWND hWnd,UINT nIDEvent,DWORD dwIntervalInMs,TIMERPROC pfn);
	void RemoveTimer(HWND hWnd,UINT nIDEvent);
};

class CCaret
{
public:
	UINT mCaretTimer;
	AGGDC mBuffer;
	BOOL bOnScreen;

	CBaseWnd* pWnd;
	CPoint ptPos;
	DWORD dwBlinkTime;
	BOOL bShow;

	HIMAGEREF hImage;
	COLORREF clr;
	int nWidth;
	int nHeight;

	CCaret() 
	{
		mCaretTimer = 0;
		dwBlinkTime=500;
		pWnd=NULL; 
		bShow=FALSE; 
		clr=RGB(0,0,0); 
		hImage=NULL;
	}
};

typedef LRESULT (*PfnAnimateCB)(float fStep,LPVOID lpParam);

class CAnimate
{
	friend class CAnimateMgr;
protected:
	CBaseWnd* mpWnd ;
	DWORD dwAnimateBeginTM;		//动画开始时间
	DWORD dwAnimateDuration;	//动画长度
	DWORD dwLastHit;			//
	BOOL bCanceled;

public:
	CAnimate() : mpWnd(NULL) {
		dwAnimateBeginTM = 0 ;
		dwAnimateDuration= 0 ;
	}
	virtual ~CAnimate() {}

	BOOL IsDone() const { return GetTickCount()-dwAnimateBeginTM>dwAnimateDuration; }

	BOOL IsCanceled() const { return bCanceled; }

	void CancelAnimate() { bCanceled=TRUE; }

	virtual BOOL AnimatePrepare() { return TRUE; }

	// 一半在此函数中执行delete this
	virtual void AnimateUnprepare() {}
	virtual void DoAnimate(float fStep) = 0;
};

class CAnimateMgr : protected CThread
{
	friend class CTopWnd;
protected:

	BOOL  m_bRunning ;
	DWORD m_dwMainThreadID;
	CArray<CAnimate*> maAnimates;	//当前正在执行的动画列表
	CMutex	mmutex;

	virtual void Run();

	CAnimateMgr();
	~CAnimateMgr();

	BOOL StartAnimateLoop(DWORD dwMainThreadID);
	void EndAnimateLoop();
	BOOL HasAnimate()  { CAutoLock lock(mmutex); return !maAnimates.IsEmpty(); }

	void BeginAnimate(CBaseWnd* pWnd,DWORD dwAnimateMs,CAnimate* pAnimate);

	void ExecuteAllAnimate();
};

class CTopWnd:public CBaseWnd, protected CThread
{
	CTimerEngine mTmEngine;
	CMessageQueue mMsgQueue;
	CResManager* mResManager;
	CAnimateMgr mAnimateMgr;
	CString m_curPageName;	
	DECLARE_DYNAMIC(CTopWnd)

	void Run();
	BOOL OnInit() ;
	void OnUninit();
	void OnPaint() ;
	void Render(AGGDC* pDC);
	
	virtual LRESULT WindowProc(HWND hWnd,UINT message, WPARAM wParam, LPARAM lParam);

public:
	CTopWnd() ;	
	~CTopWnd() ;

	void Test();

	//Render Effect
	void SetRenderEffect(RENDEREFFECT effect);
	void PlayEffect(RENDEREFFECT effect);
	BOOL SetActivePage(int nPageID,BOOL saveToHis = TRUE,RENDEREFFECT effect = NONEEFFECT,BOOL transferPageName = TRUE);
	BOOL SetActivePage(CBaseWnd* pPage,BOOL saveToHis = TRUE,RENDEREFFECT effect = NONEEFFECT,BOOL transferPageName = TRUE);

	CString GetCurPageName() ;
	void SetCurPageName(LPCTSTR pageName);/*{m_curPageName = pageName;};*/
	void GoBack(RENDEREFFECT effect = SLIDER_CLOSE) ;

	BOOL LoadFromXml(LPCTSTR szXmlPath);
	BOOL LoadFromMemoryXml(const char* szXmlData,int nLen=-1);

	CMessageQueue* StartMsgLoop();
	void EndMsgLoop();

protected:
	friend class CBaseWnd;
	// Caret
	CCaret	mCaret;
	BOOL _CreateCaret(CBaseWnd* pWnd,HIMAGEREF hImg,COLORREF clr,int nWidth,int nHeight);
	void _DestroyCaret();
	void _SetCaretPos(int x,int y);
	void _ShowCaret(BOOL bShow);
	void _SetCaretBlinkTime(DWORD dwBlinkMs);
	void _DrawCaret(BOOL bDraw);
	void _RestoreCaretBuffer(AGGDC* pDC,CRect rcClip);

	virtual void HandleTimer(UINT nIdEvent);

	void BeginAnimate(CBaseWnd* pWnd,CAnimate* pAnimate,DWORD dwAnimateDuration) ;
};

#endif

