#pragma once

class CBWMenu;

class CMenuItem
{
public:
	CMenuItem();
	~CMenuItem();

	CSize CalcSize(PFONT pMenuFont);
	void DrawItem(AGGDC* pDC,CRect rcDst,BOOL bSelected);

	CBWMenu* mpParent;
	CBWMenu* mpSubMenu;
	
	DWORD mnID;
	BOOL mbSeprator;
	CString mtitle;
	HIMAGEREF mBmp;

	BOOL mbEnabled;
	BOOL mbChecked;
};

class CBWMenu : public CBWScrollWnd
{
	typedef CBWScrollWnd BaseClass;
	DECLARE_SERIAL(CBWMenu)
public:

	CBWMenu(void);
	virtual ~CBWMenu(void);

	void AppendMenuItem(DWORD dwID,LPCTSTR szTitle,HIMAGEREF hImage=NULL,CBWMenu* pSubMenu=NULL);
	void AppendSeprator();
	void CheckMenuItem(int nItem,BOOL bChecked,BOOL byIndex);

	void TrackPopupMenu(UINT nFlag,int x,int y,CBaseWnd* pWnd);

protected:
	void PopupMenu(CBWMenu* pParentMenu,int x,int y,CBaseWnd* pWnd);
	void CloseMenu();	// 销毁菜单,顶级菜单调用
	void HideMenu();	// 隐藏菜单，次级窗口调用
	void OnSelect(int nNewSelect);
	void OnClick(int nNewSelect);

	virtual CSize CalcSizeToFit(const CSize* pNewSize=NULL,BOOL bUpdateWndRect=TRUE);
	virtual BOOL HandleMouse( UINT uMsg, POINT pt, const CMouse& mouse) ;

	virtual void OnDraw(AGGDC* pDC) ;
	virtual void OnSize(int cx,int cy) ;
	virtual void OnCaptureOut(CBaseWnd* pNewCaptureWnd) ;
	virtual void OnFocusOut(CBaseWnd* pNewFocus) ;

	int HitTest(CPoint pt);
	CRect GetItemRect(int nItem);


	CBWMenu* mpParentMenu;	
	CBWMenu* mpPopedSubMenu;

	std::vector<CMenuItem*> mvItems;
	int	mnCurSel;

	// 仅顶级菜单才有
	CBaseWnd* mpMsgTarget;
	CBaseWnd* mpFocus;
};


