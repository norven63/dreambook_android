#pragma once
#include <map>

class CFont;
typedef CFont* PFONT;
struct STextMetrics;
struct LOGFONT;

PFONT CreateFont(LPCTSTR szFontName,int nSize,BOOL bBold,BOOL bItalic,BOOL bAntiAlias);
PFONT CreateFont(const LOGFONT& font);
void DeleteFont(PFONT pFont);
PFONT GetDefaultFont();
BOOL GetTextMetrics(PFONT,STextMetrics*);


enum ETextAlign
{
	TA_NOUPDATECP=0,
	TA_UPDATECP=1,

	TA_LEFT	=0,
	TA_RIGHT=2,
	TA_CENTER=6,

	TA_TOP=0,
	TA_BOTTOM=8,
	TA_BASELINE=24,

	TA_VCENTER=32,
};

struct STextMetrics
{
	int ascender;
	int descender;
	int height;				//比ascender+descender要大
	int nExtra;				// (height-(ascender-stm.descender))/2
	int max_advance_width;
	int max_advance_height;
	int underline_position;	// 以baseline为基准，此值通常是负值
	int underline_thickness;

	int GetLineHeight() const	//整行的高度(包括行的前后间距)
	{
		return height;
	}
	int GetDistBase2Bottom() const //基线到行的底部
	{
		return nExtra-descender;
	}
};

struct SGlyphCache
{
	unsigned        glyph_index;
	BYTE*          data;
	unsigned        data_size;
	int			left, top ;
	int			width, height;
	int          advance_x;
	int          advance_y;
};


class CFontCache
{
private:
	friend class CFont;
	friend class CFaceInfo;

	CFontCache()
	{
		memset(m_glyphs, 0, sizeof(m_glyphs));
	}
	~CFontCache() { Close(); }

	SGlyphCache* FindGlyph(unsigned glyph_code) const
	{
		unsigned msb = (glyph_code >> 8) & 0xFF;
		if(m_glyphs[msb]) 
		{
			return m_glyphs[msb][glyph_code & 0xFF];
		}
		return 0;
	}

	//--------------------------------------------------------------------
	SGlyphCache* CacheGlyph(unsigned        glyph_code);

	void Close();

	SGlyphCache**   m_glyphs[256];
};

class CFaceInfo;
namespace agg
{	
	class agg_text;
}

class CFont
{
	friend class agg::agg_text;
private:
	// 字体内部都使用设备坐标
	CFaceInfo* m_pFaceInfo;
	CFontCache m_fontCache;
	BOOL	m_bBold;
	BOOL	m_bItalic;
	BOOL	m_bAntiAlias;
	BOOL	m_bLCDMode;
	int		m_nSize;		// in device unit (pixel)
	int		m_nLogiSize;	//

	int m_nChineseWidth;	// in device unit (pixel)

	STextMetrics m_tm;
public:
	CFont(CFaceInfo* pFace,int nSize,
		BOOL bBold,BOOL bItalic,
		BOOL bAntiAlias,BOOL bLCDMode=FALSE);
	~CFont();

	CFont* CreateSizeFont(int nNewSize,BOOL bBold,BOOL bItalic);

	int GetCharWidth() 
	{
		return ScreenDp2Lp(m_nChineseWidth); 
	}

	// 获取字形的glyph，会缓存字形加快速度
	SGlyphCache* GetGlyph(unsigned nCharCode,unsigned nDefaultOnNotExist=L' ');
	
	BOOL IsLCDMode() const { return m_bLCDMode; }
	
	void GetTextMetrics(STextMetrics& tm,BOOL bInDeviceUnit=TRUE) const 
	{
		tm=m_tm; 

		if ( !bInDeviceUnit )
		{
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// 以下返回尺寸单位 logical unit

	CSize GetCharSize(unsigned nCharCode) 
	{
		int nCharWidth = 0 ;
		if ( nCharCode>=0x4e00 && nCharCode<=0x9fa5 )	
			//认为汉字都是等宽字符
		{
			nCharWidth = m_nChineseWidth;
		}else
			nCharWidth = GetGlyph(nCharCode)->advance_x;

		return CSize(ScreenDp2Lp(nCharWidth),
			ScreenDp2Lp(m_tm.height));
	}
	int GetDistBase2Bottom() const // 基线到行的底部
	{
		return ScreenDp2Lp(m_tm.GetDistBase2Bottom());
	}
	int GetLineHeight() const // 
	{
		return ScreenDp2Lp(m_tm.GetLineHeight());
	}

	void CalcCaret(WCHAR c,int& nBase/*in-out*/,int& nHeight/*out*/);
};


struct LOGFONT
{
	CString sFontName;
	int nCharSize;
	BYTE bBold;
	BYTE bItalic;
	BYTE bUnderline;
	BYTE bDelLine;
	BYTE bAntiAlias;

	LOGFONT() {}
	LOGFONT(LPCTSTR szFontName,int nSize,BYTE _bBold,BYTE _bItalic=FALSE,
		BYTE _bUnderline=FALSE,BYTE _bDelLine=FALSE,BYTE _bAntiAlias=TRUE);

	bool operator<(const LOGFONT& attr) const;
	bool operator==(const LOGFONT& attr) const;

	void Serialize(CArchive& ar);
};
