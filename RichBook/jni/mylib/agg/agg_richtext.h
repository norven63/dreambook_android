#pragma once

#include "agg_brush.h"

BOOL IsEndSepChar(TCHAR c);
BOOL IsFrontSepChar(TCHAR c);
void LimitPtInRect(POINT& pt,const RECT& rc);
int FindNearestRect2Pt(const CRect* pRcs,int nRcCount,CPoint pt);

class TiXmlElement ;
class CPara;

enum ERenderUnitType
{
	rtDefault,		// ���з�default���͵Ľ����Ƚ���������һ��
	rtAlpha,
	rtDigit,
	rtPicture,
};

struct SRange
{
	int nFrom;	// word�ķ�Χ[nFrom,nTo)
	int nTo;	// ��һ��Ӧ��ΪnTo=0,Ȼ��ÿ����һ��ȡһ���µķ�Χ	

	int Count() const
	{
		return nTo-nFrom;
	}

	// ������Χ�ϲ���һ����һ����ʾ
	BOOL Merge(const SRange& range)
	{
		BOOL bHasSpace= ( nTo<range.nFrom ||nFrom>range.nTo );

		if ( !bHasSpace )
		{
			nFrom = min(nFrom,range.nFrom);
			nTo=max(nTo,range.nTo);
			return TRUE;
		}
		return FALSE;
	}
};

struct SLineInfo
{
	SRange lineRange;	//ֻ��lineRange.nTo�����룬����ȫ�����
	int nLineWidth;
	int nLineHeight;
	int nDistTop2Base;		//
	int nDistBottom2Base;	//
	int chAppendChar;		//��β��Ҫ��ӻ��Ƶķ��ţ�������'-'����'��'
};
struct SLineBufData
{
	int yTop;
	int yBaseline;
	float x;
	float fDistAdjust;
	SLineInfo lineInfo;

	BOOL IsLineVisible(const CRect& rcClip)
	{
		return !(yTop>=rcClip.bottom||yTop+lineInfo.nLineHeight<rcClip.top);
	}
	void Offset(int cx,int cy)
	{
		x += cx ;
		yTop += cy;
		yBaseline += cy;
	}
};

int DrawTextInRect(AGGDC& dc,
				   CPara& para,
				   const CRect& rc,
				   DWORD dwOptions,
				   int nFrom,
				   RECT* pRcBox=NULL,
				   CSize* pszMaxAllowed=NULL
				   );
int HitTestInRect(CPoint pt,CPara& para,const CRect& rc,DWORD dwOptions,int nFrom,CSize* pszMaxAllowed=NULL);

CRect GetCaretInRect(int nCaretPos,BOOL bGetPrev,BOOL bUseLineHeight,
					 CPara& para,const CRect& rc,DWORD dwOptions,int nFrom,
					 CSize* pszMaxAllowed=NULL);

// ʹ�õ�ǰ������Ⱦ���֣�������Ⱦ����������
int DrawTextInRect(AGGDC& dc, CString str, 
				   const CRect& rc, 
				   DWORD dwOptions,
				   PFONT hFont,
				   COLORREF clrText, 
				   RECT* pRcBox/* =NULL */ );

int HitTestInRect(CPoint pt,CString str,
				  const CRect& rc,DWORD dwOptions,PFONT hFont);

CRect GetCaretInRect(int nCaretPos,BOOL bGetPrev,BOOL bUseLineHeight,
					 CString str,const CRect& rc,DWORD dwOptions,PFONT hFont);

enum ERenderType
{
	renderChar=0,
	renderPic,
	renderVideo,
	renderLineBreak,		//���з�
	renderPageBreak,		//��ҳ��
	renderBlank,			//��
	renderSplitColumn,		//����
	renderRemoveColumn,		//ȡ������
	renderHorzLine,			//ˮƽ��
	renderAnnotation,		//ע,�������ַ������������ʾ
	renderAnchor,			//��������ҳ�е�anchor
	renderAudio,			//��Ƶ
};

