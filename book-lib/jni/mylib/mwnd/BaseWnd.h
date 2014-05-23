/********************************************************************
	file base:	BaseWnd
	file ext:	h
	author:		zhenglibao
	
	purpose:	仿MFC的CWnd，实现整个程序只有一个窗口，提高程序在嵌入式
				设备上的性能。派生类通过重载虚函数来实现消息响应及刷新
				界面。该界面库是线程安全的，在某线程中创建的CBaseWnd派生
				类对象只能在该线程中访问，不能跨线程访问，但多个线程可以
				拥有多个CBaseWnd实例。

*********************************************************************/
#pragma once

#include "BWDef.h"

// get
CTopWnd* BWGetDesktopWnd() ;
BOOL BWIsRectVisible(const CRect& rcInScreen) ;

// mouse / keyboard / timer
void BWHandleMouseMsg(UINT uMsg, POINT pt, const CMouse& mouse) ;
void BWHandleKeyboardMsg(UINT uMsg, WPARAM wParam, LPARAM lParam) ;

//////////////////////////////////////////////////////////////////////////
//

typedef void (* TIMERPROC)(HWND, UINT, UINT, DWORD);

class CBWTimer
{
public:
	UINT SetTimer(UINT nIdEvent,UINT nElapse) ;
	void KillTimer(UINT nIdEvent) ;
	
	virtual void HandleTimer(UINT nIdEvent) {}
};

#define BWALignLeft		0
#define BWAlignCenter	1
#define BWAlignRight	2
#define BWAlignTop		0
#define BWAlignVCenter	4
#define BWAlignBottom	8

class CAnimate;

class CBaseWnd : public CObject, public CBWTimer
{
	DECLARE_SERIAL(CBaseWnd)
public:
	enum 
	{
		topMost , bottomMost ,
	};

	CBaseWnd(void);
	virtual ~CBaseWnd(void);
	void SafeDelete(void);
	void DelayDeleteSelf();	// 延时删除自身

	//////////////////////////////////////////////////////////////////////////
	// get / set
	void SetParent(CBaseWnd* pParent) ;
	CBaseWnd* GetParent()  const ;
	BOOL IsChildOfWnd(CBaseWnd* pParentWnd) ;	

	CBaseWnd* FindWndByPath(CString sWndPath);
	CBaseWnd* FindWndByName(CString sWndName,BOOL bRecursive=FALSE);
	void EnableBeDeleted(BOOL bCanBeDel) { m_bCanBeDeleted=bCanBeDel; }
	BOOL CanBeDeleted() const { return m_bCanBeDeleted; }

	void SetName(LPCTSTR sname) { m_sName=sname; }
	CString GetName() const { return m_sName; }

	CBaseWnd* SetFocus() ;
	void ReleaseFocus() ;
	CBaseWnd* SetCapture() ;
	void ReleaseCapture() ;
	static void ReleaseCapture(CBaseWnd* pCaptureWnd) ;

	BOOL WantAllMsg() const ;
	BOOL WantChildMsg() const;

	// Caret Function
	BOOL CreateCaret(HIMAGEREF hImg,COLORREF clr,int nWidth,int nHeight);
	void DestroyCaret();
	void SetCaretPos(int x,int y);
	void ShowCaret(BOOL bShow);
	void SetCaretBlinkTime(DWORD dwBlinkMs);
	BOOL HasCaret()const;

	// animate
	void BeginAnimate(CAnimate* pAnimate,DWORD dwAnimateDuration) ;

	void AddChild(CBaseWnd* pChild) ;
	void RemoveChild(CBaseWnd* pChild) ;
	void DeleteChild(CBaseWnd* pChild) ;

	void SetEnabled( BOOL bEnabled ) ;
	BOOL GetEnabled()  const ;
	void EnableWindow(BOOL bEnabled);

	void SetVisible( BOOL bVisible ) ;
	BOOL IsVisible() const ;

	void SetWindowText(LPCTSTR lpszText) ;
	void GetWindowText(CString& str) const ;

