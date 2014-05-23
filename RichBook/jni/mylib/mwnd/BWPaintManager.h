#pragma once

enum emColorIndex
{
	emClrBorder = 0 ,

	//滚动条
	emClrSBarBorder ,		//边框颜色
	emClrSBarTrackStart ,	//渐变色起点颜色
	emClrSBarTrackEnd ,		//终点颜色
	emClrSBarTrackDownStart ,
	emClrSBarTrackDownEnd ,
	emClrSBarThumb ,
	emClrSBarThumbBorder ,
	emClrSBarArrowStart ,
	emClrSBarArrowEnd ,
	emClrSBarArrowTriangle ,

	emClrMsgBg ,
	emClrMsgBorder ,
	emClrMsgTextBg ,
	emClrMsgTextClr ,

	emClrPopBg ,
	emClrPopTextClr ,
	emClrPopBorder ,

	emClrListCtrlBgFrom ,
	emClrListCtrlBgTo ,

	emClrBtnBgPressed,     //按钮按下时的背景颜色
	emClrBtnTrans,         //按钮图片的透明色

	emClrMax ,
};

enum emSizeIndex
{
	emSizeBorder ,			// frame的边框，用于resize
	emSizeCaption ,			// frame标题栏高度
	emSizeCaptionTBar ,		// toolbar的frame的标题栏高度

	//系统按钮大小
	emSizeBtnCX ,
	emSizeBtnCY ,

	emSizeMax ,
};

enum emFontIndex
{
	emFontNormal ,
	emFontEditField ,
	emFontLabel ,
	emFontMenu,
	emFont ,
	emFontMax ,
};

enum emBmpIndex
{
	emBmpMax ,
};

class CBWPaintManager 
{
	BOOL Init();
	void Close();
public:
	CBWPaintManager(void);
	~CBWPaintManager(void);
	


	COLORREF GetColor(int nIndex) ;
	int		 GetMetric(int nIndex) ;
	HFONTREF GetFont(int nIndex) ;

	void	 ReInitFont() ;

	// public
	static CBWPaintManager& GetPainterManager() ;

protected:
	COLORREF	m_clrs[emClrMax] ;
	int			m_nsizes[emSizeMax] ;
	HFONTREF	m_fonts[emFontMax] ;

};


//////////////////////////////////////////////////////////////////////////
// inline member function

inline COLORREF CBWPaintManager::GetColor(int nIndex) 
{
	ASSERT(nIndex>=0 && nIndex<emClrMax) ;
	return m_clrs[nIndex] ;
}
inline int CBWPaintManager::GetMetric(int nIndex) 
{
	ASSERT(nIndex>=0 && nIndex<emSizeMax) ;
	return m_nsizes[nIndex] ;
}
inline HFONTREF	 CBWPaintManager::GetFont(int nIndex) 
{
	ASSERT(nIndex>=0 && nIndex<emFontMax) ;
	return m_fonts[nIndex] ;
}
