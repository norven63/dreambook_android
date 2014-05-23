#pragma once

#include "BWRichTextDoc.h"

class CBWLabel : public CBaseWnd
{
	typedef CBaseWnd BaseClass;
	DECLARE_SERIAL(CBWLabel)
public:

	CBWLabel(void);
	virtual ~CBWLabel(void);

	void SetText(LPCTSTR lpszText,COLORREF clrText);
	void SetFormatText(LPCTSTR szText);
	CString GetText() const { return mDoc.GetAllText(); }

	virtual CSize CalcSizeToFit(const CSize* pNewSize=NULL,BOOL bUpdateWndRect=TRUE);

protected:
	virtual BOOL HandleMouse( UINT uMsg, POINT pt, const CMouse& mouse) ;

	virtual void OnDraw(AGGDC* pDC) ;
	virtual void OnSize(int cx,int cy) ;


	CRenderDoc	mDoc;
};


