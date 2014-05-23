#pragma once

#include "agg_win.h"

namespace agg 
{
	class agg_graph;

	class agg_shape
	{
	public:
		// constructor, it should get the buffer whose pixel format should be one assigned by the template parameter
		// of pixel_format, and it should get the transformation matrix to transform the drawing objects
		agg_shape(CClipPixelFmtRender& prender);

	public:
		// draw the line from "ptStart" to "ptEnd", user should provide line's width, color, whether the line has edge
		void Line(const POINT& ptStart, const POINT& ptEnd, int nWidth, COLORREF color, bool bEdge);


		// draw the arc
		void Arc(const RECT& rcBound, POINT& ptStart, POINT& ptEnd, int nWidth, COLORREF crLine);

		void GlobeArc(const RECT& rcBound, int nWidth, COLORREF crLine);

		// draw the ellipse, if "crFill" is not null, the ellipse will be filled with the color of "*crFill"
		void Ellipse(const RECT& rect, int nWidth, COLORREF crLine, const COLORREF* crFill);

	public:	
		// draw the rectangle, if "crFill" is not null, the ellipse will be filled with the color of "*crFill"
		void Rectangle(const RECT& rect, int nWidth, COLORREF crLine, const COLORREF* crFill);


		// draw the round rectangle, "ptArc" is the arc's radius in X and Y
		void RoundRect(const RECT& rect, int nWidth, POINT ptArc, COLORREF crLine, const COLORREF* crFill);
		void HalfRoundRect(const RECT& rect, int nWidth, POINT ptArc, int direction,COLORREF crLine, const COLORREF* crFill);

		void GradientHalfRoundRect(const RECT& rect, POINT ptArc, float fMidPos, int direction,COLORREF crStart, COLORREF crMid, COLORREF crEnd);

		void Draw3DRect(const RECT& rect, int nWidth, COLORREF crLight, COLORREF crDark);
		void Draw3DRoundRect(const RECT& rect, int nWidth, CPoint ptArc, COLORREF crLight, COLORREF crDark);

		// draw the polygon
		void Polygon(const RECT& rect,const LPPOINT& vecPoints, int nSize, int nWidth, COLORREF crLine, const COLORREF* crFill);
		void PolygonRound(const RECT& rect,const LPPOINT& vecPoints, int nSize, int nWidth, COLORREF crLine, const COLORREF* crFill);
		void GradientPolygon(const LPPOINT& vecPoints, int nSize, COLORREF crFillSide, COLORREF crFillinner, bool bOnX); //x direction
		void PolyPolygon(LPPOINT vecPoints, int* vecSize, int nPolyCount, COLORREF crFill, int nEdge, COLORREF crEdge);
		void PolyPolygon(LPPOINT* vecPoints, int* vecSize, int nPolyCount, COLORREF crFill, int nEdge, COLORREF crEdge);

		// the the pie, if "crFill" is null, the pie will not filled, attention, in this case, it is not an arc either
		void Pie(const RECT& rect, POINT& ptStart, POINT& ptEnd, int nWidth, COLORREF crLine, const COLORREF* crFill);

		void FillDistortedRect(const RECT& rect, COLORREF crFill, int nRoundedR, int nDistortedDist);
		void GradientCircle(int x, int y, int r, COLORREF crStart, COLORREF crEnd, int hX, int hY, CRect& rcClip);
		void BubbleRect(const RECT& rect, CPoint ptFrom, int nWidth, COLORREF crLine, COLORREF crFill);

		bool validate_rect(const RECT& rcIn, RECT& rcOut);

	public:
	
		void PolyPolyDashLine(	LPPOINT vecPoints, 
								std::vector<int>& vecSize,  
								int nPatternWidth, 
								int nPattenLen1, 
								int nPattenLen2, 
								int nPattenLen3, 
								int nPattenLen4, 
								COLORREF crPatternColor1, 
								COLORREF crPatternColor2, 
								int nPatternEdgeWidth, 
								COLORREF crPatterEdgeColor1,
								COLORREF crPatterEdgeColor2	) ;

	private:
		void Line(int x0, int y0, int x1, int y1, int nWidth,const rgba8& color, rgba8* crBack);
		void LineVerti(int x0, int y0, int y1, int nWidth,const rgba8& cr, rgba8* crBack);
		void LineHori(int x0, int y0, int x1, int nWidth,const rgba8& cr, rgba8* crBack);
		void ScanHori(int x, int y, int len,const rgba8& cr, rgba8* crBack);
		void ScanVerti(int x, int y, int len,const rgba8& cr, rgba8* crBack);

		void ConstructRailImage(byte* pBuffer, 
								int nPatternWidth, 
								int nPattenLen, 
								COLORREF crPatternColor1, 
								COLORREF crPatternColor2, 
								int nPatternEdgeWidth, 
								COLORREF crPatterEdgeColor1,
								COLORREF crPatterEdgeColor2	);
		void ConstructDashLineImage(	byte* pBuffer, 
										int nPatternWidth, 
										int nPattenLen1, 
										int nPattenLen2,
										int nPattenLen3,
										int nPattenLen4,
										COLORREF crPatternColor1, 
										COLORREF crPatternColor2, 
										int nPatternEdgeWidth, 
										COLORREF crPatterEdgeColor1,
										COLORREF crPatterEdgeColor2	);


		void ConstructPolypolylinePath(path_storage& path, LPPOINT vecPoints, std::vector<int>& vecSize);
		void ConstructPolypolylinePath(path_storage& path, LPPOINT* vecPoints, std::vector<int>& vecSize);

		void ConstructPolyPolygonPath(path_storage& path, LPPOINT vecPoints, int* vecSize, int nPolyCount);
		void ConstructPolyPolygonPath(path_storage& path, LPPOINT* vecPoints, int* vecSize, int nPolyCount);

		void DrawPolypolyPattern(	path_storage& path, 
									int nPatternWidth, 
									int nPattenLen, 
									COLORREF crPatternColor1, 
									COLORREF crPatternColor2, 
									int nPatternEdgeWidth, 
									COLORREF crPatterEdgeColor1,
									COLORREF crPatterEdgeColor2	);

		void DrawPolypolyDashLine(	path_storage& path, 
									int nPatternWidth, 
									int nPattenLen1, 
									int nPattenLen2,
									int nPattenLen3, 
									int nPattenLen4, 
									COLORREF crPatternColor1, 
									COLORREF crPatternColor2, 
									int nPatternEdgeWidth, 
									COLORREF crPatterEdgeColor1,
									COLORREF crPatterEdgeColor2	) ;
	public:
		// draw polyline, which vertexes is contained by path,  and polygon if "crFill" is not null
		template <class vertexSource>
		void DrawGlyph(vertexSource& path, int nWidth, COLORREF crLine, const COLORREF* crFill);

	private:
		CClipPixelFmtRender& m_render; //will not changed
	};
}