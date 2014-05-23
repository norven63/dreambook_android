#pragma once


// 通过CResManager创建和销毁
class CFontRef
{
	friend class CResManager;

	LOGFONT logFont;
	int nRef;
	PFONT hFont;
public:
	PFONT GetFont() { return hFont; }

	operator PFONT() const{ return hFont; }

	const LOGFONT& GetLogFont() const { return logFont; }
};
typedef CFontRef* HFONTREF;
// 通过CResManager创建和销毁
class CImgRef
{
	friend class CResManager;

	CString sImgPath;
	int nRef;
	HIMAGE pImg;
public:
	HIMAGE GetImg() { return pImg; }

	operator HIMAGE() const { return pImg; }

	CSize GetSize() const { return GetImageSize(pImg); }
};
typedef CImgRef* HIMAGEREF;


class CResManager
{
	std::map<LOGFONT,CFontRef*> mmFont;
	std::map<CString,CImgRef*> mmImg;

public:
	CResManager(){}
	~CResManager() { ReleaseAll(); }

	void ReleaseAll();

	CFontRef* LoadFont(LPCTSTR szFontName,
		int nSize,BYTE bBold,BYTE bItalic,
		BYTE bUnderline,BYTE bDelLine,
		BOOL bAntiAlias);
	void ReleaseFont(CFontRef* pRef);

	CImgRef* LoadImg(LPCTSTR szImgPath);
	void ReleaseImage(CImgRef* pRef);
};

// 注意：缺省的资源管理器未进行加锁管理
// 使用缺省的管理器加载字体和图像，程序内各组件共享资源

CResManager& GetDefaultResMgr();
HFONTREF LoadFontRef(LPCTSTR szFontName,
					 int nSize,BYTE bBold,BYTE bItalic,
					 BYTE bUnderline,BYTE bDelLine,
					 BOOL bAntiAlias);
HFONTREF LoadFontRef(const LOGFONT& logFont);
void ReleaseFontRef(HFONTREF pRef);

HIMAGEREF LoadImageRef(LPCTSTR szImgPath);
void ReleaseImageRef(HIMAGEREF pRef);