enum ERenUnitStyle
{
	renPreferEnd	= 0x1,
	renPreferFront	= 0x2,
	renForceEnd		= 0x4,
};
DWORD GetCharPreferStyle(TCHAR c);
class CRenderUnit 
{
public:
	// dyamic, not saved
	BOOL mbSelected;		//����ѡ��״̬
	BOOL mbUnderlined;		//�����»���
	BOOL mbDelline;
	BOOL mbBgTransparent;
	COLORREF mclrUnderline;	//�»�����ɫ
	COLORREF mclrDelline;
	COLORREF mclr;
	COLORREF mclrBg;
	int mnUnderlineThickness;
	int mnDelThickness;
	DWORD mPreferStyle;		//ERenUnitStyle
	BOOL mbForEdit;			//��ʾ�Ƿ��ڱ༭ģʽ�£��ڴ�ģʽ�£�ĳЩ�ն�����Ҫ��ʾ����

	// Do not set this field, will call CalcSize
	CSize msz;		
public:
	CRenderUnit()
	{
		mbSelected=FALSE;
		mbUnderlined=FALSE;
		mbDelline = FALSE;
		mclrUnderline=RGB(255,0,0);
		mclrDelline = RGB(255,0,0);
		mclr = RGB(0,0,0);
		mnUnderlineThickness=2;
		mnDelThickness=2;
		mbBgTransparent=TRUE;
		mclrBg=RGB(255,255,255);
		mPreferStyle=0;
		mbForEdit=FALSE;

		msz = CSize(0,0);
	}
	virtual ~CRenderUnit() {}

	void Select(BOOL bSel) { mbSelected=bSel; }
	void Underline(BOOL bUnderline) { mbUnderlined=bUnderline; }
	void Delline(BOOL bDelline) { mbDelline=bDelline; }
	void SetUnderlineClr(COLORREF clrUnderline) { mclrUnderline=clrUnderline; }
	void SetDellineClr(COLORREF clrDelline) { mclrDelline=clrDelline; }
	void SetClr(COLORREF clr) { mclr=clr; }
	void SetUnderLineThickness(int nthick) { mnUnderlineThickness=nthick; }
	void SetDelLineThickness(int nthick) { mnDelThickness=nthick; }
	void SetBgClr(BOOL bgTransparent,COLORREF clrBg) { mbBgTransparent=bgTransparent; mclrBg=clrBg; }

	CSize GetSize() const { return msz; }
	BOOL IsPreferEnd() const { return mPreferStyle&renPreferEnd; }
	BOOL IsPreferFront() const { return mPreferStyle&renPreferFront; }
	BOOL IsForceEnd() const { return mPreferStyle&renForceEnd; }

	// �Ű�ӿ�
	virtual BOOL HasAdorn() const { return TRUE; }
	virtual BOOL IsSpace()	{	return FALSE;	}
	virtual int GetWordType(){	return rtDefault;}

	virtual int GetDistTop2Base() {return msz.cy-GetDistBottom2Base();	}
	virtual int GetDistBottom2Base(){return 0;}
	virtual int GetWidth() {return msz.cx; }

	virtual void CalcSize(const CSize& szMaxAllowed,int nParaWidth,int nLeftWidth,int nLeftHeight) 
	{
		ASSERT(FALSE);		// child must overide
	}
	// ���wc��=0����ʾʹ��ͬ�������û���wc
	virtual void Render(AGGDC& dc,int nPageWidth,int x,int y)	//yָ���ǻ��� 
	{
		ASSERT(FALSE);
	}
};


class CCharObj : public CRenderUnit
{
public:
	WCHAR ch;
	PFONT hFont;

	CCharObj() : ch(0), hFont(NULL) {}

	// �Ű�ӿ�
	virtual BOOL IsSpace() { return xisspace(ch); }
	virtual int GetWordType();

	virtual void CalcSize(const CSize& szMaxAllowed,int nParaWidth,int nLeftWidth,int nLeftHeight) ;
	virtual int GetDistBottom2Base();
	virtual void Render(AGGDC& dc,int nPageWidth,int x,int y);	//yָ���ǻ��� 
};

class CTextObj : public CRenderUnit
{
public:
	CString sText;
	PFONT hFont;

	CTextObj() : hFont(NULL) {}

	// �Ű�ӿ�
	virtual BOOL IsSpace() { return FALSE; }
	virtual int GetWordType() { return rtDefault; }

	virtual void CalcSize(const CSize& szMaxAllowed,int nParaWidth,int nLeftWidth,int nLeftHeight) ;
	virtual int GetDistBottom2Base();
	virtual void Render(AGGDC& dc,int nPageWidth,int x,int y);	//yָ���ǻ��� 
};

