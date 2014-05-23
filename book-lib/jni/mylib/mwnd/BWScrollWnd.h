#pragma once


class CBWScrollWnd : public CBaseWnd
{
	DECLARE_SERIAL(CBWScrollWnd)
	typedef CBaseWnd BaseClass;

public:
	CBWScrollWnd(void);
	virtual ~CBWScrollWnd(void);

	void   SetScrollSize(SIZE sizeTotal);
	CSize  GetTotalSize( ) const;

	void   ScrollToPosition(POINT pt );
	CPoint GetScrollPosition( ) const { return m_curPos; }

	void MakeVisible(CPoint pt);

	CRect GetVisibleRect() const ;
	BOOL IsPtVisible(CPoint pt) const ;

	void  ScrollPage(BOOL bVert,BOOL bDown) ;

protected:
	void LimitScrollPos();
	void DrawScrollBar(AGGDC* pDC);
	void HandleTimer(UINT nIdEvent);

	// 重载基类函数
	void BuildRenderBuf();
	virtual AGGDC* GetAggDC() ;
	virtual void Render(AGGDC* pDC) ;
	virtual BOOL HandleMouse( UINT uMsg, POINT pt, const CMouse& mouse) ;
	
	virtual void ChildToThis(const CBaseWnd* pChild,POINT& pt)const;
	virtual void ThisToChild(const CBaseWnd* pChild,POINT& pt)const;
	virtual void GetWorkRect(RECT* lpRc) const ;

	virtual void OnSize(int cx,int cy) ;
	void UpdateRenderBlockSize();


	// scroll variable
	CSize m_szTotal;
	CPoint m_curPos;
	CSize  m_szRenderBlock;

	CPoint	m_posOnBtnDown;
	CPoint m_ptDragFrom;
	CPoint m_ptLast;
	DWORD  m_dwLast;
	BOOL	m_bBtnDown;
	BOOL	m_bScrolling;
	SIZEFLT m_szSpeed;
	SIZEFLT m_szAccel;

	struct _Mouse
	{
		CPoint pt;
		DWORD dwTime;
	};
	CList<_Mouse> m_lMouse;
	SIZEFLT CalcSpeed(CPoint ptNew);


	static CSize szRenderBlockSize;
};

inline CSize  CBWScrollWnd::GetTotalSize( ) const
{
	return m_szTotal;
}
