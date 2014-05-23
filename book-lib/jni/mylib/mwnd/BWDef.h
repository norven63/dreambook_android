#pragma once


//////////////////////////////////////////////////////////////////////////
// macro define

#define ALIGN_LEFT     agg::aline_left
#define ALIGN_RIGHT    agg::aline_right
#define ALIGN_CENTER   agg::aline_h_center
#define ALIGN_TOP      agg::aline_top
#define ALIGN_BOTTOM   agg::aline_bottom 
#define ALIGN_VCENTER  agg::aline_v_center
#define AL_MULTILINE   agg::aline_multiline


//////////////////////////////////////////////////////////////////////////
// enum define

typedef enum RenderEffect
{
	NULLEFFECT = 0,
	NONEEFFECT,
	//slide
	SLIDER_LEFT,
	SLIDER_RIGHT,
	SLIDER_UP,
	SLIDER_DOWN,

	//
	SLIDER_OPEN,
	SLIDER_CLOSE,

	//rotate
	ROTATE_CLOCKWISE,
	ROTATE_COUNTERCLKWISE,

	//fade
	FADE_OUT,

	//
	ZOOM_OPEN,
	ZOOM_CLOSE,

	REVERSE_LAST,
}RENDEREFFECT;

//////////////////////////////////////////////////////////////////////////
// 类类型
class TiXmlElement ;



class IIdle
{
public:
	IIdle() ;
	virtual ~IIdle() ;
	virtual bool OnIdle(long lCount) { return true ; }
};

class IMemLow
{
public:
	IMemLow() ;
	virtual ~IMemLow() ;
	virtual void OnMemLow() {}
};


class CBWTimer;
class CTopWnd;
class CBaseWnd;
class CTimerEngine;
class CResManager;
class CBWMenu;

class BASEWNDSTATUS //: public CNoTrackObject 
{
public:
	// readonly
	CTopWnd*	pTopWnd ;
	CMessageQueue* pMsgQueue;
	CTimerEngine* pTimerEngine;
	CResManager* pResManager;

	// maybe changed
	CBaseWnd*	pFocusWnd ;
	CBaseWnd*	pCaptureWnd ;
	BOOL		bSysIniting ;	//true: CTopWnd或其派生类在初始化阶段

	POINT		ptCurMouse ;	//屏幕坐标
	POINT		ptDragFrom;
	BOOL		bMouseDown;		//鼠标当前处于按下状态
	PFONT		font ;
public:
	BASEWNDSTATUS()
	{
		bSysIniting = TRUE ;
		pFocusWnd = NULL ;
		pCaptureWnd = NULL ;
		pTopWnd = NULL ;
		pTimerEngine = NULL;
		ptCurMouse.x = ptCurMouse.y = 0 ;
	}

	~BASEWNDSTATUS();

};
typedef BASEWNDSTATUS *LPBASEWNDSTATUS;
#define afxWndStatus  GetThreadData()
extern BASEWNDSTATUS& GetThreadData();

//////////////////////////////////////////////////////////////////////////
// 
