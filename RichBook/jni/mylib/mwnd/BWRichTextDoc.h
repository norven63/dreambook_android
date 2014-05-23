#pragma once


/*
	��Ҫ����ʵ�������Ű棬�������ֶ���ͬһҳ����ʾ��������CRichBook��
	��ҳ���½��Ű棬�����ڴ�����������ҳ����
*/

class CRenderDoc;

class SRenderObj
{
private:
	int nType;

	struct
	{
		WCHAR c;
		int nTextAttrID;
	}text;
	CString szParam;
	CSize	szObjectSize;		//����ߴ磬����ͼƬ����Ƶ
	SHorzLine mLineParam ;
	CString msIntro;		//����ͼƬ����Ƶ�ȵý���
	CString msVideoPath;
	CString msAudioPath;

	SRenderObj(const SRenderObj&) {}
	SRenderObj& operator=(const SRenderObj&);
public:
	// ����
	int mnSplitColumn;	//����������
	int mnColumnDist;	//����� 
	bool mbOnlyThisPage;//����ֻ�Ե�ǰҳ��Ч��ȱʡΪTRUE

public:

	// for special obj
	SRenderObj(int _type) : nType(_type) {}

	// for char obj
	SRenderObj(WCHAR c,int nTextAttrID)
	{
		nType = renderChar; text.c=c; text.nTextAttrID=nTextAttrID;
		szObjectSize = CSize(0,0);
	}

	// for image
	SRenderObj(LPCTSTR szImagePath,CSize sz,LPCTSTR szIntro)
	{
		nType = renderPic; szParam=szImagePath;
		szObjectSize = sz;
		msIntro = szIntro ;
	}

	// for video
	SRenderObj(LPCTSTR szVideoPath,LPCTSTR szImgagePath,CSize sz,LPCTSTR szIntro)
	{
		nType = renderVideo; szParam=szImgagePath;
		szObjectSize = sz;
		msVideoPath=szVideoPath;
		msIntro = szIntro ;
	}
	// for audio
	SRenderObj(LPCTSTR szAudioPath,LPCTSTR szIconPath,CSize szIcon)
	{
		nType = renderAudio; szParam=szIconPath;
		szObjectSize = szIcon;
		msAudioPath=szAudioPath;
	}

	// for annotation
	SRenderObj(int nTextAttrID,LPCTSTR szAnnotation)
	{
		nType = renderAnnotation; szParam=szAnnotation;
		text.nTextAttrID = nTextAttrID;
		text.c = L'ע';
	}

	// for anchor
	SRenderObj(int nTextAttrID,CString sAnchorName)
	{
		nType = renderAnchor;
		text.nTextAttrID = nTextAttrID;
		szParam = sAnchorName;
	}

	// for horzline
	SRenderObj(const SHorzLine& line)
	{
		nType = renderHorzLine;
		mLineParam=line;
	}

	// for split column
	SRenderObj(int nColumn,int nDist,bool bThisPage)
	{
		nType = renderPageBreak;
		mnSplitColumn=nColumn;
		mnColumnDist=nDist;
		mbOnlyThisPage=bThisPage;
	}

	CRenderUnit* Build(CRenderDoc* pDoc) const;

	ERenderType GetType() const { return (ERenderType) nType; }
	
	BOOL IsType(ERenderType _type) const { return nType==_type; }
	BOOL IsText() const { return nType==renderChar; }
	BOOL IsImage() const { return nType==renderPic; }
	BOOL IsVideo() const { return nType==renderVideo; }
	BOOL IsAudio() const { return nType==renderAudio; }
	BOOL IsLineBreak() const { return nType==renderLineBreak; }
	BOOL IsPageBreak() const { return nType==renderPageBreak; }
	BOOL IsBlank() const {return nType==renderBlank; }
	BOOL IsHorzLine() const {return nType==renderHorzLine; }

	BOOL GetText(WCHAR& c,int& nID) const;
	WCHAR GetText() const { return text.c; }
	BOOL GetImage(CString& szImagePath) const;
	CString GetImage() const { return szParam; }
	CSize GetImageSize() const { return szObjectSize; }
	SHorzLine GetHorzLine() const { return mLineParam; }
	CString GetAnnotation() const { return szParam; }
	CString GetAnchor() const { return szParam; }
	CString GetImgIntro() const { return msIntro; }
	CString GetVideoPath() const { return msVideoPath; }
	CString GetAudioPath() const { return msAudioPath; }

