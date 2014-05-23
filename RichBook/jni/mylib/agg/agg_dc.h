#ifndef _agg_graph_h
#define _agg_graph_h

#include "agg_graph_text.h"
#include "agg_graph_shape.h"
#include "agg_graph_image.h"
#include "agg_svg.h"
#include "agg_brush.h"

class CBrush;
class CPen;


struct AggStatus
{
	COLORREF crClear;
	CPoint ptViewOrigin;
	CRect rcClip;

	PFONT hFont;
	DWORD dwAlignMode;
	COLORREF clrText;
	COLORREF clrTextBg;	//背景色
	BOOL  bTextBgTransparent;			//使用背景色or背景色透明

};

enum EImageTransType
{
	imgTransBilinear,
	imgTransPerspective,
};


//////////////////////////////////////////////////////////////////////////
namespace agg
{
	class CGrayPixFunc
	{
	public:
		virtual rgba8 GetGrayPix(rgba8 cr){
			byte gray = byte((gray_r*cr.r + gray_g*cr.g + gray_b*cr.b) >> gray_shift);
			cr.r = cr.g = cr.b = gray;
			return cr;
		}

		static CGrayPixFunc* Instance(){static CGrayPixFunc inst; return &inst;}
		static const int gray_shift = 8;
		static const int gray_r = int(0.3 * (1 << gray_shift));
		static const int gray_g = int(0.59 * (1 << gray_shift));
		static const int gray_b = int(0.11 * (1 << gray_shift));
	};

	//////////////////////////////////////////
	//				
	// 所有绘制函数均使用逻辑坐标，修改大小使用设备坐标
	class agg_graph
	{
	public:
		agg_graph();  // constructor, set pixel format, background color, log
		~agg_graph();

	public:
		void test();

		// 使用设备坐标，即像素
		void Resize(int nx, int ny);
		void ResizeInLp(int nx, int ny);	//使用逻辑坐标
		CSize GetSize() const ;
		CSize GetLogicalSize() const ;
		void CopyFrom(agg_graph& from,BOOL bCopyImg=FALSE);

		void GetStatus(AggStatus& status);
		void RestoreStatus(const AggStatus& status);
		void PushStatus();
		void PopStatus();

		// 使用memset 更快，适用于用灰度直接填充（白黑）,且不考虑裁剪区
		void ClearWithGray(BYTE by)
		{
			m_agg_base.clear(by);
		}
		void ClearBuffer(COLORREF clr)
		{
			m_render.clear(Clr2Rgba8(clr));
		}
		void SetBackColor(COLORREF cr,bool bClearBuffer=false);
		
		BYTE* GetBuffer() ;


	public:

		// 逻辑坐标与设备坐标的变换,坐标单位，logical unit
		CPoint SetViewOrg(CPoint ptViewOrg) 
		{
			m_bUpdateInLogiValid=false;
			CPoint pt=m_ptViewOrigin; m_ptViewOrigin=ptViewOrg; 
			return pt;
		}
		CPoint GetViewOrg() const { return m_ptViewOrigin; }

		// logical unit --->  device unit
		void Lp2Dp(int& x,int& y) const
		{
			x+=m_ptViewOrigin.x; y+=m_ptViewOrigin.y; 
			x=ScreenLp2Dp(x); y=ScreenLp2Dp(y);
		}
		void Lp2Dp(POINT& pt)  const
		{
			Lp2Dp(pt.x,pt.y); 
		}
		void Lp2Dp(RECT& rc) const
		{	
			Lp2Dp(rc.left,rc.top); Lp2Dp(rc.right,rc.bottom);
		}
		void Lp2DpSize(int& nWidth,int& nHeight)
		{
			Lp2DpSize(nWidth); Lp2DpSize(nHeight); 
		}
		void Lp2DpSize(int& nWidth) 
		{ 
			nWidth=ScreenLp2Dp(nWidth); 
		}
		void Lp2DpPen(CPen* pPen);

