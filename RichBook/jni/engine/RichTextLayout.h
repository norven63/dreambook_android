#pragma once


 //////////////////////////////////////////////////////////////////////////

#define MAX_FONT_LEVEL 3
#define MIN_FONT_LEVEL -3


class CFontCacheManager;
class CPageRender;
class CPage;
class CBookWriter;
class CPostion;
class CRBClickObserver;
class CResMapping;
class CRichBook;
struct SResFile;

enum EResType
{
	resBook=0,
	resImage,
	resVideo,
	resFont,
	resPageLayout,
	resAudio,
};



enum EBookThemeStyle
{
	bookThemeWhite=0,
	bookThemeBlack,
	bookThemeYellow,
	bookThemeCyan,
};

enum EBookDragMode
{
	dragSel,
	dragComment,
};

struct SBookTheme
{
	CString sBgPattern;
	CString sBgHorzCenterLine;
	COLORREF clrBg;
	COLORREF clrText;
};


class CBookThemeMgr
{
	HIMAGEREF mhImgBg ;
	HIMAGEREF mhHorzCenterLine;
	HIMAGEREF mhSelHead;
	CBrush* mhSelHeadBrush;

	SBookTheme mTheme;
	CString msResDir ;

	CBookThemeMgr();
public:
	EBookThemeStyle mStyle;
	CSize	mszScreen;
	CRect	mrcBookMargin;
	unsigned mnTitleFontSize;

	~CBookThemeMgr();
	void Close();

	void SetTheme(const SBookTheme& theme);
	void GetTheme(SBookTheme& theme);
	void SetResourceDir(CString sDir);
	void DrawBg(AGGDC& dc,const CRect& rcBg,BOOL bVert,int nTop,int nBottom);
	void DrawSelIndicator(AGGDC& dc,CRect rc,BOOL bFirst);

	void SetBookScreenSzInDp(CSize sz);
	void SetMarginInMM(int l,int t,int r,int b);
	void SetTitleFontSizeInLP(int nFontSize);
  

	COLORREF GetTextClr() const { return mTheme.clrText; }

	static CBookThemeMgr& GetSingleInst()
	{
		static CBookThemeMgr inst;
		return inst;
	}
};
inline CBookThemeMgr& GetBookTheme()
{
	return CBookThemeMgr::GetSingleInst();
}

class SFontInfo
{
	friend class CAttrManager;
private:
	CCharAttr attr;
	int nRef;
	PFONT hFont;

	SFontInfo()
	{
		nRef=0;
		hFont=NULL;
	}
	~SFontInfo()
	{
		if (hFont!=NULL)
		{
			DeleteFont(hFont);
		}
	}
public:

	PFONT GetFont()
	{
		return hFont;
	}
	CCharAttr& GetAttr()
	{
		return attr;
	}
};


#include <map>

class CAttrManager
{
	std::map<CString,SFontInfo*> mmCharAttr;
	std::map<CString,CParaAttr> mmParaAttr;
	CRichBook* mpBook;

public:
	float m_fScaleFactor;	//字体放大因子

	CAttrManager();
	~CAttrManager() { Close(); }

	void Close();
	
	void SetBook(CRichBook* pBook) { mpBook=pBook; }
	void SetScaleFactor(float fFactor) { m_fScaleFactor=fFactor; }

	SFontInfo* CreateFont(CString sCharAttrName);
	void ReleaseFont(SFontInfo* pFontInfo);

	BOOL GetParaAttr(CString sParaAttrName,CParaAttr& attr);

	void PreloadAll();
	void UnPreloadAll();

	void FromXml(TiXmlElement* pElem);
	void ToXml(TiXmlElement* pParentElem);

	void Serialize(CArchive& ar);

	// 编辑
	void AddFontStyle(const CCharAttr& attr );
	void AddParaStyle(const CParaAttr& attr );
	void DeleteFontStyle(CString sFontStyleName);
	void DeleteParaStyle(CString sFontStyleName);
	BOOL RenameFontStyle(CString sOldName,CString sNewName);
	BOOL RenameParaStyle(CString sOldName,CString sNewName);

	BOOL ExistFontStyle(CString sCharAttrName);
	BOOL ExistParaStyle(CString sParaStyleName);
	BOOL GetFontStyleByName(LPCTSTR szName,CCharAttr& attr);
	BOOL GetParaStyleByName(LPCTSTR szName,CParaAttr& attr);

	void GetAllFontStyle(CArray<CCharAttr>& ary);
	void GetAllParaStyle(CArray<CParaAttr>& ary);

	// 
	void GetAllNoBuildinFont(CArray<SResFile>& aFont);

	// 根据属性获取其属性名称（如果已存在）并填充到m_sAttrName
	BOOL GetFontStyleName(CCharAttr& attr);
	BOOL GetParaStyleName(CParaAttr& attr);
};

class CRichObj
{
public:
	int nType;	// renderChar, renderPic, renderVideo

	CRichObj(int _type) { nType=_type; }
	virtual ~CRichObj() {}

	BOOL IsType(int _type) { return _type==nType; }

	BOOL IsChar() const { return nType==renderChar; }
	BOOL IsPic() const { return nType==renderPic; }
	BOOL IsVideo() const { return nType==renderVideo; }
	BOOL IsAudio() const { return nType==renderAudio; }
	BOOL IsLineBreak() const { return nType==renderLineBreak; }
	BOOL IsPageBreak() const { return nType==renderPageBreak; }
	BOOL IsBlank() const { return nType==renderBlank; }
	BOOL IsHorzLine() const { return nType==renderHorzLine; }

	BOOL IsNoRender()const 
	{
		return nType==renderBlank||nType==renderAnchor;
	}
	virtual CRenderUnit* Build(CAttrManager* pMgr);
	// 获取对象描述信息
	virtual CString GetInfo() ;
};

class CRichChar : public CRichObj
{
public:
	WCHAR ch;
	CString mCharAttrName;