	int GetTextAttr() const 
	{
		if( nType!=renderChar&&nType!=renderAnnotation)
			return 0;
		return text.nTextAttrID; 
	}
	void SetTextAttr(int nNewTextAttr)
	{
		if (nType==renderChar||nType==renderAnnotation)
			text.nTextAttrID=nNewTextAttr;
	}

	CString GetInfo() const ;
};
struct SLink
{
	int mFrom;	// �����ӷ���[mFrom,mTo)
	int mTo;
	CString sLinkUrl;

	SLink() { mFrom=mTo=0; }

	// �������в���һ���ַ�ʱ���ô˺�������������ʼλ��
	void InsertAt(int nIndex,int nLen=1);
	// ��������ɾ��һ���ַ�ʱ���ô˺�����������λ��
	void DeleteAt(int nIndex,int nLen=1);
	BOOL IsValid() const { return mFrom>=0 && mFrom<mTo && !sLinkUrl.IsEmpty(); }
	BOOL IsInLink(int nIndex) const { return nIndex>=mFrom && nIndex<mTo; }
	BOOL MergeWith(const SLink& link);
};
class CRenderPara : public CPara
{
	friend class CRenderDoc;

	CArray<SRenderObj*> maObjs;// 
	CSize	mszLayout;
	CRenderDoc* mpDoc;
	CArray<SLink> maLinks;
	int mnParaId;

	BOOL PrepareRender(void* pAttrMgr);
	void UnprepareRender(void* pAttrMgr);
	BOOL IsTextIDUsed(int nTextId)const;
	int GetTextAttrFromCaret(int nIndex);
	void ChangeRenderObjToLink(int nIndex);

	void OutputDebug(int nPara);
public:
	CRenderPara(CRenderDoc* pDoc) ;
	~CRenderPara();

	void Close();

	int GetLength() const {	return maObjs.GetCount();}
	int GetCount() const { return maObjs.GetCount(); }
	virtual CString GetText(int nFrom,int nTo);

	int GetParaId() const { return mnParaId; }
	void SetParaId(int nNewParaId) { mnParaId=nNewParaId;}

	void SetParaAttr(const CParaAttr& attr,BOOL bBatchEdit) ;

	CSize GetSize() const ;
	BOOL HasParaEndChar() const;

	//
	SRenderObj* GetObjAt(int nIndex) { return maObjs[nIndex]; }
	BOOL IsSingleTextPara() ;
	BOOL IsSpecialPara();
	CString GetObjInfo(int nIndex);

	BOOL IsInLink(int nCharIndex,SLink* pLinkOut=NULL);
	void AddLink(int nFrom,int nTo,CString sUrl);
	BOOL DelLink(int nCharIndex);
	BOOL DelLink(int nFrom,int nTo);
	int GetLinkCount() const { return maLinks.GetCount(); }
	SLink GetLinkAt(int nIndex) { return maLinks[nIndex]; }

	void SetWidth(int nWidth);
	void UpdateLayout();

	void Select(int nFrom,int nTo,BOOL bSel);
	void SetTextAttr(int nFrom,int nTo,int nNewTextAttr);
	void ChangeTextAttr(int nFrom,int nTo,const CCharAttr& attr,DWORD dwMask);

	//////////////////////////////////////////////////////////////////////////
	// �༭ģʽ����
	//
	// �޸ĺ󲻻��Զ������²��֣����ⲿ���е��ü���,���nTextAttrID==0����ʹ��nIndex֮ǰ���Ǹ��ַ�������
	void InsertChar(int nIndex,TCHAR c,int nTextAttrID=0);
	void InsertObj(int nIndex,SRenderObj* pObj,BOOL bBatchInsert);
	// ɾ�����ַ���С��nDelCountʱ����FALSE�����򷵻�TRUE
	BOOL Delete(int nIndex,int nDelCount=1);

