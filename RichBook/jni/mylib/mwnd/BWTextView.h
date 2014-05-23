#pragma once

#include "BWScrollWnd.h"
#include "BWRichTextDoc.h"

class CBWTextView :	public CBWScrollWnd
{
	typedef CBWScrollWnd BaseClass;
	DECLARE_SERIAL(CBWTextView)
public:

	CBWTextView(void);
	virtual ~CBWTextView(void);

	void SetText(LPCTSTR lpszText);
	void AddText(LPCTSTR lpszText);
	CString GetText() const;
	void UpdateCaret();

	virtual BOOL SetParam(CString strKey,const CStringArray& strValue) ;
	virtual void OnShow(BOOL bShow) ;

	CRenderDoc& GetDoc() { return mDoc; }
protected:
	virtual BOOL OnInit();
	virtual BOOL HandleMouse( UINT uMsg, POINT pt, const CMouse& mouse) ;
	virtual BOOL HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam ) ;

	virtual void DrawBg(AGGDC* pDC) ;
	virtual void OnDraw(AGGDC* pDC) ;
	virtual void OnSize(int cx,int cy) ;
	virtual void OnFocusIn();

	CRenderDoc mDoc;	
	int mCaretHeight ;
	BOOL mbButtonDown;
	BOOL mbMouseMoved;
	BOOL mbShiftDown;
};