	CRichChar() : CRichObj(renderChar),
		ch(0) {}
};
class CRichAnnotation : public CRichObj
{
public:
	WCHAR ch;
	CString mCharAttrName;
	CString msAnnotation;

	CRichAnnotation() : CRichObj(renderAnnotation),
		ch(0x6ce8/*L'注'*/) {}
};
class CRichPic : public CRichObj
{
public:
	CString mFilePath;  // file path
	CSize mszPic;		// 可以指定大小
	CString msDescription;	// 图像描述

	CRichPic() : CRichObj(renderPic), mszPic(0,0) {}

	BOOL IsAbsolutePath() const { return mFilePath.Find(L'/')!=-1 ||mFilePath.Find(L'\\')!=-1;}
};
class CRichVideo : public CRichPic
{
public:
	CString mVideoPath;

	CRichVideo() 
	{
		nType = renderVideo;
	}
};
class CRichAudio : public CRichPic
{
public:
	CString mAudioPath;

	CRichAudio() 
	{
		nType = renderAudio;
	}
};

// 仅用于定位使用，不做其他用途，在渲染时不显示，仅在编辑的时候能够看到
class CRichAnchor : public CRichObj
{
public:
	CString msAnchorName;	

	CRichAnchor() : CRichObj(renderAnchor) {}
};

class CRichHorzLine : public CRichObj
{
public:
	SHorzLine mParam ;
	
	CRichHorzLine() : CRichObj(renderHorzLine){}

	virtual CRenderUnit* Build(CAttrManager* pMgr)
	{
		CHorzLineObj* p = new CHorzLineObj ;
		p->mParam = mParam ;
		return p;
	}
};


class CPostion
{
public:
	int nPara;
	int nPos;

	CPostion();
	CPostion(int para,int pos);

	// 根据para来判断
	BOOL IsValid() { return nPara>=0 && nPos>=0; }
	void Serialize(CArchive& ar);

	// 运算符重载
	bool operator==(const CPostion& pos) const ;
	bool operator!=(const CPostion& pos) const ;
	bool operator>(const CPostion& pos) const ;
	bool operator<(const CPostion& pos) const ;
	bool operator>=(const CPostion& pos) const ;
	bool operator<=(const CPostion& pos) const ;
};

enum ECommentType
{
	commentHighlight=0,
	commentUnderline,
	commentDeline,
};

/*
	对于书摘和批注来说：mRemark,mStart,mEnd,mTime,mnCommentType,mclr为有效字段
	对于目录来说:mTitle,mStart,mnLevel,mnParaId为有效字段
*/
class CCatalogItem 
{
public:
	CString mTitle;		//对应[mStart,mEnd)的文字
	CString mRemark;	//批注
	CPostion mStart;
	CPostion mEnd;
	int mnLevel;		//
	CTime mTime;		//创建时间
	BYTE mnCommentType;
	COLORREF mclr;
	int mnParaId;

	int mnPage;		// dynamic, not save, will be updated when get, only for display

	CCatalogItem() ;
	CCatalogItem(const CCatalogItem& item) { *this=item; }

	void Serialize(CArchive& ar);
	CString FormatTime() const;
	BOOL MergeComment(const CCatalogItem& item);
	BOOL IsRemark() const { return !mRemark.IsEmpty(); }
};

class CRichPara : public CObject
{
	DECLARE_SERIAL(CRichPara)
public:
	struct SLink
	{
		int nFrom, nLength;
		CString sUrl;

		BOOL IsInLink(int nIndex)
		{
			return nIndex>=nFrom && nIndex<nFrom+nLength; 
		}
		void Serialize(CArchive& ar)
		{
			if(ar.IsStoring())  ar<<nFrom<<nLength<<sUrl;
			else				ar>>nFrom>>nLength>>sUrl;
		}
	};
	// saved
	CArray<SLink> maLinks;
	CString mParaAttrName;
	int mnParaId;		//段落标示符，一个书籍中唯一的id，用来查找

	bool mbFullWidth;	// 当为false时以下变量有效
	bool mbNotSplit;	// 段落不能拆分
	bool mbSurround;	// 下一个段落可以环绕
	bool mbByPercent;	
	bool mbOnLeft;		// left or right
	WORD mnWidth;		// percent: 0~100, or actual width
	BYTE mnSurroundDist;// 环绕距离

	// not saved
	CRichBook* mpBook;

public:

	CRichPara();	

	void GetSentenceRange(int nPos,int& ns,int& ne);
	BOOL IsLinkElement(int nIndex);
	void AddLink(int nFrom,int nLen,CString sUrl);
	int GetLinkCount() const { return maLinks.GetCount(); }
	SLink GetLinkAt(int nIndex) { return maLinks[nIndex]; }

	int CalcWidth(int nPageWidth);
	void CalcSurroundRect(const CRect& rcIn,
		CSize szPageSize,
		CPara* pRender,
		int nFrom,
		CRect rcOut[3]);
	void UpdateSurroundRects(CPageRects& pageMgr,CPara* pRender,int nFrom);


	void SetParaStyle(LPCTSTR szParaStyle) { mParaAttrName=szParaStyle; }

	//////////////////////////////////////////////////////////////////////////
	// 虚函数
	virtual BOOL IsPageBreak() const { return FALSE; }
	virtual int GetLength() const {return 0;}
	virtual BOOL IsSeparator(int nIndex){return FALSE;}
	virtual BOOL IsBlankPara() const { return FALSE; }
	virtual CString GetText(int nFrom,int nTo)
	{
		ASSERT(FALSE);
		return L"";
	}