	// ��������
	CRenderPara* SplitFrom(int nIndex);
	// ������ϲ�
	void MergeWith(CRenderPara* pNext);

	//////////////////////////////////////////////////////////////////////////
	// ������Ӻ���,���������Ⱦ����
	void BatchInsertText(int nIndex,LPCTSTR sz,int nLen,int nTextAttrID=0,LPCTSTR szLink=NULL);
};

struct SCaret
{
	int nPara;
	int nIndex;
	SCaret() { nPara=0; nIndex=0; }
	SCaret(int _para,int _index) : nPara(_para),nIndex(_index) {}

	BOOL operator==(const SCaret& pos) const { return nPara==pos.nPara && nIndex==pos.nIndex; }
	BOOL operator>(const SCaret& pos) const { 
		if(nPara==pos.nPara)
			return nIndex>pos.nIndex;
		return nPara>pos.nPara;
	}
	BOOL operator<(const SCaret& pos) const { 
		if(nPara==pos.nPara)
			return nIndex<pos.nIndex;
		return nPara<pos.nPara;
	}
	BOOL operator>=(const SCaret& pos) const { return *this>pos ||*this==pos; }
	BOOL operator<=(const SCaret& pos) const { return *this<pos ||*this==pos; }
	BOOL operator!=(const SCaret& pos) const { return !(*this==pos); }
};
// �ö���һ�����������ܸ���������
class CTextAttr 
{
private:
	int		nID;			// Ψһ��ʶ������

	CCharAttr attr;

	HFONTREF hFont;

public:
	CTextAttr(int _id,const CCharAttr& charAttr):
	  nID(_id), attr(charAttr), hFont(NULL)
	{}
	  ~CTextAttr() 
	  {
		  if(hFont) 
		  {
			  ReleaseFontRef(hFont);
			  hFont=NULL;
		  }
	  }

	  const CCharAttr& GetFontAttr() const { return attr; }
	  COLORREF GetTextClr() const { return attr.m_clr; }

	  PFONT GetFont()
	  {
		  if( hFont==NULL )
		  {
			  hFont = LoadFontRef(attr.m_font);
		  }
		  return *hFont;
	  }
	  BOOL IsSameAs(const CCharAttr& charAttr) const
	  {
		  return attr==charAttr ;
	  }
};


class CRenderDoc
{
	friend class CRenderPara;

	CArray<CRenderPara*> mParas ;
	CSize	mszLayout;
	CPoint	mptDest;
	BOOL	mbPrepared;
	UINT	mnLength;

	std::map<int,CTextAttr*> mTextAttr;

	// ��̬����
	SCaret	mCaret;
	CRect	mrcInvalid;
	CRect	mrcCaret;	//���
	SCaret  mselFrom;
	SCaret	mselTo;
	
	// ��ǰʹ�õ���������
	int		mnTextID;
	CCharAttr mCharAttr;

	BOOL mbBatchEditing;

	BOOL PrepareRender();
	void UnPrepareRender();
	void UpdateLayout();
	CRect GetParaRect(int nFrom,int nTo=-1);
	BOOL IsTextIDUsed(int nTextId)const;
	int ChangeCharAttr(int nOldId,const CCharAttr& attr,DWORD dwMask);
	void UpdateTextID();
	void RestoreTextAttrFromCaret();
	void InvalidRect(const CRect& rc) { mrcInvalid.UnionRect(&mrcInvalid,&rc); }
	SCaret HitTest(CPoint pt,BOOL bForObjIndex=FALSE);
	void UnSelect();
	void Select(SCaret  from,SCaret to,BOOL bSel);

	void InsertObj(SRenderObj* pObj);
public:
	CRenderDoc();
	~CRenderDoc();

	//��ʼһ���µ��ĵ���ɾ���������е����ݣ�
	void NewDoc();

	CTextAttr* GetTextAttr(int nID) ;
	const CTextAttr* GetTextAttr(int nID) const;

	SCaret GetCaret() const { return mCaret; }
	
	BOOL CvtTextAttr2CharAttr(int nTextAttrID,CCharAttr& attrOut)const;

	void Close();
	UINT GetLength() const { return mnLength; }
	BOOL IsEmpty() const { return mnLength==0; }