class CAnnotationObj : public CCharObj
{
	PFONT mhSmall;
public:
	CAnnotationObj():mhSmall(NULL) {}
	virtual ~CAnnotationObj() { if(mhSmall) DeleteFont(mhSmall); }

	virtual BOOL HasAdorn() const { return FALSE; }
	virtual void Render(AGGDC& dc,int nPageWidth,int x,int y);	//yָ���ǻ��� 
};


class CPicObj : public CRenderUnit
{
public:
	HIMAGE mpImg;		// only valid after PrepareRender
	CSize mszImg;

	CPicObj() : mpImg(NULL),mszImg(0,0) {}

	// �Ű�ӿ�
	virtual BOOL IsSpace() { return FALSE; }
	virtual int GetWordType() { return rtPicture; }

	virtual void CalcSize(const CSize& szMaxAllowed,int nParaWidth,int nLeftWidth,int nLeftHeight) ;
	virtual int GetDistBottom2Base() { return 0; }
	virtual void Render(AGGDC& dc,int nPageWidth,int x,int y);	//yָ���ǻ��� 
};

class CVideoObj : public CPicObj
{
public:

	virtual void Render(AGGDC& dc,int nPageWidth,int x,int y)	//yָ���ǻ��� 
	{
		CPicObj::Render(dc,nPageWidth,x,y);

		const int r=20;
		CPoint ptCenter(x+msz.cx/2,y-msz.cy/2);
		CRect rcDst(ptCenter.x-r,ptCenter.y-r,ptCenter.x+r,ptCenter.y+r);
		CPen pen(PsSolid,3,RGB(255,255,255));
		CBrush* pbr = CBrush::CreateSolidBrush(RGBA(150,150,150,100));
		dc.Ellipse(rcDst,&pen,pbr);
		DeleteBrush(pbr);
	}
};
class CAudioObj : public CPicObj
{
public:

	virtual void Render(AGGDC& dc,int nPageWidth,int x,int y)	//yָ���ǻ��� 
	{
		CPicObj::Render(dc,nPageWidth,x,y);

		const int r=20;
		CPoint ptCenter(x+msz.cx/2,y-msz.cy/2);
		CRect rcDst(ptCenter.x-r,ptCenter.y-r,ptCenter.x+r,ptCenter.y+r);
		CPen pen(PsSolid,3,RGB(255,255,255));
		CBrush* pbr = CBrush::CreateSolidBrush(RGBA(150,150,150,100));
		dc.Ellipse(rcDst,&pen,pbr);
		DeleteBrush(pbr);
	}
};
struct SHorzLine
{
	bool byPercent;
	int mnWidth;
	EPenStyle mnStyle;	
	int mnThick;	//���
	COLORREF mClrLine;

	SHorzLine() : byPercent(true), mnWidth(100),
		mnStyle(PsSolid),mnThick(1) {}
};
class CHorzLineObj : public CRenderUnit
{
public:
	SHorzLine mParam ;

	CHorzLineObj();

	// �Ű�ӿ�
	virtual BOOL HasAdorn() const { return FALSE; }
	virtual void CalcSize(const CSize& szMaxAllowed,int nParaWidth,int nLeftWidth,int nLeftHeight) ;
	virtual void Render(AGGDC& dc,int nPageWidth,int x,int y);	//yָ���ǻ��� 
};



class CLineBreakObj : public CRenderUnit
{
public:
	CLineBreakObj() { mPreferStyle=renForceEnd;}

protected:
	virtual BOOL HasAdorn() const { return FALSE; }
	virtual void CalcSize(const CSize& szMaxAllowed,int nParaWidth,int nLeftWidth,int nLeftHeight) 
	{ 
		msz = CSize(nLeftWidth,0);
	}
	virtual void Render(AGGDC& dc,int nPageWidth,int x,int y)	//yָ���ǻ��� 
	{
	}
};
// �ն���
class CBlankObj : public CRenderUnit
{
public:
	CBlankObj() {}

protected:
	virtual BOOL HasAdorn() const { return FALSE; }
	virtual void CalcSize(const CSize& szMaxAllowed,int nParaWidth,int nLeftWidth,int nLeftHeight) 
	{ 
		msz = CSize(0,0);
	}
	virtual void Render(AGGDC& dc,int nPageWidth,int x,int y)	//yָ���ǻ��� 
	{
	}
};