	virtual CString GetInfo(int nPos) const { return L""; }
	virtual BOOL Click(CPostion pos,CRBClickObserver* pObserver) {return FALSE; }
	virtual void ApplyFontStyle(LPCTSTR szFontStyle) {}
	virtual void ApplyParaStyle(LPCTSTR szParaStyle) {}
	virtual BOOL IsFontStyleUsed(LPCTSTR szFontStyleName){ return FALSE; }
	virtual BOOL IsParaStyleUsed(LPCTSTR szParaStyleName) { return mParaAttrName==szParaStyleName; }
	virtual void RenameFontStyle(LPCTSTR szOldName,LPCTSTR szNewName){ }
	virtual void RenameParaStyle(LPCTSTR szOldName,LPCTSTR szNewName)
	{
		if( mParaAttrName==szOldName)
		{
			mParaAttrName = szNewName;
		}
	}


	// 渲染之前做初始化工作，如准备字体等
	virtual CPara* BuildRenderPara(CAttrManager* pAttrMgr) 
	{
		return NULL;
	}

	// 如果未找到，返回-1
	virtual int SearchText(LPCTSTR sz,int nFrom)
	{
		return -1;
	}

	virtual void Serialize(CArchive& ar);

	BOOL FromXml(TiXmlElement* pFontElem);
	void ToXml(TiXmlElement* pParentElem);
};

class CMixedPara : public CRichPara
{
	DECLARE_SERIAL(CMixedPara) 
public:
	CArray<CRichObj*> maObjs;// 

public:
	CMixedPara();
	virtual ~CMixedPara();

	void Close();

	virtual int GetLength() const 
	{
		return maObjs.GetCount();
	}
	// 排版
	virtual CString GetText(int nFrom,int nTo);
	virtual BOOL IsSeparator(int nIndex);

	virtual BOOL IsPageBreak() const ;
	virtual BOOL IsBlankPara() const ;


	virtual void Serialize(CArchive& ar);
	virtual int SearchText(LPCTSTR sz,int nFrom);
	CPara* BuildRenderPara(CAttrManager* pAttrMgr);
	virtual CString GetInfo(int nPos) const;
	virtual BOOL Click(CPostion pos,CRBClickObserver* pObserver);
	virtual void ApplyFontStyle(LPCTSTR szFontStyle);
	virtual void ApplyParaStyle(LPCTSTR szParaStyle)
	{
		mParaAttrName=szParaStyle;
	}

	BOOL FromXml(TiXmlElement* pFontElem);
	void ToXml(TiXmlElement* pParentElem);

	BOOL IsFontStyleUsed(LPCTSTR szFontStyleName);
	void RenameFontStyle(LPCTSTR szOldName,LPCTSTR szNewName);

	void AddObj(CRichObj* pObj) { maObjs.Add(pObj); }
	int FindAnchor(LPCTSTR szAnchorName);
	void RemappingResFile(CResMapping& mapping);
	void UnMappingResFile(CResMapping& mapping);
};
class CSinglePara : public CRichPara
{
	DECLARE_SERIAL(CSinglePara)
public:
	CString maChars ;		// 段落内的字符串
	CString mCharAttrName;

public:
	CSinglePara();
	virtual ~CSinglePara();


	// 排版
	virtual int GetLength() const 
	{
		return maChars.GetLength();
	}
	virtual CString GetText(int nFrom,int nTo);
	virtual BOOL IsSeparator(int nIndex);
	virtual BOOL IsBlankPara() const ;


	void Serialize(CArchive& ar);
	virtual int SearchText(LPCTSTR sz,int nFrom);

	BOOL FromXml(TiXmlElement* pFontElem);
	void ToXml(TiXmlElement* pParentElem);

	void SetFontStyle(LPCTSTR szFontStyle) { mCharAttrName=szFontStyle; }
	void AddText(LPCTSTR szText,int len) 
	{
		LPTSTR s=(LPTSTR)szText;
		TCHAR c=szText[len];
		s[len]=0;
		maChars+=s;
		s[len]=c;
	}

	virtual CPara* BuildRenderPara(CAttrManager* pAttrMgr) ;
	
	virtual CString GetInfo(int nPos) const;
	virtual BOOL Click(CPostion pos,CRBClickObserver* pObserver) ;
	virtual void ApplyFontStyle(LPCTSTR szFontStyle)
	{
		mCharAttrName=szFontStyle;
	}
	virtual void ApplyParaStyle(LPCTSTR szParaStyle)
	{
		mParaAttrName=szParaStyle;
	}
	virtual BOOL IsFontStyleUsed(LPCTSTR szFontStyleName)
	{
		return mCharAttrName==szFontStyleName;
	}
	void RenameFontStyle(LPCTSTR szOldName,LPCTSTR szNewName)
	{
		if( mCharAttrName==szOldName )
			mCharAttrName=szNewName;
	}
};

class CSpecialPara : public CRichPara
{
	DECLARE_SERIAL(CSpecialPara)
public:
	int mnType;			//段落类型, renderPageBreak
	
	// 当mnType==renderSplitColumn时有效
	int mnSplitColumn;	//分栏的栏数
	int mnColumnDist;	//栏间距 
	bool mbOnlyThisPage;//分栏只对当前页有效，缺省为TRUE

public:
	CSpecialPara() ;
	virtual ~CSpecialPara(){}

	virtual BOOL IsPageBreak() const {return mnType==renderPageBreak;}

	// 排版
	virtual int GetLength() const {	return 1; }
	virtual CString GetText(int nFrom,int nTo) ;
	virtual BOOL IsSeparator(int nIndex) { return TRUE; }
	virtual CString GetInfo(int nPos) const;

	void Serialize(CArchive& ar);

	BOOL FromXml(TiXmlElement* pFontElem);
	void ToXml(TiXmlElement* pParentElem);

	virtual CPara* BuildRenderPara(CAttrManager* pAttrMgr) ;
};

//////////////////////////////////////////////////////////////////////////

class CPage 
{
public:
	CPostion mStart;	// >=start && < end
	CPostion mEnd;	

public:
	CPage() {}
	void Serialize(CArchive& ar);
};