	void  SetTextColor(	COLORREF Color );
	COLORREF GetTextColor()  const ;
	void SetBgColor(COLORREF color) {m_clrBg=color;Invalidate();}
	COLORREF GetBgColor() const {return m_clrBg; }
	void SetBgTransparent(BOOL bTransparent) { m_bgTransparent=bTransparent; Invalidate(); }
	BOOL IsBgTransparent() const { return m_bgTransparent ;}

	CBaseWnd* GetFirstChildByRunClass(const CRuntimeClass* pRunClass) const ;
	std::vector<CBaseWnd*>&	GetChilds() { return m_vpChilds; }

	void UpdateWindow() ;

	virtual AGGDC* GetAggDC() ;
	virtual void ReleaseAggDC(AGGDC* pDC,RECT* pRcRefresh=NULL,BOOL bBackupCaret=TRUE) ;
	BOOL IsRectVisible(const CRect& rcInClient) const ;// 函数只能在渲染过程中调用，来表明此次rect是否在裁剪区范围内

	// 会重新调用Render,DrawBg, OnDraw
	void Invalidate(const RECT* lpRectRefresh=NULL) ;
	// 与Invalidate区别仅在于是否对具有渲染buffer的造成影响，如果开启了缓冲，
	// 则不会重新调用DrawBg和OnDraw
	void Refresh(const RECT* lpRectRefresh=NULL);

	//////////////////////////////////////////////////////////////////////////

	void ChildToThis(const CBaseWnd* pChild,RECT& rc)const;
	void ThisToChild(const CBaseWnd* pChild,RECT& rc)const;

	void ScreenToClient(POINT& pt) const ;
	void ScreenToClient(RECT& rc) const ;
	void ClientToScreen(POINT& pt) const ;
	void ClientToScreen(RECT& rc) const ;
	
	void ClientToParent(POINT& pt) const  { if(m_pParent) m_pParent->ChildToThis(this,pt); }
	void ClientToParent(RECT& rc) const { if(m_pParent) m_pParent->ChildToThis(this,rc); }
	void ParentToClient(POINT& pt) const { if(m_pParent) m_pParent->ThisToChild(this,pt); }
	void ParentToClient(RECT& rc) const { if(m_pParent) m_pParent->ThisToChild(this,rc); }

	// size, position
	void SetPosition( int x, int y ) ;
	void SetSize( int width, int height ) ;
	CSize GetSize() const ;
	POINT GetPosition() const ;
	BOOL IsMouseInThis() const ;

	void MoveWindow(const RECT* lpRc) ;
	void MoveWindow(const int x,const int y,const int cx,const int cy);
	
	void GetWindowRect(RECT* lpRc) const ;
	void GetVisibleRect(RECT* lpRc) const ;
	void GetClientRect(RECT* lpRc) const ;
	// 通常情况下工作区与client相同，但对于带有Scroll的窗口，将可能比client要大
	virtual void GetWorkRect(RECT* lpRc) const { GetClientRect(lpRc); }

	BOOL DragWindow(CPoint ptFrom) ;
	void ReCenterWindow() ;
	void Aign(DWORD dwAlign) ;
	void SetZOrder(int nZOrder=topMost) ;

	void ResizeChildren();
	void SaveAsInitRect(BOOL bSaveAllChildRen=FALSE);
	void EnableResizeChildren(BOOL bResizeChildren,BOOL bRecursive) ;
	BOOL IsAllowResizeChildren() const { return m_bResizeChildren; }

	//////////////////////////////////////////////////////////////////////////

	int RunModalLoop(DWORD dwFlags = 0 ) ;
	BOOL ContinueModal() ;
	void EndModalLoop(int nResult) ;

	//////////////////////////////////////////////////////////////////////////
	// static

	CBaseWnd* FromPoint(POINT pt) ;

	static CBaseWnd* GetFocus() ;
	static CBaseWnd* GetCapture() ;

