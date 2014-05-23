#pragma once

#include "agg_win.h"

namespace agg
{
	class CGrayPixFunc;
	class agg_image
	{
	public:
		// compared to agg_shape and and agg_text, agg_image has another parameter, "pAggWin32"
		// because all the loaded images are managed by agg_win32
		agg_image(CClipPixelFmtRender* pRender);

	public:
		void Gray(CRect rcGray);
		
		void DrawImageGray(BYTE* pImageBuffer, int nSrcX, int nSrcY, int nDstX, int nDstY, int nDstW, int nDstH, bool bMask, byte alpha,CGrayPixFunc* pFunc = NULL);
		void DrawImage(BYTE* pImageBuffer, const RECT& rcSrc, const RECT& rcDst);

		void DrawImageCxPng(BYTE* pImageBuffer, RECT rcSrc, RECT rcDst, CGrayPixFunc* pFunc, byte nAlpha = 255);

		void RotateDrawImage(BYTE* pImageBuffer, POINT ptCenter, float fAngle, RECT rcClip, bool bPng32 = true, byte nAlpha = 255);

	private:
		bool Clip(CRect rcMem, CRect& rcSrc, CRect& rcDst);

		// for with pallete
		void DrawImageWith8Pal(	BYTE* pBuffer, RGBQUAD* pPal,
			int nBpp,
			int nImageWidth, int nImageHeight, 
			CRect rcSrc, CRect rcDst,
			bool bGray	= false,
			byte nAlpha = 255);

		// for true color, 16,24,32
		void DrawImageInner(	BYTE* pImageBuffer, 
			int nBitsPerPixel, 
			int nImageWidth, int nImageHeight, 
			int nSrcX, int nSrcY, int nSrcW, int nSrcH, 
			int nDstX, int nDstY, int nDstW, int nDstH,
			bool bMask, byte alpha, bool bInvert, 
			CGrayPixFunc* pFunc = NULL,
			byte* pAlpha = 0	);

		struct SColor24 
		{
			byte r; 
			byte g; 
			byte b;
			void color_conv(rgba8& cr) const
			{
				cr.r = b;
				cr.g = g;
				cr.b = r;
				cr.a = 255; //$$
			}
			void color_conv_file(rgba8&cr) const
			{
				cr.r = r;
				cr.g = g;
				cr.b = b;
				cr.a = 255; //$$
			}
		};
		struct SColor555 
		{ 
			unsigned short color; 
			void color_conv(rgba8& cr) const
			{
				cr.r = (color >> 7) & 0xF8;
				cr.g = (color >> 2) & 0xF8; 
				cr.b = (color << 3) & 0xF8; 
				cr.a = 255; //$$
			}

			void color_conv_file(rgba8& cr) const
			{
				cr.r = (color << 3) & 0xF8;
				cr.g = (color >> 2) & 0xF8; 
				cr.b = (color >> 7) & 0xF8; 
				cr.a = 255; //$$
			}
		};
		struct SColor32
		{
			byte b;
			byte g;
			byte r;
			byte a;
			void color_conv(rgba8& cr) const
			{
				cr.r = r;
				cr.g = g;
				cr.b = b;
				cr.a = a;
			}
		};

		template <class TColor>
		bool StretchDrawImage(	BYTE* pSrcBuffer,
								int nImageWidth, int nImageSize,
								int nSrcX, int nSrcY, int nSrcW, int nSrcH, 
								int nDstX, int nDstY, int nDstW, int nDstH, 
								bool bMask, byte alpha,
								bool bInvert = true,
								CGrayPixFunc* pFunc = NULL,
								byte* pAlpha = NULL,
								CSize szImage = CSize(0,0) );

		template <class TColor>
		void CopyImage(	BYTE* pSrcBuffer, 
						int nImageWidth, int nImageSize,
						int nSrcX, int nSrcY, int nSrcW, int nSrcH, 
						int nDstX, int nDstY, int nDstW,
						bool bMask, byte alpha,
						bool Invert = true,
						CGrayPixFunc* pFunc = NULL,
						byte* pAlpha = NULL,
						CSize szImage = CSize(0,0));

	private:
		CClipPixelFmtRender* m_pRender; // will not changed
};
}