typedef CArray<CPage> ArrayPage;
BOOL LoadPageLayout(LPCTSTR szPageFile,ArrayPage& pages);
BOOL SavePageLayout(LPCTSTR szPageFile,ArrayPage& pages);

//////////////////////////////////////////////////////////////////////////

struct SCalcLayoutParam
{
	CAttrManager* pAttrMgr ;
	CRect	rcPage;					// in
	CString sPageFilePath;		// in

	// valid for cur mode
	ArrayPage* pAryPageOut;		// out, maybe null
	CMutex* pMutex;				// protect ArrayPage
	IProgressReceiver* pProgress;

	SCalcLayoutParam() : pAttrMgr(NULL),
		pAryPageOut(NULL),
		pMutex(NULL),
		pProgress(NULL) {}

	~SCalcLayoutParam()
	{
		if(pAttrMgr!=NULL)
		{
			delete pAttrMgr;
			pAttrMgr = NULL;
		}
	}
};

class CParaStream
{
	friend class CRichBook;
protected:
	CArray<CRichPara*> maObjs;		//所有的段落
	CRichBook* mpBook;

	BOOL IsEnd(const CPostion& pos);
	int BuildParaId();
	void ResetAllParaId();
	int FindByParaId(int nId);

public:

	CParaStream();
	virtual ~CParaStream();
	
	void RemoveAll();
	void Serialize(CArchive& ar);
	void SetBook(CRichBook* pBook);
	
	// 操作段落
	void AddPara(CRichPara* para) ;
	void InsertParaAt(int nIndex,CRichPara* para) ;
	void DeletePara(int nIndex); 
	CRichPara* RemovePara(int nIndex);
	void RemoveAllParas();	// careful, maybe cause memory leak
	void InsertParaPageBreak(int nIndex);
	void InsertParaColum(int nIndex,int nColNum,int nColDist,bool bOnlyThisPage);
	void InsertParaRemoveColum(int nIndex);

	CRichPara* GetParaAt(int nIndex) ;
	int GetParaCount() const ;
	int GetParaLength(int nPara)const ;
	int GetLength()const;	// 所有的段落长度相加
	BOOL IsEmpty()const;
	BOOL IsParaEmpty(int nPara)const;

	BOOL MovePosOneStep(CPostion& pos,BOOL bNext,BOOL bForObjIndex);
	BOOL FindAnchor(LPCTSTR szAnchorName,CPostion& pos);

	// 排版接口
	CPageRender* BuildPageRender(CPostion posFrom,CPostion posTo);
	void ReCalcLayout(SCalcLayoutParam* param,BOOL& bRunning);

	int CalcHeight(CAttrManager* pAttrMgr,int nWidth,CArray<int>* paParaHeight);
	BOOL Click(CPostion posObj,CRBClickObserver* pObserver);

	// 高级接口
	BOOL GetSentenceRange(CPostion pos,CPostion& ps,CPostion& pe);
	CPostion SearchText(CPostion posFrom,LPCTSTR sz,CArray<CPostion>& aryOut,int nMaxCount=20);
	CString GetText(CPostion ps,CPostion pe,BOOL bRemoveParaSign);
	void SearchParaByParaStyle(CArray<int>& aParas,CString sParaStyle);
	void SearchParaByFontStyle(CArray<int>& aParas,CString sFontStyle);

	// 数据转换
	BOOL FromXml(TiXmlElement* pFontElem);
	void ToXml(TiXmlElement* pParentElem);

	void RemappingResFile(CResMapping& mapping);
	void UnMappingResFile(CResMapping& mapping);

	// 编辑接口
	BOOL IsFontStyleUsed(LPCTSTR szFontStyleName);
	BOOL IsParaStyleUsed(LPCTSTR szParaStyleName);
	void RenameFontStyle(LPCTSTR szOldName,LPCTSTR szNewName);
	void RenameParaStyle(LPCTSTR szOldName,LPCTSTR szNewName);
};

//////////////////////////////////////////////////////////////////////////



class CPageRender
{
public:
	CPostion pageStart;
	CPostion pageEnd;

	struct _Para
	{
		CPara* pRenderPara;
		CRichPara* pRichPara;
	};
	struct _Block
	{
		int			nPara;
		CRichPara*	pRichPara;
		CPara*		pRenderPara;
		CRect		rc;
		int			nFrom,nTo;
	};
	CArray<_Para> maPageParas;
	CArray<_Block> maBlocks;
	CRect mRcPage;
	CRect mRcFullPage;

	// 页显示的其他信息
	CString msPageNo;		//页码	
	CString msPageTitle;	//页标题
	CString msBookName;	//页标题
	DWORD mTitleAlign;	//页标题的水平对齐方式
	PFONT hTitleFont;	//页标题使用的字体
	int   mTitleWidth;

	// 评注
	CArray<CPostion> maRemark;	//评注的位置
	PFONT hRemarkFont;


	CPageRender();
	~CPageRender();

	// 返回pos指向的对象，然后将pos指向下一个对象,
	// 有可能返回NULL，对于超过段落结尾的对象
	// 即使返回NULL，也会修改pos
	CRenderUnit* NextRenderObj(CPostion& pos);
	
	void AddPara(int nParaInSection,CRichPara* para,int nFrom);
	void Prepare(CAttrManager* pAttrMgr);
	void UnPrepare(CAttrManager* pAttrMgr);

	// 高亮搜索词,在调用prepare之后调用此函数
	void HighlightSearchWord(CString strWord,COLORREF bgClr);

	BOOL CaretInPage(const CPostion& pos) const
	{
		return pos>=pageStart && pos<=pageEnd ;
	}
	BOOL PtInPage(const CPoint& pt) const
	{
		return mRcPage.PtInRect(pt);
	}

	CPostion HitTest(CPoint pt,BOOL bForCharIndex);

	void Render(AGGDC& dc);

	CRect GetCaretRect(CPostion pos,BOOL bHead,BOOL bUseLineHeight=TRUE);