	//////////////////////////////////////////////////////////////////////////
	// virtual
	virtual BOOL Create(const RECT* lpRc,CBaseWnd* pParent,BOOL bVisible=TRUE) ;
	virtual BOOL SetParam(CString strKey,const CStringArray& strValue) ;

	virtual void ChildToThis(const CBaseWnd* pChild,POINT& pt)const;
	virtual void ThisToChild(const CBaseWnd* pChild,POINT& pt)const;
	// 返回匹配的最佳大小，不会实际修改窗口的大小
	virtual CSize CalcSizeToFit(const CSize* pNewSize=NULL,BOOL bUpdateWndRect=TRUE);


	virtual void RefreshThisOnly() ;	
	virtual void Render(AGGDC* pDC) ;

	virtual void EnableRenderBuf(BOOL bUseRenderBuf,
		BOOL bHalfTransRender,
		BOOL bBuildRenderNow/*=FALSE*/);
	virtual void BuildRenderBuf();

	// msg
	virtual BOOL HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam ) ;
	virtual BOOL HandleMouse( UINT uMsg, POINT pt, const CMouse& mouse ) ;

	virtual BOOL OnInit() ;
	virtual void OnSize(int cx,int cy) ;
	virtual void OnShow(BOOL bShow) ;
	virtual void OnEnable(BOOL bEnable) ;
	virtual void OnFocusIn() ;
	virtual void OnFocusOut(CBaseWnd* pNewFocus) ;
	virtual void OnCaptureIn() ;
	virtual void OnCaptureOut(CBaseWnd* pNewCaptureWnd) ;
	virtual void OnUpdateData(){TRACE(_T("OnUpdateData() not Implemented!\n"));};

	virtual void ProcessMsg(UINT uMsg,WPARAM wParam, LPARAM lParam) ;
	virtual void ProcessCmd(UINT nCmd,LPARAM lParam);

	BOOL SendParentMsg(UINT uMsg,WPARAM wParam, LPARAM lParam) ;
	void SendMsgToSibling(UINT uMsg,WPARAM wParam, LPARAM lParam) ;
	void SendMsgToSibling(const CRuntimeClass* pRunClass,UINT uMsg,WPARAM wParam, LPARAM lParam) ;

	// 从xml文件加载
	void FromXml(TiXmlElement* pElem);


protected:
	virtual void OnDraw(AGGDC* pDC) ;		// derived class implement this
	virtual void DrawBg(AGGDC* pDC) ;
	

protected:
	CBaseWnd*	m_pParent ;
	std::vector<CBaseWnd*>	m_vpChilds ;
	BOOL m_bWantAllMsg;		//阻止消息向父窗口传递（鼠标消息）
	BOOL m_bWantChildMsg;
	BOOL m_bCanBeDeleted;	//如果建立在栈上，则此值为FALSE，默认为TRUE
	BOOL m_bHasBeenDelayDel;

	int m_x, m_y;
	int m_width, m_height;
	BOOL m_bResizeChildren;		//true：当自身大小发生改变的时候自动修改子窗口的大小进行缩放
	CRect m_rcInit;				//初始化时的区域
	POINTFLT m_TopLefPtResize;	// (0,0)表示不动，(1,1)表示随着右侧变动
	POINTFLT m_BottomRightPtResize;

	CString m_sName;			//控件的名字，可以根据路径查找

	CString m_strWindowText ;
	COLORREF m_clrText ;
	COLORREF m_clrBg ;			//背景色
	BOOL	 m_bgTransparent ;	//背景透明

	BOOL m_bEnabled;           // Enabled/disabled flag
	BOOL m_bVisible;                // Shown/hidden flag
	BOOL m_bDraging;


	BOOL m_bContinueModal ;
	int  m_nModalResult ;

	// 渲染缓冲
	BOOL m_bUseRenderBuf;
	struct _Block
	{
		BOOL bValid;
		CRect rc;
		AGGDC dc;
	};
	struct _RenderBuf
	{
		BOOL bValid;
		BOOL bHalfTransBuf;
		CSize szBlock;		//每个缓存块的大小
		CArray<_Block*> blocks;

		_RenderBuf();
		~_RenderBuf();
		void Close();
		void Draw(AGGDC* pDC);
		void InvalidBuffer(const RECT* pRc);
		_Block* AddBlock(const CRect& rc);
		void Resize(int nwidth,int nheight);
	};
	_RenderBuf* m_pRenderBuf;

	
};