class CPageBreakObj : public CRenderUnit
{
public:
	CPageBreakObj() {mPreferStyle=renForceEnd;}

protected:
	virtual BOOL HasAdorn() const { return FALSE; }
	virtual void CalcSize(const CSize& szMaxAllowed,int nParaWidth,int nLeftWidth,int nLeftHeight) 
	{ 
		if(mbForEdit)
		{
			msz = CSize(nParaWidth,20) ;
		}else
		{
			msz = CSize(nParaWidth,nLeftHeight) ;
		}
	}
	virtual void Render(AGGDC& dc,int nPageWidth,int x,int y)	//yָ���ǻ��� 
	{
		if (mbForEdit)
		{
			CPen pen(PsDot,1,RGB(150,150,150));
			y -= 1 ;
			dc.Line(CPoint(x,y),CPoint(x+msz.cx,y),&pen);			
		}
	}	
};
enum ECharAttrMask
{
	ECAFontName = 0x01,
	ECAFontSize = 0x02,
	ECABold		= 0x04,
	ECAItalic	= 0x08,
	ECAUnderline= 0x10,
	ECADelLine	= 0x20,
	ECAColor	= 0x40,
	ECADefaultClr = 0x80,
};
class CCharAttr 
{
public:

	CString m_sAttrName;

	LOGFONT m_font;
	COLORREF m_clr;		// ������ɫ
	bool m_bDefaultClr;	// ʹ��ϵͳ������ɫ

	CCharAttr();
	CCharAttr(LPCTSTR szAttrName,const LOGFONT& font,COLORREF clr,bool bSysClr);
	CCharAttr(LPCTSTR szAttrName,LPCTSTR szFontName,int nSize,COLORREF clr,bool bSysClr,BYTE bBold=FALSE,BYTE bItalic=FALSE,
		BYTE bUnderline=FALSE,BYTE bDelLine=FALSE);
	void Reset();

	static CString BuildCharAttrName(const CCharAttr& attr);
	void UpdateName() { m_sAttrName=BuildCharAttrName(*this); }

	// ����true���������������������������ͬ
	bool operator==(const CCharAttr& attr) const;
	bool operator<(const CCharAttr& attr) const;
	bool operator!=(const CCharAttr& attr) const { return !(*this==attr);}

	void Serialize(CArchive& ar);
	BOOL FromXml(TiXmlElement* pFontElem);
	void ToXml(TiXmlElement* pParentElem);
};

class CParaAttr 
{
public:
	CString m_sAttrName ;

	DWORD m_dwAlignMode ;
	int m_nPreLineSpace;
	int m_nPostLineSpace;
	int m_nPreParaSpace;
	int m_nPostParaSpace;

	int m_nFirstLineIdentByChar;		//������������,�����Ǹ�ֵ
	int m_nLeftIdentDist;
	int m_nRightIdentDist;

	BYTE m_bBorder[4];
	int m_nBorderWidth;
	COLORREF m_clrBorder;
	int m_nPenStyle;
	int m_bHasBg;
	COLORREF m_clrBg;

	//CString m_sDefaultFont;
	//CString m_sBullet;	//��Ŀ����,��ǰû��ʹ��


	CParaAttr();
	CParaAttr(LPCTSTR szAttrName,DWORD dwAlignMode,int nPreLineSpace,
		int nPostLineSpace,int nPreParaSpace,int nPostParaSpace,
		int firstident,int leftident,int rightident);

	int GetFirstIdent(PFONT hFont) const
	{
		if( hFont!=NULL )
			return hFont->GetCharWidth()*m_nFirstLineIdentByChar;
		return m_nFirstLineIdentByChar*20;
	}

	bool operator==(const CParaAttr& attr) const;
	bool operator!=(const CParaAttr& attr) const { return !(*this==attr);}
	bool HasBorder() const { return *((int*)m_bBorder)!=0; }

	static CString BuildParaAttrName(const CParaAttr& attr);
	void UpdateName()
	{
		m_sAttrName = BuildParaAttrName(*this);
	}

	BOOL FromXml(TiXmlElement* pFontElem);
	void ToXml(TiXmlElement* pParentElem);