	CRect GetRangeTextRect(CPostion posFrom,CPostion posTo);
};

typedef CArray<CCatalogItem> ArrayCatalog;


enum EBookOrient
{
	bookVert=0,
	bookHori,
};

enum ECatalogType
{
	ctCatalog,		//目录
	ctBookmark,		//书签
	ctComment,		//注释
	ctRemark,		//评注
};

struct SPosRange
{
	CPostion posStart;
	CPostion posEnd;

	BOOL IsEmpty()
	{
		return posStart>=posEnd;
	}

	// 两个范围合并在一起用一个表示
	BOOL Merge(const SPosRange& range)
	{
		BOOL bHasSpace= ( posEnd<range.posStart ||posStart>range.posEnd );

		if ( !bHasSpace )
		{
			posStart = min(posStart,range.posStart);
			posEnd=max(posEnd,range.posEnd);
			return TRUE;
		}
		return FALSE;
	}

	void Serialize(CArchive& ar)
	{
		posStart.Serialize(ar);
		posEnd.Serialize(ar);
	}
};

struct SBookInfo
{
	CString sBookName;
	CString sCreator;
	CString sSubject;
	CString sLanguage;
	CString sDate;
	CString sType;

	void Close();
	void Serialize(CArchive& ar);
};

struct SSearchResult
{
	CPostion pos;			//结果所在位置
	int nPageNo;			//页码,以0开始
	CString strDisplayTxt;	//显示字符串
	CString strSectionName;	//章节名称
};

// 当点击时会调用如下函数处理点击事件
class CRBClickObserver
{
public:

	virtual BOOL ClickImage(CPostion pos,LPCTSTR szImagePath,LPCTSTR szIntro) { return FALSE; }

	virtual BOOL ClickVideo(CPostion pos,LPCTSTR szVideoPath,LPCTSTR szIntro) { return FALSE; }

	virtual BOOL ClickAudio(CPostion pos,LPCTSTR szAudioPath,LPCTSTR szIntro) { return FALSE; }

	virtual BOOL ClickLink(CPostion ps,CPostion pe,LPCTSTR szLink) { return FALSE; }

	virtual BOOL ClickChar(CPostion pos,WCHAR c) { return FALSE; }

	virtual BOOL ClickSentence(CPostion ps,CPostion pe,LPCTSTR szString) { return FALSE; }
    
    virtual BOOL ClickAnnotation(CPostion pos,LPCTSTR szAnnotation) { return FALSE; }

	virtual BOOL ClickRemark(CPostion ps,CPostion pe,LPCTSTR szRemark) { return FALSE; }
};

class _CRecalLayoutThread;
class CRichBook : public CObject
{
	friend class CRBAdaptor;
	friend class CBookThemeMgr;

	CParaStream mStream;
	SBookInfo mBookInfo;
	CAttrManager mAttrMgr;

	int mnFontSizeLevel;
	int mnBookOrient;

	ArrayCatalog mCatalog;			//目录
	ArrayCatalog mBookmark;			//书签
	ArrayCatalog mComment;			//注释和书评

	ArrayPage mCurPages;		// 当前布局的分页情况
	CPostion mposCurrent;			//上次退出时的位置

	// dynamic
	int mnCurPage ;
	int mnPageCountPerScrn;				 //每屏显示的页数
	CArray<CPageRender*> mCurPageRender; //可能一屏显示多页

	int mnPreparedPage;
	PFONT mhTitleFont;		//用于渲染章节标题和页码的字体
	PFONT mhRemarkFont;		//绘制评注

	CString msFilePath;		//文档路径
	CString msFolder;		//文档所在文件夹

	CRBClickObserver* mpClickObserver;
	IProgressReceiver* mpCalcLayoutReceiver;

	// 编辑模式变量
	CPostion mSelFrom;
	CPostion mSelTo;
	CPostion mDragCaret;
	EBookDragMode mDragMode;
	CString mHighlightText;
	CRect mrcInvalid;		//每次调用render之后清空

	CStringArray msaRegistedFont;

	// 用来保护多线程分页
	CMutex mmutex;
	_CRecalLayoutThread* mpCalcLayoutThread;
	BOOL mbCaculating;
    CPostion mPosBeforeCalcLayout;

	void InvalidRect(CRect rcInvalid) { mrcInvalid.UnionRect(&mrcInvalid,&rcInvalid); }

	ArrayPage& GetPageLayout();
	CRect GetPageRect(BOOL bVert,BOOL bFullPage);

	void RegistFont(CString sDir);
	void UnRegistFont();


	// 针对pagerender
	void CommentPart(const CCatalogItem& item,
		CPageRender* pPageRender=NULL);

	void SelectPart(CPostion ps,CPostion pe,BOOL bSelect,CPageRender* pPageRender=NULL);


	// 准备渲染
	void PreparePages();
	void UnPreparePages();
	CPageRender* PreparePage(int nPage);

	// 序列化
	BOOL SaveBinDoc(LPCTSTR szFilePath);
	BOOL SaveAsXmlDoc(LPCTSTR szXmlPath);
	BOOL SaveAsTxtDoc(LPCTSTR szXmlPath);
	BOOL LoadBinDoc(LPCTSTR szFilePath);
	BOOL LoadXmlDoc(LPCTSTR szFilePath);
	void Serialize(CArchive& ar);
	// 序列化用户看书过程中产生的数据，如书摘、当前页数、评注等
	void SerializeUser(CArchive& ar);
	BOOL LoadUser(CString szFilePath);
	BOOL SaveUser(CString szFilePath);
	BOOL InternalLoad(CString szFilePath);

	void OnOpen();

	CPostion HitTest(CPoint pt,BOOL bForCharIndex);

	void ReCalcLayout(BOOL bCalcAllMode,BOOL bSyncMode);
	SCalcLayoutParam* BuildCalcParam(BOOL bVert,int nBookStyle,BOOL bCurMode);

public:
	void Test();