		// device unit  ---->  logical unit
		void Dp2Lp(int& x,int& y) const
		{
			x=ScreenDp2Lp(x); y=ScreenDp2Lp(y);
			x-=m_ptViewOrigin.x; y-=m_ptViewOrigin.y; 
		}
		void Dp2Lp(POINT& pt) const 
		{ 
			Dp2Lp(pt.x,pt.y); 
		}
		void Dp2Lp(RECT& rc) const
		{	
			Dp2Lp(rc.left,rc.top); Dp2Lp(rc.right,rc.bottom); 
		}
		void Dp2LpSize(int& nWidth,int& nHeight) 
		{ 
			Dp2LpSize(nWidth); Dp2LpSize(nHeight); 
		}
		void Dp2LpSize(int& nWidth) 
		{
			nWidth=ScreenDp2Lp(nWidth); 
		}

		//////////////////////////////////////////////////////////////////////////
		// 裁减区的函数,均为逻辑坐标
		const CRect& GetUpdateRect()  
		{
			if( !m_bUpdateInLogiValid )
			{
				m_render.GetClipAt(0,m_rcUpdateInLogi);
				Dp2Lp(m_rcUpdateInLogi);
				m_bUpdateInLogiValid=true;
			}
			return m_rcUpdateInLogi;
		}
		void SetUpdateRect(const CRect& rcNewClip)
		{
			CRect rc(rcNewClip);
			Lp2Dp(rc);
			SetUpdateRectInDP(rcNewClip);
		}
		void SetUpdateRectInDP(const CRect& rcNewClip)
		{
			m_render.SetClipRect(rcNewClip);
			m_bUpdateInLogiValid=false;
		}
		BOOL IsVisible(const CRect& rc) 
		{
			const CRect& rcClip=GetUpdateRect();
			CRect rcTemp;
			rcTemp.IntersectRect(&rcClip,&rc);
			return !rcTemp.IsRectEmpty();
		}
		BOOL IsVisible(int x,int y) 
		{
			const CRect& rcClip=GetUpdateRect();
			return rcClip.PtInRect(CPoint(x,y));
		}

		//////////////////////////////////////////////////////////////////////////

		void BlendFrom(agg_graph* pSrcDc,CRect rcSrc,CPoint ptDst,int8u nFactor=255);
		void CopyFrom(agg_graph* pSrcDc,CRect rcSrc,CPoint ptDst);
		void StretchDrawBuffer(	agg_graph* pBufferSrc, 
			int nSrcX, int nSrcY, int nSrcW, int nSrcH, 
			int nDstX, int nDstY, int nDstW, int nDstH);

		////////////////////////////
		// draw the shapes, agg_shape is responsible for the actual work
	public:
		template <class vertexSource> // member function template
		void DrawGlyph(vertexSource& path, 
			CPen* pPen=NULL,
			CBrush* pBrush=NULL);

		// 绘制带有图案的曲线,用图案沿曲线进行绘制
		template <class vertexSource>
		void DrawPatternGlyph(vertexSource& path,HIMAGE hPattern,float fScale=1.0f,float fStart=0.0f);

		void DrawPixelDirectly(int x, int y, rgba8 cr)
		{
			Lp2Dp(x,y);

			m_render.copy_pixel_directly(x,y,cr);
		}

		void Line(POINT ptStart, POINT ptEnd, int nWidth, COLORREF color, bool bEdge = false) 
		{

			Lp2Dp(ptStart);
			Lp2Dp(ptEnd);
			Lp2DpSize(nWidth);

			toolShape.Line(ptStart, ptEnd, nWidth, color, bEdge);
		}
		void Line(POINT ptStart, POINT ptEnd,CPen* pPen);
		void Polyline(LPPOINT vecPoints, int nSize, CPen* pPen);

		void FillSolidRect(RECT rect,COLORREF clr)
		{
			Lp2Dp(rect);
			
			BYTE a = GetAValue(clr);
			if( a==255 )
				m_render.clear_rect(rect.left,rect.top,rect.right-1,rect.bottom-1,Clr2Rgba8(clr));
			else if( a!=0 )
				m_render.blend_rect(rect.left,rect.top,rect.right-1,rect.bottom-1,Clr2Rgba8(clr),a);
		}
		
		void Rectangle(RECT rect, CPen* pPen=NULL, CBrush* crFill = NULL);

		void InvertRect(RECT rect);

		void BlendRect(RECT rect,COLORREF clr,BYTE alpha); 
 
		void RoundRect(RECT rect, POINT ptArc, CPen* pPen=NULL, CBrush* pBrush= NULL);