	void Serialize(CArchive& ar);

	void DrawBg(AGGDC& dc,const CRect& rc);

};

struct CParaBlock
{
	CRect rc;
	int nFrom, nTo;
	CPara* pRenderPara;
};
class CPageRects
{
	CRect mrcNewPage;
	CList<CRect> mlRcs;
	CArray<CParaBlock> maBlock;	// ������CalcLayout�󱣴�Ľ��

	BOOL mbSplitColum;			// �Ƿ����ҳ����
	int  mnColumNum;
	int  mnColumDist;			//

	CSize mszMax;				//����С��ȱʡ����ҳ���С

public:
	CPageRects();

	// ��ʼһ����ҳ��ʱ��ʹ�õĴ�С
	void SetDefaultRect(const CRect& rc);
	void StartNewPage();
	void RemoveAllRects();

	void SetMaxSize(CSize szMax) { mszMax=szMax; }
	void ResetMaxSize() { mszMax = mrcNewPage.Size(); }
	CSize GetMaxSize() const { return mszMax; }

	// rect
	BOOL IsFullPage() const;
	BOOL HasRect() const ;
	CRect PopFirstRect() ;
	void PushToFirst(const CRect& rc) ;
	CSize GetPageSize() const { return mrcNewPage.Size(); }
	
	// block
	int GetBlockCount() const ;
	CParaBlock& GetBlock(int nIndex) ;
	void AddParaBlock(CPara* para,int nFrom,int nTo,const CRect& rc);
	void RemoveAllBlock();

	// colum
	BOOL IsColumEnabled() const;
	void EnableColum(BOOL b);
	void SetColum(int nColumNum,int nColumDist);
	void GetColum(int& nColumNum,int& nColumDist)const;
};

class CResMgrBase
{
public:
	virtual ~CResMgrBase() {}
};

class CPara 
{
protected:
	// dynamic, �����ฺ�𴴽�������ֻ�����Ű����Ⱦ
	CArray<CRenderUnit*> maRenderObjs;

	// dynamic, ����
	int		mnBufferFrom;
	CRect	mrcBufferOut;		//����buffer��ʱ�����������
	CArray<SLineBufData> maRenderBuf;
	CRect mrcRenderBox;	//����ľ�������

public:
	CParaAttr mParaAttr;		// CPara not save this field
	PFONT	  mhDefaultFont;	// ȱʡ����,�ⲿ�����ͷ�
	CResMgrBase* mpResMgr ;

	//////////////////////////////////////////////////////////////////////////
	// ��Ⱦ����
	void SaveBuffer(const CRect& rcOut,int nFrom) { mrcBufferOut=rcOut; mnBufferFrom=nFrom;}
	BOOL HasRenderBuf(const CRect& rcOut,int nFrom) 
	{
		if( maRenderBuf.IsEmpty() )
			return FALSE;

		// ȷ��rc��nfromһ��
		if( mnBufferFrom!=nFrom )
		{
			ClearRenderBuf();
			return FALSE;
		}
		if ( rcOut.Width()!=mrcBufferOut.Width() )
		{
			ClearRenderBuf();
			return FALSE;
		}
		int cx = rcOut.left - mrcBufferOut.left ;
		int cy = rcOut.top - mrcBufferOut.top ;
		if( cx!=0 || cy!=0 )
		{
			for (int i=0;i<maRenderBuf.GetCount();i++)
			{
				maRenderBuf[i].Offset(cx,cy);
			}
			mrcRenderBox.OffsetRect(cx,cy);
			mrcBufferOut.OffsetRect(cx,cy);
		}
		return TRUE; 
	}
	void SetRenderBox(const CRect& rcBox) { mrcRenderBox=rcBox; }
	CRect GetRenderBox() const { return mrcRenderBox; }
	int GetRenderBufferCount() const { return maRenderBuf.GetSize(); }
	void GetRenderLineBuf(int nIndex,SLineBufData& lineBuf)
	{
		if( nIndex>=0 && nIndex<maRenderBuf.GetCount() )
		{
			lineBuf=maRenderBuf[nIndex];
		}else
			ASSERT(FALSE);
	}
	void BufferLine(const SLineBufData& lineBuf) { maRenderBuf.Add(lineBuf); }
	void ClearRenderBuf() { maRenderBuf.RemoveAll(); mrcBufferOut.SetRectEmpty(); mnBufferFrom=0; }

public:
	CPara() : mpResMgr(NULL),mhDefaultFont(NULL),mnBufferFrom(0)
	{
		mrcBufferOut=CRect(0,0,0,0); 
	}
	virtual ~CPara()
	{ 
		Close();
	}