	CRichBook();
	virtual ~CRichBook();

	//////////////////////////////////////////////////////////////////////////
	// 基本函数
	CParaStream& GetParaStream() { return mStream; }

	void Close();
	void NewBook();
	BOOL IsCalcLayout() ;
	void StopCaculate();
	void WaitHasPages();
    BOOL IsCurPosVisible();
	BOOL UpdatePageLayout(BOOL bRecalOnFail,CPostion posGoto=CPostion(-1,-1));

	void SetTheme(EBookThemeStyle theme);
	EBookThemeStyle GetTheme() const ;

	void SetBookInfo(const SBookInfo& info) { mBookInfo=info; }
	const SBookInfo& GetBookInfo() const { return mBookInfo; }
	
	// font size level
	// caller should render at once when return true
	BOOL SetFontSizeLevel(int nLevel) ;
	void ChangeFontSizeLevel(int nChangeLevel);
	int GetFontSizeLevel() const { return mnFontSizeLevel; }
	
	// book name
	void SetBookName(LPCTSTR szName) ;
	CString GetBookName() ;	

	// book path
	CString GetDocPath() const { return msFilePath; }
	CString GetResPath()  ;
	// 获取资源文件的绝对路径
	CString GetAbsoluteResPath(CString sRelResPath);
	void SetResPath(CString sResFolder) ;
	// caller should render at once when return true
	BOOL SetBookOrient(EBookOrient bo) ;
	int GetBookOrient() const { return mnBookOrient; }
	BOOL IsVerticalMode() const { return mnBookOrient==bookVert; }

	// 获取目录
	ArrayCatalog& GetCatalog(ECatalogType catalogType);
	void GetCatalog(ArrayCatalog& aryCatalog,ECatalogType catalogType);
	void UpdatePageNo(ArrayCatalog& aryCatalog);
	void UpdateCatalog();

	// 渲染
	void ForceRefresh();
	void Render(AGGDC& dc);
	void RenderPara(AGGDC& dc,CRichPara* pPara);


	// 序列化/加载
	BOOL Load(CString szFilePath);
	BOOL Save(CString szFilePath);
	BOOL MakePackage(LPCTSTR szZipFilePath);
	BOOL LoadUser();
	BOOL SaveUser();
	
	static CString GetBookNameFromFile(CString sBookFilePath);

	// 翻页/跳转
	void GotoPage(int nPage);
	void GotoNextPage();
	void GotoLastPage();
	void GotoPosition(const CPostion& pos);
	void GotoCatalog(int nCatalogItem);
	void GotoComment(int nCatalogItem);
	void GotoBookmark(int nCatalogItem);
	BOOL GotoAnchor(CString sAnchor);

	int GetLength() ;
	int GetPageCount() ;
	BOOL IsFirstPage() { return mnCurPage==0; }
	BOOL IsLastPage() { return mnCurPage+mnPageCountPerScrn>=GetPageCount(); }
	
	CPostion GetCurPosition()  ;
	int GetCurPage() const { return mnCurPage; }
	int GetPositionPageNo(const CPostion& pos);
	BOOL IsPositionValid(const CPostion& pos);

	// 鼠标事件
	BOOL OnTouchStart(CPoint pt,EBookDragMode mode=dragSel);
	void OnTouchMove(CPoint pt);
	BOOL Click(CPoint pt);
	void SetClickObserver(CRBClickObserver* pClickObserver) ;
	void SetCalcLayoutObserver(IProgressReceiver* pProgressReceiver);
	
	CRect GetInvalidRect() const { return mrcInvalid; }
	CRect GetRangeTextRect(CPostion p1,CPostion p2);
	void OnThemeChanged();


	// 选取函数
	BOOL IsSelEmpty() ;
	void RemoveSel() ;
	void Select(CPostion posFrom,CPostion posTo);
	void SetSel(CPoint ptFrom,CPoint ptTo);
	void SelSentence(CPoint pt);
	void SelSentence(CPostion pos);
	CRect GetCaretRect(CPostion pos,BOOL bHead,BOOL bUseLineHeight);
	CRect GetSelRect();

	// 书签/书摘...
	BOOL AddBookmark(); // 添加书签
	void RemoveBookmark();// 删除书签
	BOOL AddSelToComment(CString strRemark,ECommentType nCommentType);	// 书摘
	void AddComment(CString strRemark,CPostion posFrom,CPostion posTo,ECommentType nType);

	void InsertCatalogItem(const CCatalogItem& item);
	void DeleteCatalogItem(int nIndex);
	BOOL EditCatalogItem(int nIndex,const CCatalogItem& item);

	// 书签书摘批注操作
	BOOL HasBookmark();
	BOOL HasComment(CPoint pt,CPostion& pos);
	BOOL GetCommentRange(CPostion pos,CPostion& ps,CPostion& pe);
	void RemoveComment(CPostion pos);
	CRect GetCommentRect(CPostion pos);
	CString GetRemark(CPostion pos);	//如果pos所在为书摘则返回空
	void SetRemark(CPostion pos,CString sRemark);

	// 返回最后的搜索位置
	CPostion SearchText(CPostion posFrom,LPCTSTR szText,CArray<CPostion>& aryOut,int nMaxCount=20);
	void GetSearchForDisplay(const CPostion& pos,SSearchResult& result);
	CString GetDisplayTextAt(CPostion pos);
	void SetHighlightText(CString strWord);

	// 获取字符串
	CString GetText(CPostion ps,CPostion pe,BOOL bRemoveParaSign=FALSE);
	CString GetSelString();

	//////////////////////////////////////////////////////////////////////////
	// 获取某页的章节名称，从目录中获取，没有找到返回空
	CString GetPageTitle(int nPage);
	// 获取某个位置的章节标题
	CString GetPositionTitle(CPostion pos);
	// 返回与pos所属的章节标题在目录中的索引
	int GetCatalogIndexByPos(const CPostion& pos);
	// 获取某一页的提示信息
	CString GetPagePopInfo(int nPage);
	
	
	//////////////////////////////////////////////////////////////////////////
	// 编辑函数
	// 文件格式转换函数
	CSize GetRenderSize() const ;
	CRect GetRenderMargin() const ;


