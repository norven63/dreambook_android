#pragma once
#include "agg_win.h"
#include "agg_font.h"

namespace agg
{	
	class agg_text
	{
	public:
		// same logic as agg_shape
		agg_text(CClipPixelFmtRender* pRender, PFONT strFont);
		~agg_text(){}

		void CopyFrom(const agg_text& from);

		// ��ǰ����
		PFONT SetFont(PFONT strFont) 
		{
			if( strFont!=m_font )
			{
				PFONT hOld=m_font ;
				if(strFont==NULL)
					m_font=GetDefaultFont();
				else
					m_font = strFont; 

				m_font->GetTextMetrics(m_stm);
				return hOld;
			}
			return m_font;
		}
		PFONT GetFont() { return m_font; }

		// ����ģʽ
		void SetAlignMode(DWORD dwAlignMode) { m_dwAlignMode=dwAlignMode; }
		DWORD GetAlignMode() const { return m_dwAlignMode; }

		// ����ɫ
		void SetTextBgClr(BOOL bTransparent,COLORREF clrBg) {m_bBgTransparent=bTransparent; m_clrBg=Clr2Rgba8(clrBg);}
		COLORREF GetTextBgClr() const { return Rgba82Clr(m_clrBg); }
		BOOL IsBgTransparent() const { return m_bBgTransparent; }

		//
		COLORREF SetTextClr(COLORREF clrText) 
		{
			COLORREF oldClr = GetTextClr();
			m_clrText=Clr2Rgba8(clrText); 
			return oldClr;
		}
		COLORREF GetTextClr() const { return Rgba82Clr(m_clrText); }

		//////////////////////////////////////////////////////////////////////////
		// text extent
		CSize GetTextExtent(
			LPCTSTR lpszString,
			int nCount 
			) ;

		//////////////////////////////////////////////////////////////////////////
		// text output

		CSize DrawChar(int x,int y,unsigned c,
			BOOL bUnderline=FALSE,	// draw underline
			BOOL bStrike=FALSE		// draw strike line
			);

		// ���ڸ߼������Ű�
		CSize DrawCharAtBase(PFONT hFont,int x,int y,unsigned c,COLORREF clr,BOOL bStrike);

		void DrawSingleLineText(
			const CRect& rc,
			LPCTSTR szString,
			int nStrLen,
			BOOL bUnderline=FALSE,	// draw underline
			BOOL bStrike=FALSE		// draw strike line
			);

		// ����ָ���Ŀ�����ܹ���ʾ���ַ���
		// dwFmt:DT_WORDBREAK / 
		//int CalcDisplayedCharCountInWidth(int nWidth,
		//	LPCTSTR szString,DWORD dwFmt);

		void TextOut(
			int nXStart,       // x-coordinate of starting position
			int nYStart,       // y-coordinate of starting position
			LPCTSTR lpString,  // character string
			int cbString,        // number of characters
			BOOL bUnderline=FALSE,	// draw underline
			BOOL bStrike=FALSE		// draw strike line
			);

	private:
		agg_text() {}

	private:
		CClipPixelFmtRender* m_pRender; // will not changed

		PFONT m_font;
		DWORD m_dwAlignMode;
		rgba8 m_clrText;
		rgba8 m_clrBg;	//����ɫ
		BOOL m_bBgTransparent;			//ʹ�ñ���ɫor����ɫ͸��

		STextMetrics m_stm;	// ��ǰ���������
	};
}