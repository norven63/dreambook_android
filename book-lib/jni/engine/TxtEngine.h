#pragma once


class CBookEngine
{
public:
	virtual BOOL LoadDoc(LPCTSTR szDocFile){return FALSE;}
	virtual void SetPageSize(int cx,int cy){}

	virtual int GetPageCount(){return 0;}
    
    virtual BOOL IsFirstPage() { return FALSE; }
    virtual BOOL IsLastPage() { return FALSE; }
    
	virtual BOOL GotoPage(int nPage){return FALSE;}
	virtual BOOL Goto(float fPercent){return FALSE;}
	virtual BOOL GotoNextPage(){return FALSE;}
	virtual BOOL GotoLastPage(){return FALSE;}

	virtual void Render(AGGDC& dc){}
};

class CTxtEngine : public CBookEngine
{
	// 用于表示段落、行的开始或结束
	struct _SRange
	{
		int nStart;
		int nEnd;

		BOOL IsInRange(int nIndex) const { return nIndex>=nStart && nIndex<nEnd; }
		BOOL IsEmpty() const { return nStart>=nEnd; }
	};

	CString m_sFile;
	CString m_sContent;
	int m_nPageStart;
	CRect m_rcContent;	//文字渲染的区域
	CRect m_rcPage;		//整个页面的区域
	PFONT m_hFont;		//文字字体
	PFONT m_hOtherFont;	//渲染页码等其他属性时的字体
	int m_nFontSize;
	int m_nLineSpace;	// default: 0
	CArray<_SRange> m_aUnderline;

	_SRange m_sel;

	// 鼠标拖动
	int m_nDragCaret ;

	//////////////////////////////////////////////////////////////////////////
	//
	// 用于内部枚举行信息
	_SRange m_curPara;				//当前段落
	CArray<_SRange> m_vLineInfo;	//当前段落的航信息
	int m_nCurLine;					//当前行
	BOOL InitCurLine(int nPos,int& nParaStart,_SRange* line);
	// 如果已经到了第一行，返回false
	BOOL NextLine(int& nParaStart,_SRange& line);
	// 如果已经到了最后一行，返回false
	BOOL PrevLine(int& nParaStart,_SRange& line);

	//////////////////////////////////////////////////////////////////////////
	//
	int GetLinesPerPage();
	int FindMatchedPageStart(int nCurPageStart);
	void CalcParaLayout(const _SRange& para,CArray<_SRange>& lines);
	static BOOL FindParaRange(const CString& s,int nIndexInPara,_SRange& para);

	int HitTest(CPoint pt);
	CRect GetCaretRect(int nCaretIndex,BOOL bHead);

	BOOL IsUnderlined(int nIndex); //字符索引

	void UpdateFont();
public:
	CTxtEngine();

	BOOL LoadDoc(LPCTSTR szDocFile);
	void SetPageSize(int cx,int cy);

	int GetFontSize() const { return m_nFontSize; }
	void SetFontSize(int nNewSize)
	{ 
		if( nNewSize!=m_nFontSize && nNewSize>0
			&& nNewSize<=80 )
		{
			m_nFontSize=nNewSize; 
			UpdateFont(); 
		}
	}
    
    BOOL IsFirstPage() { return m_nPageStart<=0; }
    BOOL IsLastPage() ;

	int GetPageCount();
	BOOL GotoPage(int nPage);
	BOOL Goto(float fPercent);
    float GetPercent() const ;
	BOOL GotoNextPage();
	BOOL GotoLastPage();

	void Render(AGGDC& dc);

	void SetContentRect(CRect rc) { m_rcContent=rc; }
	CRect GetContentRect() const { return m_rcContent; }

	BOOL IsSelEmpty() const { return m_sel.IsEmpty(); }
	CString GetSelString() const ;
	BOOL UnderlineSel();

	void SetSel(CPoint ptFrom,CPoint ptTo);
	void SelSentence(CPoint pt);
    void RemoveSel() { m_sel.nStart=m_sel.nEnd=0; }

	BOOL OnTouchStart(CPoint pt);
	void OnTouchMove(CPoint pt);
	
    CRect GetSelRect();
};