	void SetCurTextAttr(const LOGFONT& font,COLORREF clr);
	void SetCurTextAttr(const CCharAttr& charAttr);
	void SetCurTextAttr(const CCharAttr& charAttr,DWORD dwMask);
	void GetCurTextAttr(CCharAttr& charAttr);
	void GetCurTextAttr(LOGFONT& font,COLORREF& clr);
	void GetCurParaAttr(CParaAttr& attr);
	void SetCurParaAttr(const CParaAttr& attr);
	void SetCurParaId(int nParaId);

	// Ӧ�ý��ڹ���ʱ����
	void SetDestPt(CPoint pt) { mptDest=pt; UpdateCaret(); }
	int  GetWidth() const { return mszLayout.cx; }
	void SetWidth(int nWidth) ;
	CSize GetSize() const { return mszLayout; }

	//
	int GetParaCount() const { return mParas.GetCount(); }
	CRenderPara* GetParaAt(int nIndex) { return mParas[nIndex]; }

	// ��ȡ��������
	CString GetAllText() const ;
	CString GetSelText()  ;

	BOOL IsSelEmpty() const { return mselFrom==mselTo; }
	void Click(CPoint pt);
	void Drag(CPoint ptFrom,CPoint ptTo);
	CString GetObjInfo(CPoint pt);

	CRect GetDirtyRect() const { return mrcInvalid; }
	void Render(AGGDC* pDC);

	//////////////////////////////////////////////////////////////////////////
	// ���º��������ڱ༭ģʽ��ÿ�β����ַ�����ɾ���ַ�������¹�ꡢ�ĵ��ߴ缰��Ч����
	// 
	// �༭ģʽ����
	void AppendPara();	//���һ���µĶ���
	
	// �ڵ�ǰ��괦�������ݣ����ƶ���ǰ���
	void InsertText(LPCTSTR sz);
	void InsertChar(TCHAR c);
	void InsertHorzLine(SHorzLine line);
	void InsertImage(LPCTSTR szImagePath, CSize sz, LPCTSTR szIntro);
	void InsertVideo(LPCTSTR szVideoPath, CSize sz, LPCTSTR szIntro,LPCTSTR szIconPath);
	void InsertAudio(LPCTSTR szAudioPath, LPCTSTR szImagePath, CSize sz);

	void InsertSpecial(ERenderType type);
	void InsertSplitColumn(int nSplitColumn,int nColumnDist,bool bOnlyThisPage);
	void InsertAnnotation(LPCTSTR sz);
	void InsertAnchor(LPCTSTR szAnchorName);
	void Delete(int nDelCount);
	void DeleteSel();
	void SetSelLink(LPCTSTR szLink);	// ���õ�ǰѡ��Χ�ĳ�����
	void InsertLink(int nPara,int nFrom,int nTo,LPCTSTR szLink);

	// �ƶ����
	BOOL MoveTo(int nPara,int nIndexInPara,BOOL bUpdateCaretRect=TRUE);
	BOOL MoveToBegin(BOOL bUpdateCaretRect=TRUE);
	BOOL MoveToEnd(BOOL bUpdateCaretRect=TRUE);
	BOOL MoveCaretPrev(BOOL bUpdateCaretRect=TRUE);
	BOOL MoveCaretNext(BOOL bUpdateCaretRect=TRUE);
	BOOL MoveCaretUp(BOOL bUpdateCaretRect=TRUE);
	BOOL MoveCaretDown(BOOL bUpdateCaretRect=TRUE);
	void UpdateCaret();
	CRect GetCaretRect() const { return mrcCaret; }

	//////////////////////////////////////////////////////////////////////////
	// ���º�����������������ģʽ,�ڴ�ģʽ�ڼ䣬���λ�á���������ѡ�����ԵȾ���Ч
	// �����������������
	BOOL IsBatchEditing() const { return mbBatchEditing; }
	BOOL BeginBatchEdit();
	void EndBatchEdit();
	void BatchInsertText(CString sText,LPCTSTR szLink=NULL);
	void BatchInsertChar(WCHAR c,LPCTSTR szLink=NULL);

	void Debug();
};