		void HalfRoundRect(RECT rect, int nWidth, POINT ptArc, int direction,COLORREF crLine, const COLORREF* crFill = NULL)
		{
			Lp2Dp(rect);
			Lp2Dp(ptArc);
			Lp2DpSize(nWidth);

			toolShape.HalfRoundRect(rect, nWidth, ptArc, direction,crLine, crFill);
		}

		void Draw3DRect(RECT rect, int nWidth, COLORREF crLight, COLORREF crDark)
		{
			Lp2Dp(rect);
			Lp2DpSize(nWidth);

			toolShape.Draw3DRect(rect, nWidth, crLight, crDark);
		}
		void Draw3DRoundRect(RECT rect, int nWidth, CPoint ptArc, COLORREF crLight, COLORREF crDark)
		{
			Lp2Dp(rect);
			Lp2Dp(ptArc);
			Lp2DpSize(nWidth);

			toolShape.Draw3DRoundRect(rect, nWidth, ptArc, crLight, crDark);
		}
		void Polygon(const LPPOINT& vecPoints, int nSize, int nWidth, COLORREF crLine, const COLORREF* crFill = NULL)
		{
			Lp2DpSize(nWidth);
			int nBufSize= sizeof(POINT)*nSize;
			LPPOINT ptTemp=(LPPOINT)malloc(nBufSize);
			memcpy(ptTemp,vecPoints,nBufSize);
			for (int i=0;i<nSize;i++)
			{
				Lp2Dp(ptTemp[i]);
			}

			toolShape.Polygon(CRect(0,0,GetSize().cx,GetSize().cy),
				ptTemp, nSize, nWidth, crLine, crFill);

			free(ptTemp);
		}
		void PolygonRound(const LPPOINT& vecPoints, int nSize, int nWidth, COLORREF crLine, const COLORREF* crFill = NULL)
		{
			Lp2DpSize(nWidth);
			int nBufSize= sizeof(POINT)*nSize;
			LPPOINT ptTemp=(LPPOINT)malloc(nBufSize);
			memcpy(ptTemp,vecPoints,nBufSize);
			for (int i=0;i<nSize;i++)
			{
				Lp2Dp(ptTemp[i]);
			}
			toolShape.PolygonRound(CRect(0,0,GetSize().cx,GetSize().cy),
				ptTemp, nSize, nWidth, crLine, crFill);

			free(ptTemp);
		}
		void GradientPolygon(const LPPOINT& vecPoints, int nSize, COLORREF crSide, COLORREF crInner, bool bOnX)
		{
			int nBufSize= sizeof(POINT)*nSize;
			LPPOINT ptTemp=(LPPOINT)malloc(nBufSize);
			memcpy(ptTemp,vecPoints,nBufSize);
			for (int i=0;i<nSize;i++)
			{
				Lp2Dp(ptTemp[i]);
			}
			toolShape.GradientPolygon(ptTemp, nSize, crSide, crInner, bOnX);
			free(ptTemp);
		}
		void DrawPath(path_storage& path, int nStrokeWidth, COLORREF strokeColor, COLORREF* fillColor)
		{
			toolShape.DrawGlyph<path_storage>(path, nStrokeWidth, strokeColor, fillColor);
		}

		void Arc(RECT rcBound, POINT ptStart, POINT ptEnd, int nWidth, COLORREF crLine)
		{
			Lp2Dp(rcBound);
			Lp2Dp(ptStart);
			Lp2Dp(ptEnd);
			Lp2DpSize(nWidth);

			toolShape.Arc(rcBound, ptStart, ptEnd, nWidth, crLine);
		}
		void GlobeArc(RECT rcBound, int nWidth, COLORREF crLine)
		{
			Lp2DpSize(nWidth);
			Lp2Dp(rcBound);

			toolShape.GlobeArc(rcBound, nWidth, crLine);
		}
		void Pie(RECT rect, POINT ptStart, POINT ptEnd, int nWidth, COLORREF crLine, const COLORREF* crFill = NULL)
		{
			Lp2Dp(rect);
			Lp2Dp(ptStart);
			Lp2Dp(ptEnd);
			Lp2DpSize(nWidth);

			toolShape.Pie(rect, ptStart, ptEnd, nWidth, crLine, crFill);
		}
		void Ellipse(RECT rect, CPen* pPen=NULL, CBrush* pBrush = NULL);