	void Close();
	void AddRenderObj(CRenderUnit* pObj) { maRenderObjs.Add(pObj); }
	
	//////////////////////////////////////////////////////////////////////////
	// �Ű���Ⱦ����

	
	// Ϊ�˼򻯴��� 
	int GetWordType(int nIndex) { return maRenderObjs[nIndex]->GetWordType(); }
	int GetRenderWidth(int nIndex) const { return maRenderObjs[nIndex]->msz.cx; }
	int GetRenderHeight(int nIndex) const { return maRenderObjs[nIndex]->msz.cy; }
	BOOL IsPreferEnd(int nIndex) const { return maRenderObjs[nIndex]->IsPreferEnd(); }
	BOOL IsPreferFront(int nIndex) const { return maRenderObjs[nIndex]->IsPreferFront(); }

	void SetParaAttr(const CParaAttr& attr) { mParaAttr=attr; }
	const CParaAttr& GetParaAttr()const{ return mParaAttr;}

	int GetExtraParaSpace(BOOL bPre)
	{
		return bPre?
			mParaAttr.m_nPreParaSpace:
		mParaAttr.m_nPostParaSpace;
	}
	DWORD GetAlignMode()
	{
		return mParaAttr.m_dwAlignMode|DT_WORDBREAK;
	}

	//////////////////////////////////////////////////////////////////////////
	// virtual function
	virtual CRenderUnit* GetRenderAt(int index);
	virtual int GetRenderCount() const ;
	virtual int Render(AGGDC& dc,const CRect& rc,int nFrom,CRect& rcBox,CSize* pszMaxAllowed);
	// ����ֵΪ���һ����Ⱦ�Ķ�������+1
	virtual int ReCalcLayout(const CRect& rc,int nFrom,CRect& rcBox,CSize* pszMaxAllowed) ;
	virtual int ReCalcLayout(CPageRects& pageMgr,int nFrom,BOOL bNotAllowSplit);
	virtual int HitTest(CPoint pt,const CRect& rc,int nFrom,BOOL bForCharIndex);
	virtual CRect GetCaret(int nPos,BOOL bGetPrev,BOOL bUseLineHeight,
		const CRect& rc,int nFrom);
};

class CPageBreakPara : public CPara
{
public:
	CPageBreakPara() 
	{
		AddRenderObj((CRenderUnit*)(new CBlankObj()));
	}

	virtual int Render(AGGDC& dc,const CRect& rc,int nFrom,CRect& rcBox,CSize* pszMaxAllowed)
	{
		rcBox = rc ;
		return 1;
	}

	// ����ֵΪ���һ����Ⱦ�Ķ�������+1
	virtual int ReCalcLayout(const CRect& rc,int nFrom,CRect& rcBox,CSize* pszMaxAllowed) 
	{
		rcBox = rc ;
		return 1;
	}
	virtual int ReCalcLayout(CPageRects& pageMgr,int nFrom,BOOL bNotAllowSplit)
	{
		if( !pageMgr.IsFullPage() )
			pageMgr.RemoveAllRects();
		return 1;
	}
	virtual int HitTest(CPoint pt,const CRect& rc,int nFrom,BOOL bForCharIndex)
	{
		return 0;
	}
	virtual CRect GetCaret(int nPos,BOOL bGetPrev,BOOL bUseLineHeight,
		const CRect& rc,int nFrom)
	{
		return CRect(0,0,0,0);
	}
};
class CSplitColumPara : public CPara
{
public:
	int mnColumNum ;
	int mnColumDist;
	bool mbOnlyThisPage;

	CSplitColumPara() : mnColumNum(1), mnColumDist(10) 
	{
		AddRenderObj((CRenderUnit*)(new CBlankObj()));
	}

public:
	virtual int Render(AGGDC& dc,const CRect& rc,int nFrom,CRect& rcBox,CSize* pszMaxAllowed)
	{
		rcBox = rc ;
		rcBox.bottom=rc.top;
		return 1;
	}

