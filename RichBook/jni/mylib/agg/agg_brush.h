#pragma once

enum EGradientType
{
	eGradientLinearX,
	eGradientLinearY,
	eGradientRadial,
	eGRadientDiamond,
	eGradientSqrtXY,
	eGradientXY,
	eGradientConic,
};
enum EBrushType
{
	eBrushSolid,
	eBrushGradient,
	eBrushPattern,
};

enum EPatternFillMode
{
	ePatternRepat,
	ePatternReflect,
};
class CBrush
{
private:
	EBrushType mBrushType;
public:

	CBrush(EBrushType brushtype) : mBrushType(brushtype) {}

	virtual ~CBrush(){}

	virtual void SetDestFillRect(const CRect& rc)=0;

	virtual void prepare() = 0;

	virtual void generate(color_type* span, int x, int y, unsigned len) = 0 ;

public:

	static CBrush* CreateSolidBrush(COLORREF color);

	static CBrush* CreatePatternBrush(HIMAGE hImage,EPatternFillMode fillMode);

	static CBrush* CreateGradientBrush(EGradientType eGradType,const color_type* colors);
};

void DeleteBrush(CBrush* &pBrush);


enum EPenStyle
{
	PsSolid,
	PsDash,
	PsDot,
	PsDashDot,
	PsDashDotDot,
};

enum EJoinType
{
	JtMite,
	JtMiteRevert,
	JtRound,
	JtBevel,
	JtMiterRound,
};

namespace agg
{
	class agg_graph;
}
class CPen
{
	friend class agg::agg_graph;
public:

	CPen() : 
	  mnJoinType(JtMite),mnStyle(PsSolid),
		  mnLpWidth(0),mclr(RGB(0,0,0)),mpBrush(NULL) {}

	  CPen(EPenStyle penStyle,int fWidth,COLORREF clr) :
	  mnJoinType(JtMite), mnStyle(penStyle),
		  mnLpWidth(fWidth),mclr(clr),mpBrush(NULL) {}

	  CPen(EPenStyle nPenStyle,int fWidth,CBrush* pBrush) :
	  mnJoinType(JtMite), mnStyle(nPenStyle),
		  mnLpWidth(fWidth),mclr(RGB(0,0,0)),mpBrush(pBrush) {}

	  void SetJoinType(EJoinType nJoinType) { mnJoinType=nJoinType; }
	  void SetWidth(float fw) { mnDpWidth=(int)fw; }
	  void SetStyle(EPenStyle ns) { mnStyle=ns; }
	  void SetBrush(CBrush* pb) { mpBrush=pb; }
	  void SetColor(COLORREF clr) { mclr=clr; }


private:
	EJoinType mnJoinType;
	int mnLpWidth;
	EPenStyle mnStyle;
	COLORREF mclr;
	CBrush* mpBrush;

	int mnDpWidth;
};