		void FillDistortedRect(RECT rect, COLORREF crFill, int nRoundedR = 5, int nDistortedDist = 12)
		{
			Lp2Dp(rect);
			Lp2DpSize(nRoundedR);
			Lp2DpSize(nDistortedDist);

			toolShape.FillDistortedRect(rect, crFill, nRoundedR, nDistortedDist);
		}

		void BubbleRect(RECT rect, CPoint ptFrom, int nWidth, COLORREF crLine, COLORREF crFill)
		{
			Lp2Dp(rect);
			Lp2Dp(ptFrom);
			Lp2DpSize(nWidth);

			toolShape.BubbleRect(rect, ptFrom, nWidth, crLine, crFill);
		}

		//////////////////////////////////////////////////////////////////////////
		// Text Part
		// draw the text, agg_text is responsible the actual work
	public:
		PFONT SetFont(PFONT strFont);
		PFONT GetFont();

		// 对齐模式
		void SetAlignMode(DWORD dwAlignMode) 
		{
			toolText.SetAlignMode(dwAlignMode); 
		}
		DWORD GetAlignMode() const 
		{
			return toolText.GetAlignMode(); 
		}

		// 背景色
		void SetTextBgClr(BOOL bTransparent,COLORREF clrBg)
		{
			toolText.SetTextBgClr(bTransparent,clrBg);
		}
		COLORREF GetTextBgClr() const
		{ 
			return toolText.GetTextBgClr();
		}
		BOOL IsBgTransparent() const
		{
			return toolText.IsBgTransparent(); 
		}

		// 文本颜色
		COLORREF SetTextClr(COLORREF clrText) 
		{
			return toolText.SetTextClr(clrText);
		}
		COLORREF GetTextClr() const 
		{
			return toolText.GetTextClr();
		}


		// 尺寸
		CSize GetTextExtent(
			LPCTSTR lpszString,
			int nCount 
			) 
		{
			CSize sz = toolText.GetTextExtent(lpszString,nCount);
			Dp2LpSize(sz.cx,sz.cy);
			return sz;
		}
		
		CSize DrawChar(int x,int y,unsigned c,
			BOOL bUnderline=FALSE,	// draw underline
			BOOL bStrike=FALSE		// draw strike line
			)
		{
			Lp2Dp(x,y);
			CSize sz= toolText.DrawChar(x,y,c,bUnderline,bStrike);
			Dp2LpSize(sz.cx,sz.cy);
			return sz;
		}

		CSize DrawCharAtBase(PFONT hFont,int x,int y,unsigned c,
			COLORREF clr,BOOL bStrike)
		{
			Lp2Dp(x,y);
			CSize sz=toolText.DrawCharAtBase(hFont,x,y,c,clr,bStrike);
			Dp2LpSize(sz.cx,sz.cy);
			return sz;
		}
		
		void DrawSingleLineText(
			CRect rc,
			LPCTSTR szString,
			int nStrLen,
			BOOL bUnderline=FALSE,	// draw underline
			BOOL bStrike=FALSE		// draw strike line
			)
		{
			Lp2Dp(rc);
			toolText.DrawSingleLineText(rc,szString,nStrLen,bUnderline,bStrike);
		}


		void TextOut(
			int nXStart,       // x-coordinate of starting position
			int nYStart,       // y-coordinate of starting position
			LPCTSTR lpString,  // character string
			int cbString,        // number of characters
			BOOL bUnderline=FALSE,	// draw underline
			BOOL bStrike=FALSE		// draw strike line
			)
		{
			Lp2Dp(nXStart,nYStart);
			toolText.TextOut(nXStart,nYStart,lpString,cbString,bUnderline,bStrike);
		}

		////////////////////////////
		// draw the image, agg_image is responsible the actual work
	public:
		void TransformImage(HIMAGE hImage,CPoint ptDest[4],EImageTransType nType);