	// ����ֵΪ���һ����Ⱦ�Ķ�������+1
	virtual int ReCalcLayout(const CRect& rc,int nFrom,CRect& rcBox,CSize* pszMaxAllowed) 
	{
		rcBox = rc ;
		rcBox.bottom=rc.top;
		return 1;
	}
	virtual int ReCalcLayout(CPageRects& pageMgr,int nFrom,BOOL bNotAllowSplit)
	{
		if(!pageMgr.HasRect())
			return 0;

		if ( !mbOnlyThisPage )
		{
			pageMgr.EnableColum(TRUE);
			pageMgr.SetColum(mnColumNum,mnColumDist);
		}

		if(mnColumNum<2)
			return 1;

		CRect rc = pageMgr.PopFirstRect();
		int nColumnWidth = (rc.Width()-(mnColumNum-1)*mnColumDist)/mnColumNum;
		CRect rcColum(rc);
		rcColum.left = rcColum.right-nColumnWidth;
		for (int i=0;i<mnColumNum;i++)
		{
			if(i==mnColumNum-1)
				rcColum.left = rc.left;
			pageMgr.PushToFirst(rcColum);
			rcColum.OffsetRect(-nColumnWidth-mnColumDist,0);
		}
		return 1;
	}
	virtual int HitTest(CPoint pt,const CRect& rc,int nFrom,BOOL bForCharIndex)
	{
		return 0;
	}
	virtual CRect GetCaret(int nPos,BOOL bGetPrev,BOOL bUseLineHeight,
		const CRect& rc,int nFrom)
	{
		return CRect(0,0,0,0);
	}
};
class CRemoveColumPara : public CPara
{
public:
	CRemoveColumPara() 
	{
		AddRenderObj((CRenderUnit*)(new CBlankObj()));
	}

	virtual int Render(AGGDC& dc,const CRect& rc,int nFrom,CRect& rcBox,CSize* pszMaxAllowed)
	{
		rcBox = rc ;
		return 1;
	}

	// ����ֵΪ���һ����Ⱦ�Ķ�������+1
	virtual int ReCalcLayout(const CRect& rc,int nFrom,CRect& rcBox,CSize* pszMaxAllowed) 
	{
		rcBox.SetRectEmpty();
		return 1;
	}
	virtual int ReCalcLayout(CPageRects& pageMgr,int nFrom,BOOL bNotAllowSplit)
	{
		pageMgr.EnableColum(FALSE);
		return 1;
	}
	virtual int HitTest(CPoint pt,const CRect& rc,int nFrom,BOOL bForCharIndex)
	{
		return 0;
	}
	virtual CRect GetCaret(int nPos,BOOL bGetPrev,BOOL bUseLineHeight,
		const CRect& rc,int nFrom)
	{
		return CRect(0,0,0,0);
	}
};

class CGroupPara : public CPara
{
public:
	CArray<CPara*> maParas ;
	// buffer
	CArray<CParaBlock> maRects;

	CGroupPara() {}
	virtual ~CGroupPara();

	void CalcStart(int nFrom,int& nFromPara,int& nFromInPara);
	int CalcIndex(int nParaIndex,int nIndexInPara);
	int FindParaIndex(const CPara* para);

	virtual int GetRenderCount() const ;
	virtual CRenderUnit* GetRenderAt(int index);
	virtual int Render(AGGDC& dc,const CRect& rc,int nFrom,CRect& rcBox,CSize* pszMaxAllowed);
	// ����ֵΪ���һ����Ⱦ�Ķ�������+1
	virtual int ReCalcLayout(const CRect& rc,int nFrom,CRect& rcBox,CSize* pszMaxAllowed) ;
	virtual int ReCalcLayout(CPageRects& pageMgr,int nFrom,BOOL bNotAllowSplit);
	virtual int HitTest(CPoint pt,const CRect& rc,int nFrom,BOOL bForCharIndex);
	virtual CRect GetCaret(int nPos,BOOL bGetPrev,BOOL bUseLineHeight,
		const CRect& rc,int nFrom);
};


COLORREF ToColor(CString str);
COLORREF ToColor(CStringA str);
EPenStyle ToPenStyle(CStringA str);
CStringA FromPenStyle(EPenStyle style);