//////////////////////////////////////////////////////////////////////////
//
// Inline Class Member Function
//
inline CBaseWnd* CBaseWnd::GetParent()  const
{
	return m_pParent ;
}
inline void CBaseWnd::EnableWindow(BOOL bEnabled)
{
	if(bEnabled)
		SetEnabled(TRUE);
	else
		SetEnabled(FALSE);
}
inline void CBaseWnd::SetEnabled( BOOL bEnabled ) 
{
	if( bEnabled!=m_bEnabled )
	{		
		m_bEnabled = bEnabled ;
		OnEnable(bEnabled) ;
	}
}
inline BOOL CBaseWnd::GetEnabled()  const 
{
	return m_bEnabled ;
}
inline void CBaseWnd::SetVisible( BOOL bVisible ) 
{
	if( bVisible!=m_bVisible )
	{
		m_bVisible = bVisible ;
		OnShow(m_bVisible) ;
		if( bVisible )
			Invalidate() ;
		else if( m_pParent )
		{
			CRect rc; GetClientRect(&rc);
			m_pParent->ChildToThis(this,rc);
			m_pParent->Invalidate(&rc);
		}
	}
}
inline BOOL CBaseWnd::IsVisible()  const 
{
	const CBaseWnd* pCur = this ;
	while(pCur!=NULL)
	{
		if( !pCur->m_bVisible )
			return FALSE ;
		pCur = pCur->m_pParent ;
	}
	return TRUE ;
}
inline void CBaseWnd::SetPosition( int x, int y ) 
{
	if( x!=m_x || y!=m_y )
	{
		Refresh();
		m_x = x ; m_y = y ;
		Refresh() ;
	}
}
inline void CBaseWnd::SetSize( int width, int height ) 
{
	if( width!=m_width || height!=m_height )
	{
		Refresh();
		m_width = width ;
		m_height= height ;
		OnSize(m_width,m_height) ;
		Invalidate() ;
	}
}
inline void CBaseWnd::MoveWindow(const int x,const int y,const int cx, const int cy) 
{
	RECT rc = { x,y,x+cx,y+cy } ;
	MoveWindow(&rc);
}
inline void CBaseWnd::MoveWindow(const RECT* lpRc) 
{
	ASSERT(lpRc!=NULL) ;
	int newWidth = lpRc->right-lpRc->left ;
	int newHeight= lpRc->bottom-lpRc->top ;
	BOOL bSizeChanged = m_width!=newWidth || m_height!=newHeight;

	Refresh();
	
	m_x = lpRc->left ;
	m_y = lpRc->top ;
	m_width = newWidth;
	m_height= newHeight;

	if(bSizeChanged)
	{
		OnSize(m_width,m_height) ;
		Invalidate();
	}else
		Refresh();
}
inline CSize CBaseWnd::GetSize() const 
{
	return CSize(m_width,m_height) ;
}
inline POINT CBaseWnd::GetPosition() const 
{
	POINT pt ;
	pt.x = m_x; pt.y = m_y ;
	return pt ;
}

inline void  CBaseWnd::SetTextColor(	COLORREF Color )
{
	m_clrText = Color ;
	Invalidate() ;
}
inline COLORREF CBaseWnd::GetTextColor()  const 
{
	return m_clrText ;
}

