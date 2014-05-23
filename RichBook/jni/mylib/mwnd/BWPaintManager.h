#pragma once

enum emColorIndex
{
	emClrBorder = 0 ,

	//������
	emClrSBarBorder ,		//�߿���ɫ
	emClrSBarTrackStart ,	//����ɫ�����ɫ
	emClrSBarTrackEnd ,		//�յ���ɫ
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

	emClrBtnBgPressed,     //��ť����ʱ�ı�����ɫ
	emClrBtnTrans,         //��ťͼƬ��͸��ɫ

	emClrMax ,
};

enum emSizeIndex
{
	emSizeBorder ,			// frame�ı߿�����resize
	emSizeCaption ,			// frame�������߶�
	emSizeCaptionTBar ,		// toolbar��frame�ı������߶�

	//ϵͳ��ť��С
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
