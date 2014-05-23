#pragma once
#ifndef AGG_SVG_MANAGER_H
#define AGG_SVG_MANAGER_H

#include "svg/agg_svg_path_renderer.h"
#include "svg/agg_svg_parser.h"


namespace agg 
{
	class agg_svg
	{
	public:
		agg_svg(CClipPixelFmtRender* pRender);

	public:
		bool	DrawSvgFromFile(char* svgFullPath, bool bNoBackground, bool bIsDay, int nArrowPos, bool bArrowMulti, bool bMea, bool bTunnel);
		bool	DrawSvgFromBuffer(char* svgBuffer, unsigned int nLen, 
			bool bNoBackground, bool bIsDay, int nArrowPos, bool bArrowMulti, bool bMea, bool bTunnel, 
			bool bFlag, CRect rcBound);
		void	SetFont(const char* font) { tcscpy(m_font, font); }
		float	get_sign_top(char* pBufferOrPath, unsigned int nLen);
		void	SetFlag(bool bFlag) { m_bFlag = bFlag; }

	private:
		agg_svg(){}
		void	CalcMatrix(RECT rcBox);
		void	draw_gradient_rect(svg::parser::SGradientRect& rect);
		void	draw_gradient(std::vector<svg::parser::SGradientRect>& vecGradient);
		void	draw_shapes(svg::path_renderer& path);
		void	draw_texts(const wchar_t* str, float fSize, rgba8 crText, trans_affine& strMatrix);
		void	draw_texts(const wchar_t* str, float x, float y, float fSize, rgba8 crText);
		bool	draw_svg(char* pBufferOrPath, unsigned int nLen, bool bIsDay, int nArrowPos, bool bArrowMulti, bool bMea, bool bTunnel);

	private: 
		bool					m_bNoBackground;
		bool					m_bFlag;
		CRect					m_rcBound;
		CRect					m_rcSvgBound;
		agg::trans_affine		m_matrix;
		CClipPixelFmtRender*	m_pBuffer;
		CClipPixelFmtRender*	m_pRender;
		char					m_font[512];
	};
}

#endif