#pragma once

class CBWEdit :	public CBaseWnd
{
	typedef CBaseWnd BaseClass;
	DECLARE_SERIAL(CBWEdit)
public:

	CBWEdit(void);
	virtual ~CBWEdit(void);

	void SetText(LPCTSTR lpszText);
	void AddText(LPCTSTR lpszText);
	CString GetText() const;
	void DeleteChar(BOOL bDelPrev);
	void InsertChar(WCHAR c);
	void UpdateCaret();

	virtual BOOL SetParam(CString strKey,const CStringArray& strValue) ;
	virtual void OnShow(BOOL bShow) ;
protected:
	virtual BOOL OnInit();
	virtual BOOL HandleMouse( UINT uMsg, POINT pt, const CMouse& mouse) ;
	virtual BOOL HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam ) ;

	virtual void DrawBg(AGGDC* pDC) ;
	virtual void OnDraw(AGGDC* pDC) ;
	virtual void OnSize(int cx,int cy) ;
	virtual void OnFocusIn();

	HFONTREF GetFont() const ;
	int HitTest(CPoint pt) ;


	COLORREF	m_clrActiveBg, m_clrInActiveBg, 
		m_clrActiveBorder, m_clrInActiveBorder, 
		m_clrFont ;
	HFONTREF	m_font;

	CString		m_strText;
	int		m_nCaretIndex;
	int		m_nFirstVisibleIndex;
};