		void DrawImageGray(	HIMAGE pImageBuffer, int nSrcX, int nSrcY, int nDstX, int nDstY, int nDstW, int nDstH, bool bMask = false, byte alpha = 255,CGrayPixFunc* pFunc = NULL)
		{
			Lp2Dp(nDstX,nDstY);
			Lp2DpSize(nDstW,nDstH);

			toolImage.DrawImageGray((BYTE*)pImageBuffer, nSrcX, nSrcY, nDstX, nDstY, nDstW, nDstH, bMask, alpha ,CGrayPixFunc::Instance());
		}

		void DrawImageCxPng(HIMAGE pImageBuffer, RECT rcSrc, RECT rcDst,CGrayPixFunc* pFunc,byte alpha = 255)
		{
			Lp2Dp(rcDst);

			toolImage.DrawImageCxPng((BYTE*)pImageBuffer, rcSrc, rcDst, pFunc, alpha);
		}

		void DrawImage(HIMAGE pImageBuffer, CRect rcSrc, CRect rcDst)
		{
			CRect rcClip = GetUpdateRect();
			CRect rcInter;
			// 进行裁剪
			if(rcInter.IntersectRect(&rcDst,&rcClip))
			{
				float fsx=1.0f, fsy=1.0f ;
				if( rcSrc.Width()!=rcDst.Width() )
					fsx = float(rcSrc.Width())/rcDst.Width();
				if( rcSrc.Height()!=rcDst.Height() )
					fsy = float(rcSrc.Height())/rcDst.Height();

				CRect rcFrom;
				rcFrom.left = int( rcSrc.left+(rcInter.left-rcDst.left)*fsx) ;
				rcFrom.top = int(rcSrc.top+(rcInter.top-rcDst.top)*fsy);
				rcFrom.right= int(rcSrc.left+(rcInter.right-rcDst.left)*fsx);
				rcFrom.bottom=int(rcSrc.top+(rcInter.bottom-rcDst.top)*fsy);

				Lp2Dp(rcInter);

				toolImage.DrawImage((BYTE*)pImageBuffer, rcFrom, rcInter);
			}
		}

		void FillPattern(HIMAGE hImgPattern, RECT rcDst,EPatternFillMode mode);

		void RotateDrawImage(HIMAGE pImageBuffer, POINT ptCenter, float fAngle, RECT rcClip, bool bPng32 = true, byte nAlpha = 255)
		{
			Lp2Dp(ptCenter);
			Lp2Dp(rcClip);

			toolImage.RotateDrawImage((BYTE*)pImageBuffer, ptCenter, fAngle, rcClip, bPng32, nAlpha);
		}

		void Gray(CRect rcGray)
		{
			Lp2Dp(rcGray);

			toolImage.Gray(rcGray);
		}

	public:
		void InitSvgCustomColors(char* pBuf, int nLen);
		void SetSvgArrowColor(COLORREF crArrow);
		void SetSvgArrowBoderColor(COLORREF crArrowBoder);
		void SetAvgArrowTailBoderColor(COLORREF crArrowTailBoder);
		void DrawSvgFromFile(char* strPath, bool bNoBackground, bool bIsDay, int nArrowPos, bool bArrowMulti, bool bMea, bool bTunnel);
		void DrawSvgFromBuffer(char* pBuffer, unsigned int nLen, bool bNoBackground, 
			bool bIsDay = true, int nArrowPos = 1, bool bArrowMulti = false, bool bMea = false, bool bTunnel = false, 
			bool bFlag = false, CRect rcBound = CRect(0,0,0,0));
		////////////////////////////
		// clear the buffer
	private:
		agg::rasterizer_scanline_aa<> m_ras;
		agg::scanline_p8              m_slp8;
		agg::scanline_u8			  m_slu8;
		agg::span_allocator<color_type> m_spanalloc;

		////////////////////////////
		// members
	private:

		agg_win32		m_agg_base;
		CClipPixelFmtRender m_render;

		agg_shape toolShape;
		agg_image toolImage;
		agg_text toolText;

		trans_affine	m_matrix;

		COLORREF		m_crClear;

		CPoint	m_ptViewOrigin;	// 逻辑坐标系的原点在设备坐标系中的位置
		CList<AggStatus> m_lStatus;

		//for speed
		CRect m_rcUpdateInLogi ;
		bool m_bUpdateInLogiValid;
	};



}
typedef agg::agg_graph AGGDC;


#endif	