	void BuildNormalAttrMgr();
	void ExtractCatalog();


	//////////////////////////////////////////////////////////////////////////
	// 样式
	CAttrManager& GetAttrMgr();

	BOOL LoadStyle(LPCTSTR szStylePath);
	BOOL SaveStyle(LPCTSTR szStylePath);

	//添加风格到指定的类型
	void AddFontStyle(const CCharAttr& attr );
	void AddParaStyle(const CParaAttr& attr );
	
	BOOL DelFontStyle(LPCTSTR szFontStyleName);
	BOOL DelParaStyle(LPCTSTR szParaStyleName);
	BOOL RenameFontStyle(LPCTSTR szOldName,LPCTSTR szNewName);
	BOOL RenameParaStyle(LPCTSTR szOldName,LPCTSTR szNewName);

	void DelAllUnusedStyle();

	CBookWriter* BeginWrite();
	void EndWrite(CBookWriter*& pWriter);
};



template<class T>
void SerializeObjPtrAry(CArchive& ar,CArray<T*>& ary)
{
	int nc,i;
	if (ar.IsStoring())
	{
		nc = ary.GetCount();
		ar << nc;
		for (i=0;i<nc;i++)
		{
			ary[i]->Serialize(ar);
		}
	} 
	else
	{
		for (i=0;i<ary.GetCount();i++)
		{
			delete ary[i];
		}
		ary.RemoveAll();

		ar >> nc;
		for (int i=0;i<nc;i++)
		{
			T* p=new T();
			p->Serialize(ar);
			ary.Add(p);
		}
	}
}
template<class T>
void SerializeObjAry(CArchive& ar,CArray<T>& ary)
{
	int nc;
	if (ar.IsStoring())
	{
		nc = ary.GetCount();
		ar << nc;
		for (int i=0;i<nc;i++)
		{
			ary[i].Serialize(ar);
		}
	} 
	else
	{
		ar >> nc;
		ary.SetSize(nc);
		for (int i=0;i<nc;i++)
		{
			ary[i].Serialize(ar);
		}
	}
}

// 将纯文本转换为richibook格式的xml
BOOL ConvertTxtToBook(LPCTSTR szTxtFilePath,LPCTSTR szBookFilePath);
void ConvertToRichBookXml(CString strTxt,CRichBook& book);

//////////////////////////////////////////////////////////////////////////

// 书籍下载信息
struct SBookDownloadInfo
{
	CString sBookProductID;
	CString sBookAppID;
	CString sUrl;
	CString sBookName;
	CString sBookDigest;		//md5
};

enum EBookState
{
	bsNotDownload,
	bsDownloading,
	bsDownloaded,
    bsDownloadFailed,
	bsInstallFailed,
	bsOK,
};
// 书架
struct SBookItem
{
	BOOL	bGroup;				// must, if true, it means group only sBookName has meaning, otherwise it's book
	EBookState	bookState;		// if true, sBookPath will be empty
	BOOL	bTryVersion;		// 试用版？

	// always valid
	CString sBookName;			// must, 书的名称,如果group为true，表示组的名称
	CString sBookUrl;			// must, if empty, it's a local book
	CString sBookID;			// must,
	CString sCoverUrl;			// optional, icon url
	CString sIconPath;			// optional, 书的图标路径
	CString sDownFullPath;		//

	// valid after download finish
	CString sBookPath;			// must, 书的文件路径
	CTime	tLastVisited;		// 
	CTime	tCreated;

	// child books when it's a group
	CArray<SBookItem*> aGroupBooks;	//

	SBookItem();
	SBookItem(const SBookItem& item);
	~SBookItem();

	SBookItem& operator = (const SBookItem& item);

	void Close();
	void Serialize(CArchive& ar);

	BOOL AddBook(CString _sBookFullPath,CString _sBookName,CString sToGroup=L"/");
	BOOL DeleteBook(int nIndex);
	BOOL DeleteByID(CString _sBookID);
	BOOL DeleteByFullPath(CString sBookPath);

	BOOL GetBooks(SBookItem& books,CString sGroupPath=L"/");
	SBookItem* FindBookById(CString _sBookID);
	SBookItem* FindBookByUrl(CString _sUrl);

	int GetBookCount();
};

typedef CArray<SBookItem*> ArrayBooks;

void EnumBooksInFolder(CString sFolder,CArray<SBookItem>& aBooks);

// 从网上下载的书籍格式 
//
// zip:
//		*.book
//		font/...
//		res/...
//		info.xml
//
// 目录结构：
//		工作目录:
//			book1/
//				*.book
//				font/
//				res/
//				info.xml
//			book2/
//				....
//			bookn
//				...
//


class CBookMgr
{
    friend struct SBookItem ;
    
	SBookItem	mBookRootItem;
	CString msWorkingDir;		//工作目录

	CBookMgr();
	~CBookMgr();

	struct _SUploadedFiles
	{
		CString sOriginalFileName;	// 
		CString sFilePath ;			// for zip, it's the folder path, for other, it's absolute path
        bool bZip;                  //
	};

	CArray<_SUploadedFiles> maUploadedFiles;

    void OnDeleteBookItem(const SBookItem* pItem);
    
public: 
	BOOL SetWorkingDir(LPCTSTR szDir);

	BOOL LoadBooks();
	BOOL SaveBooks();

	BOOL StartDownloadBook(CString sParamFromClick,CString& sBookID);
	BOOL FinishDownloadBook(CString sBookId,BOOL bSuccessDown,CString sLocalTempFile);
	SBookItem* FindBookItem(CString sBookId);
	SBookItem* FindBookByUrl(CString sBookUrl);

