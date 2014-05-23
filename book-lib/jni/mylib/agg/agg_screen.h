#pragma once

#include "agg_dc.h"

class CScreen
{
protected:
	AGGDC mAggDc;
	CSize mSize;
	CRect mClipRect;

	CMutex m_mutex;
	CEvent m_hUpdate;
	BOOL m_bEnableDrawToScreen;

public:
	CScreen() :
	  mSize(0,0),
	  mClipRect(0,0,0,0),
	  m_bEnableDrawToScreen(TRUE),
	  m_hUpdate(true)
	{}

	void InitScreen(int cx,int cy)
	{
		CAutoLock lock(GetMutex());
		if( cx!=0 && cy!=0 )
		{
			mSize = CSize(cx,cy);
			mAggDc.Resize(cx,cy);
		}
	}
	CMutex& GetMutex() 
	{
		return m_mutex;
	}

	// 获取屏幕宽度和高度
	CSize GetSize()  { CAutoLock _lock(m_mutex);	return mSize;}

	CRect GetClipRect()  { return mClipRect; }

	void  ClearClipRect() { mClipRect.SetRectEmpty(); }

	AGGDC* GetScreenDc()
	{
		m_mutex.Lock();
		return &mAggDc;
	}

	void ReleaseDc(AGGDC* pDC)
	{
		m_mutex.Unlock();
	}
	// 将屏幕dc中的指定区域更新到真正的屏幕上
	void UpdateToScreen(const RECT* rcUpdate)
	{
		CSize sz = mAggDc.GetSize();
		DrawToRealScreen(mAggDc.GetBuffer(),sz.cx,sz.cy,rcUpdate);
	}

	void Invalidate(const RECT& rcRefresh)
	{
		mClipRect.UnionRect(&mClipRect,&rcRefresh);
		XPostMessage(NULL,WM_PAINT,0,0);
	}

	BOOL IsEnableDraw() { CAutoLock lock(m_mutex); return m_bEnableDrawToScreen; }
	BOOL EnableDrawToScreen(BOOL bEnableDraw)
	{
		BOOL bOld ;
		CAutoLock lock(m_mutex);
		bOld = m_bEnableDrawToScreen;
		m_bEnableDrawToScreen=bEnableDraw;
		return bOld;
	}
	void NotifyDrawFinish()
	{
		m_hUpdate.SetEvent();
	}


	//////////////////////////////////////////////////////////////////////////
	// 虚函数
	// 
	virtual BOOL SetCapture() { return TRUE; }

	virtual void ReleaseCapture() {}

	virtual void Recenter() {}

	// 绘制一个buffer到真正的屏幕上，cx,cy为宽度和高度，pRcSrc为要绘制的区域
	// 函数为同步绘制函数，绘制完成后返回,需要检测是否允许绘制
	// 每个平台有自己的实现
	virtual void DrawToRealScreen(BYTE* pImgBuf,int cx,int cy,const RECT* pRcSrc){}

};

// 获取屏幕参数，不同平台有自己的实现
extern CScreen& GetScreen();