inline void CBaseWnd::GetWindowRect(RECT* lpRc) const 
{
	CPoint pt(0,0);

	CBaseWnd* pParent = m_pParent ;
	const CBaseWnd* pChild = this;
	while(pParent!=NULL)
	{
		pParent->ChildToThis(pChild,pt);
		pChild = pParent ;
		pParent = pParent->m_pParent ;
	}

	lpRc->left = pt.x ; lpRc->top = pt.y ;
	lpRc->right= pt.x+m_width ; lpRc->bottom = pt.y + m_height ;
}
inline void CBaseWnd::ChildToThis(const CBaseWnd* pChild,RECT& rc)const
{
	ChildToThis(pChild,*((POINT*)&rc.left));
	ChildToThis(pChild,*((POINT*)&rc.right));
}
inline void CBaseWnd::ThisToChild(const CBaseWnd* pChild,RECT& rc)const
{
	ThisToChild(pChild,*((POINT*)&rc.left));
	ThisToChild(pChild,*((POINT*)&rc.right));
}

inline void CBaseWnd::ScreenToClient(POINT& pt) const 
{
	RECT rcWin ;
	GetWindowRect(&rcWin) ;
	pt.x -= rcWin.left ;
	pt.y -= rcWin.top ;
}
inline void CBaseWnd::ScreenToClient(RECT& rc) const 
{
	RECT rcWin ;
	GetWindowRect(&rcWin) ;
	rc.left -= rcWin.left ; rc.right-=rcWin.left ;
	rc.top -= rcWin.top ; rc.bottom -= rcWin.top ;
}
inline void CBaseWnd::ClientToScreen(POINT& pt) const 
{
	RECT rcWin ;
	GetWindowRect(&rcWin) ;
	pt.x += rcWin.left ;
	pt.y += rcWin.top ;

}
inline void CBaseWnd::ClientToScreen(RECT& rc) const 
{
	RECT rcWin ;
	GetWindowRect(&rcWin) ;
	rc.left += rcWin.left ; rc.right+=rcWin.left ;
	rc.top += rcWin.top ; rc.bottom += rcWin.top ;
}
inline void CBaseWnd::GetVisibleRect(RECT* lpRc) const 
{
	if( !m_bVisible )
	{
		*lpRc=CRect(0,0,0,0);
	}else
	{
		CRect rc ;
		GetClientRect(&rc) ;

		CBaseWnd* pParent = m_pParent;
		const CBaseWnd* pChild = this;

		while( pParent!=NULL )
		{
			if( !pParent->m_bVisible )
			{
				rc.SetRectEmpty();
				break;
			}
			CRect rcParent;
			pParent->ChildToThis(pChild,rc);
			pParent->GetClientRect(&rcParent);
			rc.IntersectRect(&rc,&rcParent);
			pChild = pParent;
			pParent = pParent->m_pParent;
		}
		*lpRc = rc ;
	}
}
inline void CBaseWnd::Invalidate(const RECT* lpRectRefresh/*=NULL*/) 
{
	if( m_bVisible )
	{
		CRect rcWork ;
		GetWorkRect(&rcWork) ;
		if( lpRectRefresh!=NULL )
		{
			if(!rcWork.IntersectRect(&rcWork,lpRectRefresh))
				return ;
		}

		if ( m_pRenderBuf!=NULL )
		{
			m_pRenderBuf->InvalidBuffer(&rcWork);
		}

		ChildToThis(NULL,rcWork);

		CRect rc;
		GetClientRect(&rc);
		if(!rc.IntersectRect(&rc,&rcWork))
			return ;

		if ( m_pParent!=NULL )
		{
			m_pParent->ChildToThis(this,rc);
			m_pParent->Invalidate(&rc);
		}else
		{
			GetScreen().Invalidate(rc);	//记录无效区域
		}
	}
}
//区别于Invalidate避免渲染缓冲失效
inline void CBaseWnd::Refresh(const RECT* lpRectRefresh/* =NULL */)
{
	if( m_bVisible )
	{
		CRect rc;
		GetClientRect(&rc);
		if ( lpRectRefresh )
		{
			CRect rcTemp(*lpRectRefresh);
			ChildToThis(NULL,rcTemp);
			if(!rc.IntersectRect(&rc,&rcTemp))
				return;
		}

		if ( m_pParent!=NULL )
		{
			m_pParent->ChildToThis(this,rc);
			m_pParent->Invalidate(&rc);
		}else
		{
			GetScreen().Invalidate(rc);	//记录无效区域
		}
	}
}