	/// 安装一本书，在下载完成后
	BOOL InstallLocalBook(CString sBookPath);
	BOOL UnstallBook(CString sBookID);
	BOOL UnstallByIndex(int nIndex);
	BOOL UnstallByIndexSet(CArray<int>& aIndex);
	BOOL UnstallByPath(CString sPath);

	//
	void GetBooks(SBookItem& rootBooks,CString sGroupPath=L"/");
	ArrayBooks& GetRootBooks() { return mBookRootItem.aGroupBooks; }

	// For Uploaded Files
	void GetUploadedFiles(CStringArray& saFileNames);
	void AddUploadedFile(CString sFileName,CString sFilePath,bool bzip);
	void DelUploadFile(CString sFileName);
    int GetUploadCount() const { return maUploadedFiles.GetCount(); } 
	CString GetOrignalFileUploadPath(CString sFileName);

	static BOOL HasBookInDir(CString sDir);
	static CBookMgr& GetBookMgr();
	static void MoveBookItem(ArrayBooks& books,int nItemFrom,int nToPosition);

	void Test();

protected:
	void Serialize(CArchive& ar);
};

//////////////////////////////////////////////////////////////////////////


// 用来生成复杂格式的段落
class CParaWriter
{
	struct _Sentence
	{
		int nType;			//ERenderType
		CString sText;
		CString sImg;		//if not empty, it's a img element
		CString sVideo;
		CString sAudio;
		CString sFont;
		CString sUrl;		//if not empty, it's a link
		CSize szPic;
		SHorzLine line;

		int GetLength() const 
		{
			if( nType==renderPic )
				return 1;
			if( nType==renderChar )
				return sText.GetLength();
			return 1;
		}
	};
	CArray<_Sentence> maElements;
	CString msParaStyle;

	BOOL IsSmplePara() const;
public:
	int GetLength() const;
	BOOL IsOnlyAnchor() const ;
	CRichPara* ToPara();
	void Clear() { maElements.RemoveAll(); }

	// 编辑模式
	void SetParaAttr(LPCTSTR szParaStyle) { msParaStyle=szParaStyle; }
	void WriteText(LPCTSTR szText,LPCTSTR szFontStyle,LPCTSTR szLink=NULL);
	void WriteImg(LPCTSTR szImg,CSize sz,LPCTSTR szIntro,LPCTSTR szLink=NULL);
	void WriteVideo(LPCTSTR szVideo,LPCTSTR szImg,CSize sz,LPCTSTR szIntro,LPCTSTR szLink=NULL);
	void WriteAudio(LPCTSTR szAudio,LPCTSTR szImg,CSize sz);
	void WriteSpecial(ERenderType _type);
	void WriteHorzLine(SHorzLine line);
	void WriteAnnotation(LPCTSTR szText,LPCTSTR szFontStyle);
	void WriteAnchor(LPCTSTR szText);
};
#include <set>
class CBookWriter 
{
	friend class CRichBook;

	CParaStream mStream;
	std::set<CString> maImgs;

	// saved
	CArray<CParaWriter*> maWriter ;
	CArray<CCharAttr> maCharAttrs;
	CArray<CParaAttr> maParaAttrs;

	// dynamic
	CParaWriter mParaWriter ;
	CString m_sLink;

	CCharAttr	mcharAttr;
	CParaAttr	mparaAttr;

	CList<CCharAttr> mstackCharAttr;
	CList<CParaAttr> mstackParaAttr;

	BOOL ExistCharAttr(const CCharAttr& attr);
	BOOL ExistParaAttr(const CParaAttr& attr);
public:

	CBookWriter();

	// char attr
	void GetCharAttr(CCharAttr& attr) ;
	void SetCharAttr(const CCharAttr& attr,BOOL bBuildName=TRUE) ;
	void PushCharAttr() ;
	void PopCharAttr()  ;

	// para attr
	void GetParaAttr(CParaAttr& attr);
	void SetParaAttr(const CParaAttr& attr,BOOL bBuildName=TRUE);
	void PushParaAttr();
	void PopParaAttr();

	// 以下两种不会保存属性，仅仅更改属性名称，需要确保书籍中
	// 已经包含两种风格
	void SetCharAttrName(LPCTSTR szAttrName);
	void SetParaAttrName(LPCTSTR szAttrName);

	void Bold(BOOL b);
	void Italic(BOOL i);
	void UnderLine(BOOL u);
	void Color(COLORREF clr);

	// 完成当前段落的编辑并开始新的段落
	void FinishPara(BOOL bForce=FALSE);

	void StartLink(LPCTSTR szLink);
	void EndLink();

	void WriteText(LPCTSTR szText,int len=-1);
	void WriteImg(LPCTSTR szImg,CSize sz=CSize(0,0));
	void WriteSpecial(ERenderType _type);
	void AppendPageBreak();
	void WriteHorzLine(SHorzLine line);
	void WriteAnnotation(LPCTSTR szText);
	void WriteAnchor(LPCTSTR szText);

	int GetParaCount() const { return mStream.GetParaCount(); }
	void ToRichBook(CRichBook* pBook);
	void GetImgs(CStringArray& saImg) ;
};

class CGlobalParam
{
	CString msFilePath;

public:
	CString msDefChFont;
	CString msDefWstFont;
	CString msLastDocument;		//
    bool    mbOpenLastOnStart;


	static CGlobalParam& GetSingleInst()
	{
		static CGlobalParam inst;
		return inst;
	}
	// 调用此函数初始化
	void SetParamPath(CString sPath);

	BOOL Save();
	BOOL Load();

	BOOL IsFirstRun() const { return !IsFileExist(msFilePath); }
    BOOL IsLastDocValid() const ;
    
protected:
	CGlobalParam() ;
	void Serialize(CArchive& ar);
};

// 对于ios或者android提供一个默认的书籍实例
CRichBook& GetBookInst();