inline void CBaseWnd::UpdateWindow() 
{
	MSG msg;
	if(XPeekMessage(&msg,NULL,0,0,PM_REMOVE))
	{
		XDispatchMessage(&msg);
	}
}
inline BOOL CBaseWnd::SendParentMsg(UINT uMsg,WPARAM wParam, LPARAM lParam) 
{
	if (m_pParent)
	{
		m_pParent->ProcessMsg(uMsg,wParam,lParam) ;
		return TRUE ;
	}
	TRACE(_T("Parent is NULL\r\n")) ;
	return FALSE ;
}
inline void CBaseWnd::SendMsgToSibling(UINT uMsg,WPARAM wParam, LPARAM lParam) 
{
	if( m_pParent==NULL )
		return ;

	std::vector<CBaseWnd*>& vChilds = m_pParent->m_vpChilds ;
	for (int i=0;i<(int)vChilds.size();i++)
	{
		if( vChilds[i]!=this )
		{
			vChilds[i]->ProcessMsg(uMsg,wParam,lParam) ;
		}
	}
}

inline void CBaseWnd::SendMsgToSibling(const CRuntimeClass* pRunClass,UINT uMsg,WPARAM wParam, LPARAM lParam) 
{
	if( m_pParent==NULL )
		return ;

	std::vector<CBaseWnd*>& vChilds = m_pParent->m_vpChilds ;
	for (int i=0;i<(int)vChilds.size();i++)
	{
		if( vChilds[i]!=this && vChilds[i]->IsKindOf(pRunClass) )
		{
			vChilds[i]->ProcessMsg(uMsg,wParam,lParam) ;
		}
	}
}
inline void CBaseWnd::GetClientRect(RECT* lpRc) const 
{
	lpRc->left = lpRc->top = 0 ;
	lpRc->right = m_width ;
	lpRc->bottom = m_height ;
}
inline void CBaseWnd::SetWindowText(LPCTSTR lpszText) 
{
	m_strWindowText = lpszText ;
	Invalidate() ;
}
inline void CBaseWnd::GetWindowText(CString& str) const
{
	str = m_strWindowText ;
}
inline BOOL CBaseWnd::WantAllMsg() const 
{
	return m_bWantAllMsg;
}
inline BOOL CBaseWnd::WantChildMsg() const
{
	return m_bWantChildMsg;
}


// 函数只能在渲染过程中调用，来表明此次rect是否在裁剪区范围内
inline BOOL CBaseWnd::IsRectVisible(const CRect& rcInClient) const 
{
	CRect rcTemp, rcScreen(rcInClient) ;
	ClientToScreen(rcScreen) ;
    rcTemp = GetScreen().GetClipRect();
	return rcTemp.IntersectRect(&rcScreen,&rcTemp)!=0 ;
}
//
// Inline End
//
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
//
// Inline Class Member Function
//
BOOL ExGradientFill(AGGDC& aggBuf,const RECT& rc, COLORREF clrFrom, COLORREF clrTo,BOOL bHorz) ;
void ClrFromStrArray(const CStringArray& sa,COLORREF& clr) ;
void RectFromStr(const CString& str,RECT& rc,LPCTSTR lpszSepChar=_T(";")) ;
void RectFFromStr(const CString& str,RECTF& rc,LPCTSTR lpszSepChar=_T(";")) ;
void RectFromStrArray(const CStringArray& sa,RECT& rc) ;
void RectFFromStrArray(const CStringArray& sa,RECTF& rcf) ;


//////////////////////////////////////////////////////////////////////////

#define IsClass(T,x)	\
	( strcmp(typeid(T).name(),typeid(x).name())==0 )
