#pragma once
#ifndef _WIN_TYPE_H
#define _WIN_TYPE_H

#include "Platform.h"
#include "cstring.h"
#include <time.h>
#include "mSync.h"


class CArchive;

// 单件类
template<class T>
class ISingleton
{
public:
	static T* Instance()
	{
		// double check to avoid problems in multi-thread environment
		static T* volatile pInstance = NULL;
		if(NULL != pInstance)
			return pInstance;
		{
			CMutex cs;
			cs.Lock();
			if(NULL != pInstance)
				return pInstance;
			static T instance;
			pInstance = &instance;
			cs.Unlock();
		}
		return pInstance;		
	}

protected:
	ISingleton(void)	{}
	virtual ~ISingleton(void)	{}	
private:
	ISingleton(const ISingleton&);
	ISingleton& operator=(const ISingleton&);	
};


struct rgba8
{
	rgba8(byte red, byte green, byte blue, byte alpha = 255) : 
r(red), g(green), b(blue), a(alpha) { }

rgba8() : r(0), g(0), b(0), a(255) { }

byte r;
byte g;
byte b;
byte a;
};

enum ENCODING
{
	enUTF8=0,
	enGB2312,
	enUTF16,
	enISO88591,
	enUnknown,
	enMax,
};

class CTimeSpan
{
	enum _CTIMESPANFORMATSTEP
	{
		_CTFS_NONE   = 0,	
		_CTFS_FORMAT = 1,
		_CTFS_NZ     = 2
	};

public:
	CTimeSpan() : m_timeSpan(0){}
	CTimeSpan( int64 time ) : m_timeSpan(time) {}
	CTimeSpan( LONG lDays, int nHours, int nMins, int nSecs ) 
	{
		m_timeSpan = nSecs + 60* (nMins + 60* (nHours + int64(24) * lDays));
	}

	LONGLONG GetDays() const 
	{
		return( m_timeSpan/(24*3600) );
	}

	LONGLONG GetTotalHours() const 
	{
		return( m_timeSpan/3600 );
	}

	LONG GetHours() const 
	{
		return( LONG( GetTotalHours()-(GetDays()*24) ) );
	}

	LONGLONG GetTotalMinutes() const 
	{
		return( m_timeSpan/60 );
	}

	LONG GetMinutes() const 
	{
		return( LONG( GetTotalMinutes()-(GetTotalHours()*60) ) );
	}

	LONGLONG GetTotalSeconds() const 
	{
		return( m_timeSpan );
	}

	LONG GetSeconds() const 
	{
		return( LONG( GetTotalSeconds()-(GetTotalMinutes()*60) ) );
	}

	int64 GetTimeSpan() const 
	{
		return( m_timeSpan );
	}


	CTimeSpan operator+( CTimeSpan span ) const 
	{
		return( CTimeSpan( m_timeSpan+span.m_timeSpan ) );
	}

	CTimeSpan operator-( CTimeSpan span ) const 
	{
		return( CTimeSpan( m_timeSpan-span.m_timeSpan ) );
	}

	CTimeSpan& operator+=( CTimeSpan span ) 
	{
		m_timeSpan += span.m_timeSpan;
		return( *this );
	}

	CTimeSpan& operator-=( CTimeSpan span ) 
	{
		m_timeSpan -= span.m_timeSpan;
		return( *this );
	}

	bool operator==( CTimeSpan span ) const 
	{
		return( m_timeSpan == span.m_timeSpan );
	}

	bool operator!=( CTimeSpan span ) const 
	{
		return( m_timeSpan != span.m_timeSpan );
	}

	bool operator<( CTimeSpan span ) const 
	{
		return( m_timeSpan < span.m_timeSpan );
	}

	bool operator>( CTimeSpan span ) const 
	{
		return( m_timeSpan > span.m_timeSpan );
	}

	bool operator<=( CTimeSpan span ) const 
	{
		return( m_timeSpan <= span.m_timeSpan );
	}

	bool operator>=( CTimeSpan span ) const 
	{
		return( m_timeSpan >= span.m_timeSpan );
	}

public:
	CString Format( LPCTSTR pFormat ) const
	{
		if( pFormat == NULL )
		{
			return _T("");
		}

		CString strBuffer;
		CString hmsFormats [3] = {_T("%c"),_T("%02ld"),_T("%d")};
		CString dayFormats [3] = {_T("%c"),_T("%I64d"),_T("%I64d")};
		TCHAR ch;

		while ((ch = *pFormat++) != _T('\0'))
		{
			enum _CTIMESPANFORMATSTEP formatstep = _CTFS_NONE;
			if(ch == _T('%'))
			{
				formatstep = _CTFS_FORMAT;
				ch = *pFormat++;
				if(ch == _T('#'))
				{
					formatstep = _CTFS_NZ;
					ch = *pFormat++;
				}
			}
			switch (ch)
			{
			case '%':
				strBuffer += ch;
				break;
			case 'D':
				strBuffer.AppendFormat(dayFormats[formatstep], formatstep ? GetDays()    : ch);
				break;
			case 'H':
				strBuffer.AppendFormat(hmsFormats[formatstep], formatstep ? GetHours()   : ch);
				break;
			case 'M':
				strBuffer.AppendFormat(hmsFormats[formatstep], formatstep ? GetMinutes() : ch);
				break;
			case 'S':
				strBuffer.AppendFormat(hmsFormats[formatstep], formatstep ? GetSeconds() : ch);
				break;
			default:
				if(formatstep)
				{
					ASSERT(FALSE);
				}
				else
				{
					strBuffer += ch;
				}
				break;
			}
		}

		return strBuffer;
	}

private:
	int64 m_timeSpan;
};

class CTime
{
public:
	static CTime GetCurrentTime() ;

	CTime()  { m_time = 0; }
	CTime( time_t time )  { m_time = time; }
	CTime( int nYear, int nMonth, int nDay, int nHour, int nMin, int nSec,int nDST = -1 ) ;
	CTime( int nYear, int nMonth, int nDay);
	CTime( const SYSTEMTIME& st, int nDST = -1 ) ;
	

	CTime& operator=( time_t time )  { m_time = time; return *this; }
	CTime& operator+=( CTimeSpan span ) {m_time += span.GetTimeSpan();return( *this );}
	CTime& operator-=( CTimeSpan span ) {m_time -= span.GetTimeSpan();return( *this );}
	CTimeSpan operator-( CTime time ) const {return( CTimeSpan( m_time-time.m_time ) );}
	CTime operator-( CTimeSpan span ) const {return( CTime( m_time-span.GetTimeSpan() ) );}
	CTime operator+( CTimeSpan span ) const {return( CTime( m_time+span.GetTimeSpan() ) );}

	bool operator==( CTime time ) const  { return m_time == time.m_time; }
	bool operator!=( CTime time ) const  { return m_time != time.m_time; }
	bool operator<( CTime time ) const  { return m_time < time.m_time; }
	bool operator>( CTime time ) const  { return m_time > time.m_time; }
	bool operator<=( CTime time ) const  { return m_time <= time.m_time; }
	bool operator>=( CTime time ) const  { return m_time >= time.m_time; }

	operator double()const
	{
		return (double)m_time;
	}

	struct tm* GetGmtTm( struct tm* ptm ) const;
	struct tm* GetLocalTm( struct tm* ptm ) const;
	bool GetAsSystemTime( SYSTEMTIME& st ) const;

	time_t GetTime() const  { return m_time; }
	void SetTime(time_t t) { m_time = t; }

	int GetYear() const ;
	int GetMonth() const  ;
	int GetDay() const  ;
	int GetHour() const ;
	int GetMinute() const;
	int GetSecond() const ;
	int GetDayOfWeek() const;

	void Serialize64(CArchive& ar);

	CString Format(LPCTSTR pFormat) const;
	CString FormatGmt(LPCTSTR pFormat) const;
	
	bool ParseDateTime(LPCTSTR lpszDate, DWORD dwFlags=0, LCID lcid=0)
	{		
		NOTIMPL("ParseDateTime");
		return false;
	}

private:
	time_t m_time;

};

typedef CTimeSpan COleDateTimeSpan;
typedef CTime COleDateTime;


// classes
/////////////////////////////////////////////////////////////////////////////
// Classes declared in this file

//class CSize;
//class CPoint;
//class CRect;

/////////////////////////////////////////////////////////////////////////////
// CSize - An extent, similar to Windows SIZE structure.

class CSize : public SIZE
{
public:

	// Constructors
	// construct an uninitialized size
	CSize()  {}
	// create from two integers
	CSize(int initCX, int initCY) { cx=initCX; cy=initCY; }
	// create from another size
	CSize(SIZE initSize) { cx = initSize.cx; cy = initSize.cy; }
	// create from a point
	CSize(POINT initPt) { cx = initPt.x; cy = initPt.y; }
	// create from a DWORD: cx = LOWORD(dw) cy = HIWORD(dw)
	CSize(DWORD dwSize) { cx = 0xFFFF&dwSize; cy = dwSize>>16; }

	// Operations
	BOOL operator==(SIZE size) { return cx == size.cx && cy == size.cy; }
	BOOL operator!=(SIZE size) const { return cx != size.cx || cy != size.cy; }
	void operator+=(SIZE size) { cx+=size.cx; cy+=size.cy; }
	void operator-=(SIZE size) { cx-=size.cx; cy-=size.cy; }
	void SetSize(int CX, int CY) { cx=CX; cy=CY; }

	// Operators returning CSize values
	CSize operator+(SIZE size) const { return CSize(cx+size.cx, cy+size.cy); }
	CSize operator-(SIZE size) const { return CSize(cx-size.cx, cy-size.cy); }
	CSize operator-() const { return CSize(-cx, -cy); }

	// Operators returning CPoint values
	//	CPoint operator+(POINT point) const { return CPoint(cx+point.x, cy+point.y); }
	//	CPoint operator-(POINT point) const { return CPoint(cx-point.x, cy-point.y); }

	// Operators returning CRect values
	//	CRect operator+(const RECT* lpRect) const { return CRect(lpRect) + *this; }
	//	CRect operator-(const RECT* lpRect) const { return CRect(lpRect) - *this; }

};

/////////////////////////////////////////////////////////////////////////////
// CPoint - A 2-D point, similar to Windows POINT structure.

class CPoint : public POINT
{
public:
	// Constructors

	// create an uninitialized point
	CPoint() {}
	// create from two integers
	CPoint(int initX, int initY) { x=initX; y=initY; }
	// create from another point
	CPoint(POINT initPt) { x=initPt.x; y=initPt.y; }
	// create from a size
	CPoint(SIZE initSize) { x=initSize.cx; y=initSize.cy; }
	// create from an LPARAM: x = LOWORD(dw) y = HIWORD(dw)
	CPoint(DWORD dwPoint) { x=0xFFFF&dwPoint; y=dwPoint>>16; }


	// Operations

	// translate the point
	void Offset(int xOffset, int yOffset) { x+=xOffset; y+=yOffset; }
	void Offset(POINT point) { x+=point.x; y+=point.y; }
	void Offset(SIZE size) { x+=size.cx; y+=size.cy; }
	void SetPoint(int X, int Y) { x=X; y=Y; }

	BOOL operator==(POINT point) const { return x==point.x && y==point.y; }
	BOOL operator!=(POINT point) const { return x!=point.x || y!=point.y; }
	void operator+=(SIZE size) { x+=size.cx; y+=size.cy; }
	void operator-=(SIZE size) { x-=size.cx; y-=size.cy; }
	void operator+=(POINT point) { x+=point.x; y+= point.y; }
	void operator-=(POINT point) { x-=point.x; y-= point.y; }

	// Operators returning CPoint values
	CPoint operator+(SIZE size) const { return CPoint(x+size.cx, y+size.cy); }
	CPoint operator-(SIZE size) const { return CPoint(x-size.cx, y-size.cy); }
	CPoint operator-() const { return CPoint(-x, -y); }
	CPoint operator+(POINT point) { return CPoint(x+point.x, y+point.y); }

	// Operators returning CSize values
	CSize operator-(POINT point) const { return CSize(x-point.x, y-point.y); }

	// Operators returning CRect values
	//CRect operator+(const RECT* lpRect) const { return CRect(lpRect) + *this; }
	//CRect operator-(const RECT* lpRect) const { return CRect(lpRect) - *this; }
};

/////////////////////////////////////////////////////////////////////////////
// CRect - A 2-D rectangle, similar to Windows RECT structure.
class CRect;
typedef const RECT* LPCRECT;
typedef RECT* LPRECT;

class CRect : public tagRECT
{
	
	// Constructors
public:
	// uninitialized rectangle
	CRect() {}
	// from left, top, right, and bottom
	CRect(int l, int t, int r, int b) { left = l; top = t; right = r; bottom = b; }
	// copy constructor
	CRect(const RECT& srcRect) { left=srcRect.left; top = srcRect.top; right = srcRect.right; bottom = srcRect.bottom; }
	// from a pointer to another rect
	CRect(LPCRECT lpSrcRect) { left=lpSrcRect->left; top=lpSrcRect->top; right=lpSrcRect->right; bottom=lpSrcRect->bottom; }
	// from a point and size
	CRect(POINT point, SIZE size) { right = (left = point.x) + size.cx; bottom = (top = point.y) + size.cy; }
	// from two points
	CRect(POINT topLeft, POINT bottomRight) { left = topLeft.x; top = topLeft.y; right = bottomRight.x; bottom = bottomRight.y; }

	// Attributes (in addition to RECT members)

	// retrieves the width
	int Width() const { return right - left; }
	// returns the height
	int Height() const { return bottom - top; }
	// returns the size
	CSize Size() const { return CSize(right - left, bottom - top); }
	// reference to the top-left point
	CPoint& TopLeft() { return *((CPoint*)this); }
	// reference to the bottom-right point
	CPoint& BottomRight() { return *((CPoint*)this+1); }
	// const reference to the top-left point
	const CPoint& TopLeft() const { return *((CPoint*)this); }
	// const reference to the bottom-right point
	const CPoint& BottomRight() const { return *((CPoint*)this+1); }
	// the geometric center point of the rectangle
	CPoint CenterPoint() const { return CPoint((left+right)/2, (top+bottom)/2); }
	//
	CPoint BottomLeft() const { return CPoint(left,bottom); }
	//
	CPoint TopRight() const { return CPoint(right,top); }
	// swap the left and right
	void SwapLeftRight() { SwapLeftRight(LPRECT(this)); }
	static void SwapLeftRight(LPRECT lpRect) { LONG temp = lpRect->left; lpRect->left = lpRect->right; lpRect->right = temp; }

	// convert between CRect and LPRECT/LPCRECT (no need for &)
	//operator LPRECT() { return this; }
	//operator LPCRECT() const { return this; }

	// returns TRUE if rectangle has no area
	BOOL IsRectEmpty() const { return left>=right || top>=bottom; }
	// returns TRUE if rectangle is at (0,0) and has no area
	BOOL IsRectNull() const { return left==0 && top==0 && left>=right && top>=bottom; }
	// returns TRUE if point is within rectangle
	BOOL PtInRect(POINT point) const { return point.x>=left && point.x<right && point.y>=top && point.y<bottom; }

	// Operations

	// set rectangle from left, top, right, and bottom
	void SetRect(int x1, int y1, int x2, int y2) { left=x1; top=y1; right=x2; bottom=y2; }
	void SetRect(POINT topLeft, POINT bottomRight) { left=topLeft.x; top=topLeft.y; right=bottomRight.x; bottom=bottomRight.y; }
	// empty the rectangle
	void SetRectEmpty() { left=right=top=bottom=0; }
	// copy from another rectangle
	void CopyRect(LPCRECT lpSrcRect) { left=lpSrcRect->left; right=lpSrcRect->right; top=lpSrcRect->top; bottom=lpSrcRect->bottom; }
	// TRUE if exactly the same as another rectangle
	BOOL EqualRect(LPCRECT lpRect) const { return left==lpRect->left && right==lpRect->right && top==lpRect->top && bottom==lpRect->bottom; }

	// Inflate rectangle's width and height by
	// x units to the left and right ends of the rectangle
	// and y units to the top and bottom.
	void InflateRect(int x, int y) { left-=x; right+=x; top-=y; bottom+=y;}
	// Inflate rectangle's width and height by
	// size.cx units to the left and right ends of the rectangle
	// and size.cy units to the top and bottom.
	void InflateRect(SIZE size) { InflateRect(size.cx, size.cy); }
	// Inflate rectangle's width and height by moving individual sides.
	// Left side is moved to the left, right side is moved to the right,
	// top is moved up and bottom is moved down.
	void InflateRect(LPCRECT lpRect) { InflateRect(lpRect->left, lpRect->top, lpRect->right, lpRect->bottom); }
	void InflateRect(int l, int t, int r, int b) { left-=l; top-=t; right+=r; bottom+=b; }

	// deflate the rectangle's width and height without
	// moving its top or left
	void DeflateRect(int x, int y) { InflateRect(-x, -y); }
	void DeflateRect(SIZE size) { InflateRect(-size.cx, -size.cy); }
	void DeflateRect(LPCRECT lpRect) { DeflateRect(lpRect->left, lpRect->top, lpRect->right, lpRect->bottom); }
	void DeflateRect(int l, int t, int r, int b) { left+=l; top+=t; right-=r; bottom-=b; }

	// translate the rectangle by moving its top and left
	void OffsetRect(int x, int y) { left+=x; right+=x; top+=y; bottom+=y; }
	void OffsetRect(SIZE size) { OffsetRect(size.cx, size.cy); }
	void OffsetRect(POINT point) { OffsetRect(point.x, point.y); }
	void NormalizeRect()
	{
		LONG temp;
		if (left>right)
		{
			temp = left;
			left = right;
			right = temp;
		}
		if (top>bottom)
		{
			temp = top;
			top = bottom;
			bottom = temp;
		}
	}

	// absolute position of rectangle
	void MoveToY(int y) { LONG h=Height(); top=y; bottom=top+h; }
	void MoveToX(int x) { LONG w=Width(); left=x; right=left+w; }
	void MoveToXY(int x, int y) { MoveToX(x); MoveToY(y); }
	void MoveToXY(POINT point) { MoveToXY(point.x, point.y); }

	// set this rectangle to intersection of two others
	BOOL IntersectRect(LPCRECT lpRect1, LPCRECT lpRect2)
	{
		if( lpRect1->left>= lpRect1->right ||
			lpRect1->top>= lpRect1->bottom ||
			lpRect2->left>= lpRect2->right ||
			lpRect2->top>= lpRect2->bottom )
		{
			SetRectEmpty();
			return FALSE;
		}
		left = lpRect1->left > lpRect2->left ? lpRect1->left : lpRect2->left;
		right = lpRect1->right < lpRect2->right ? lpRect1->right : lpRect2->right;
		top = lpRect1->top > lpRect2->top ? lpRect1->top : lpRect2->top;
		bottom = lpRect1->bottom < lpRect2->bottom ? lpRect1->bottom : lpRect2->bottom;

		return !IsRectEmpty();
	}

	// set this rectangle to bounding union of two others
	BOOL UnionRect(LPCRECT lpRect1, LPCRECT lpRect2)
	{
		if ( ((CRect*)lpRect1)->IsRectEmpty() )
		{
			*this = *lpRect2 ;
		} 
		else if( ((CRect*)lpRect2)->IsRectEmpty() )
		{
			*this = *lpRect1;
		}else
		{
			left = min(lpRect1->left,lpRect2->left);
			right = max(lpRect1->right,lpRect2->right);
			top = min(lpRect1->top,lpRect2->top);
			bottom = max(lpRect1->bottom,lpRect2->bottom);
		}
		return !IsRectEmpty();
	}

	// set this rectangle to minimum of two others
	//BOOL SubtractRect(LPCRECT lpRectSrc1, LPCRECT lpRectSrc2)

	// Additional Operations
	void operator=(const RECT& srcRect) { left=srcRect.left; right=srcRect.right; top=srcRect.top; bottom=srcRect.bottom; }
	BOOL operator==(const RECT& rect) const { return left==rect.left && top==rect.top && right==rect.right && bottom==rect.bottom; }
	BOOL operator!=(const RECT& rect) const { return left!=rect.left || top!=rect.top || right!=rect.right || bottom!=rect.bottom; }
	void operator+=(POINT point) { OffsetRect(point.x, point.y); }
	void operator+=(SIZE size) { OffsetRect(size.cx, size.cy); }
	void operator+=(LPCRECT lpRect) { InflateRect(lpRect); }
	void operator+=(const CRect& Rect) { left+=Rect.left; top+=Rect.top; right+=Rect.right; bottom+=Rect.bottom; }
	void operator-=(POINT point) { OffsetRect(-point.x, -point.y); }
	void operator-=(SIZE size) { OffsetRect(-size.cx, -size.cy); }
	void operator-=(LPCRECT lpRect) { DeflateRect(lpRect); }
	void operator-=(const CRect& Rect) {left-=Rect.left; top-=Rect.top; right-=Rect.right; bottom-=Rect.bottom; }
	void operator&=(const RECT& rect) { CRect rc(rect); this->IntersectRect(this, &rc); }
	void operator|=(const RECT& rect) { CRect rc(rect); this->UnionRect(this, &rc); }

	// Operators returning CRect values
	CRect operator+(POINT point) const { CRect rc(*this); rc+=point; return rc; }
	CRect operator-(POINT point) const { CRect rc(*this); rc-=point; return rc; }
	CRect operator+(LPCRECT lpRect) const {CRect rc(*this); rc+=lpRect; return rc; }
	CRect operator+(const CRect& rect) const {CRect rc(*this); rc+=rect; return rc; }
	CRect operator+(SIZE size) const { CRect rc(*this); rc+=size; return rc; }
	CRect operator-(SIZE size) const { CRect rc(*this); rc-=size; return rc; }
	CRect operator-(LPCRECT lpRect) const { CRect rc(*this); rc-=lpRect; return rc; }
	CRect operator-(const CRect& rect) const {CRect rc(*this); rc-=rect; return rc; }
	CRect operator&(const RECT& rect2) const { CRect rc(*this); rc&=rect2; return rc; }
	CRect operator|(const RECT& rect2) const { CRect rc(*this); rc|=rect2; return rc; }
	//CRect MulDiv(int nMultiplier, int nDivisor) const ;
};



//////////////////////////////////////////////////////////////////////////
//

template<class T> struct TPOINT ;
template<class T> struct TSIZE ;
template<class T> struct TRECT ;

const float PI=3.1415926535f ;
#define FLT_ZERO ((float)1e-8)

template<class T>
struct TPOINT
{
	T x,y ;

	TPOINT(){}
	TPOINT(T fx,T fy){x = fx ; y = fy ;}

	bool operator ==(const TPOINT& pt) const{	return pt.x==x && pt.y==y ;	}
	bool operator !=(const TPOINT& pt) const{	return pt.x!=x || pt.y!=y ;	}
	bool operator<(const TPOINT& pt) const // for stl uses
	{
		if (x < pt.x)
			return true;
		else if (x > pt.x)
			return false;
		else
			return y < pt.y;
	}

	TPOINT& operator=(const TPOINT& pt) { x=pt.x; y=pt.y; return *this ; }
	TPOINT& operator+=(const TPOINT& pt) { x+=pt.x; y+=pt.y; return *this ; }
	TPOINT& operator-=(const TPOINT& pt) { x-=pt.x; y-=pt.y; return *this ; }
	TPOINT& operator*=(T f) { x*=f; y*=f; return *this ; }
	TPOINT& operator/=(T f) { x/=f; y/=f; return *this ; }

	TPOINT operator+(const TPOINT& pt) const { TPOINT ptTemp(*this); ptTemp+=pt; return ptTemp; }
	TPOINT operator-(const TPOINT& pt) const { TPOINT ptTemp(*this); ptTemp-=pt; return ptTemp; }
	TPOINT operator*(T f) const { TPOINT ptTemp(*this); ptTemp*=f; return ptTemp; }
	// 点积
	T operator*(const TPOINT& pt) const { return x*pt.x+y*pt.y; }
	TPOINT operator/(T f) const { TPOINT ptTemp(*this); ptTemp/=f; return ptTemp; }

	void Translate(T fx,T fy) {	x += fx ; y += fy ;	}

	float GetDistToOrigin() const { return (float) sqrt(x*x+y*y); }
	void Normalize()	{ float fLenInv = 1.0f/GetDistToOrigin() ; x=(T)(x*fLenInv); y=(T)(y*fLenInv); }
	bool IsZero() const	{ return fabs(x)<=FLT_ZERO && fabs(y)<=FLT_ZERO ; }

	float GetAngleInRad() 				// 以x轴正向为0度，逆时针增加
	{
		if( float(x<0?-x:x) < 1e-8f )
			return float(y)>=0.0f ? PI*0.5f : PI*1.5f ;

		float fAngle = (float) atan( y/float(x) ) ;

		// ... push to [0, 2PI)
		if( x < 0.0f )
		{
			fAngle += PI ;
		}else if( y<0.0f )
		{
			fAngle += PI*2.0f ;
		}
		return fAngle ;
	}
	float GetAngleInDegree() { return GetAngleInRad()*(180.0f/3.1415926535f); }
	float GetHeadingInRad() 			// 以y轴正向为0度，顺时针增加
	{
		if( y==0.0f )
			return x>0.0f ? PI*0.5f : PI*1.5f ;

		float fAngle = (float) atan( x /float(y) );

		// ... push to [0, 360)
		if( y < 0.0f )
		{
			fAngle += PI;
		}

		if( y > 0 && x < 0 )
		{
			fAngle += 2.0f*PI;
		}
		return fAngle ;
	}
	float GetHeadingInDegree() { return GetHeadingInRad()*(180.0f/3.1415926535f); }
} ;

template<class T>
struct TSIZE
{
	T cx, cy ;

	TSIZE(){}
	TSIZE(const TSIZE& sz){cx=sz.cx;cy=sz.cy;}
	TSIZE(T fx,T fy)	{cx = fx ; cy = fy ;}

	bool operator ==(const TSIZE& sz)	{return sz.cx==cx && sz.cy==cy ;}
	bool operator !=(const TSIZE& sz)	{return sz.cx!=cx && sz.cy!=cy ;}

	TSIZE& operator=(const TSIZE& sz) { cx=sz.cx; cy=sz.cy; return *this ; }
	TSIZE& operator=(const TPOINT<T>& pt) { cx=pt.x; cy=pt.y; return *this ; }
	TSIZE& operator+=(const TSIZE& sz) { cx+=sz.cx; cy+=sz.cy; return *this ; }
	TSIZE& operator-=(const TSIZE& sz) { cx-=sz.cx; cy-=sz.cy; return *this ; }
	TSIZE& operator*=(T f) { cx*=f; cy*=f; return *this ; }
	TSIZE& operator/=(T f) { cx/=f; cy/=f; return *this ; }

	TSIZE operator+(const TSIZE& sz) { TSIZE szTemp(*this); szTemp+=sz; return szTemp; }
	TSIZE operator-(const TSIZE& sz) { TSIZE szTemp(*this); szTemp-=sz; return szTemp; }
	TSIZE operator*(T f) { TSIZE szTemp(*this); szTemp*=f; return szTemp; }
	TSIZE operator/(T f) { TSIZE szTemp(*this); szTemp/=f; return szTemp; }
};

template<class T>
struct TRECT
{
	T x1, y1, x2, y2 ;

	TRECT(){}
	TRECT(T fX1,T fY1,T fX2,T fY2)	{ x1 = fX1 ;y1 = fY1 ;x2 = fX2 ;y2 = fY2 ;}
	TRECT(const TPOINT<T>& ptTopLeft,const TPOINT<T>& ptRightBottom){	x1 = ptTopLeft.x ; y1 = ptTopLeft.y ;x2 = ptRightBottom.x ; y2 = ptRightBottom.y ;}
	TRECT(const TPOINT<T>& ptCenter, T fRadius) : x1(ptCenter.x - fRadius), x2(ptCenter.x + fRadius), y1(ptCenter.y - fRadius), y2(ptCenter.y + fRadius){}

	bool operator ==(const TRECT& rect)const	{return rect.x1==x1 && rect.y1==y1 && rect.x2==x2 && rect.y2==y2 ;}
	bool operator !=(const TRECT& rect)const	{return rect.x1!=x1 || rect.y1!=y1 || rect.x2!=x2 || rect.y2!=y2 ;}

	T Width() const	{ return x2-x1 ;}
	T Height() const{ return y2-y1 ;}	
	TSIZE<T> Size() { return TSIZE<T>(x2-x1,y2-y1); }

	bool IsIntersect(const TRECT& rect) const 
	{
		if (x2<=rect.x1||rect.x2<=x1)	
			return false ;
		if (y2<=rect.y1||rect.y2<=y1)	
			return false ;
		return true;
	}

	bool IsRectInThis(const TRECT& rect) const {return x1<=rect.x1 && rect.x2<x2 && y1<=rect.y1 && rect.y2 < y2 ;}

	bool IsRectEmpty() const { return Width()<=T(0) || Height()<=T(0) ; }
	bool PtInRect(T x,T y) const {	return x>=x1 && x<x2 && y>=y1 && y<y2 ;	}
	bool PtInRect(const TPOINT<T>& pt) const { return PtInRect(pt.x,pt.y) ; }
	bool Contains(T x,T y) const {	return x>=x1 && x<x2 && y>=y1 && y<y2 ;	}
	bool Contains(const TPOINT<T>& pt) const { return PtInRect(pt.x,pt.y) ; }

	void Translate(T fx,T fy)	{x1 += fx ;x2 += fx ;	y1 += fy ;	y2 += fy ;	}
	void Translate(const TPOINT<T>& pt) {	Translate(pt.x,pt.y) ; }
	void Translate(const TSIZE<T>& sz) {	Translate(sz.cx,sz.cy) ; }

	void Inflate(T x,T y){	x1 -= x ; x2 += x ;	y1 -= y ; y2 += y ;	}
	void Inflate(const TSIZE<T>& sz) { Inflate(sz.cx,sz.cy) ; }
	void Inflate(int l, int t, int r, int b) { x1-=l; y1-=t; x2+=r; y2+=b; }
	
	void Deflate(int x, int y) { Inflate(-x, -y); }
	void Deflate(SIZE size) { Inflate(-size.cx, -size.cy); }
	void Deflate(int l, int t, int r, int b) { x1+=l; y1+=t; x2-=r; y2-=b; }
	
	void OffsetRect(int x, int y) { x1+=x; x2+=x; y1+=y; y2+=y; }
	void OffsetRect(TSIZE<T> size) { OffsetRect(size.cx, size.cy); }
	void OffsetRect(TPOINT<T> ptOffset) { OffsetRect(ptOffset.x,ptOffset.y); }

	TPOINT<T> Center() const 
	{
		TPOINT<T> pt ;
		pt.x = (x1+x2)/T(2) ;
		pt.y = (y1+y2)/T(2) ;	
		return pt ;
	}

	TPOINT<T> TopLeft() const 		{ return TPOINT<T>(x1,y1) ; }
	TPOINT<T> TopRight() const 		{ return TPOINT<T>(x2,y1) ; }
	TPOINT<T> BottomLeft() const 	{ return TPOINT<T>(x1,y2) ; }
	TPOINT<T> BottomRight() const 	{ return TPOINT<T>(x2,y2) ; }

	TRECT<T>& UnionRect(const TRECT& r1,const TRECT& r2) 
	{
		x1 = min(r1.x1,r2.x1) ;
		y1 = min(r1.y1,r2.y1) ;
		x2 = max(r1.x2,r2.x2) ;
		y2 = max(r1.y2,r2.y2) ;
		return *this;
	}
	TRECT<T> UnionRect(TRECT<T> other) const {
		if (IsRectEmpty())
			return other;
		if (other.IsRectEmpty())
			return *this;

		T x = min(this->x1, other.x1);
		T y = min(this->y1, other.y1);
		T dx = max(this->x2, other.x2);
		T dy = max(this->y2, other.y2);

		return TRECT<T>(x, y, dx, dy);
	}
	void Normalize() 
	{
		if( x1>x2 )	std::swap(x1,x2) ;
		if( y1>y2 ) std::swap(y1,y2) ;
	}


	void SetRectEmpty() { x1=x2=y1=y2=T(0); }
	void SetEmpty() { x1=x2=y1=y2=T(0); }
	void Set(T fx1,T fy1,T fx2,T fy2) { x1=fx1;x2=fx2;y1=fy1;y2=fy2; }

	void MoveToY(T y) { T h=Height(); y1=y; y2=y1+h; }
	void MoveToX(T x) { T w=Width(); x1=x; x2=x1+w; }
	void MoveToXY(T x, T y) { MoveToX(x); MoveToY(y); }
	void MoveToXY(TPOINT<T> point) { MoveToXY(point.x, point.y); }

	 TRECT<T> IntersectRect(const TRECT<T>& other) const 
	 {
		 TRECT<T> temp;
		 temp.IntersectRect(*this,other);
		 return temp;
	 }
	void IntersectRect(const TRECT& r1,const TRECT& r2)
	{
		x1 = max(r1.x1,r2.x1) ;
		x2 = min(r1.x2,r2.x2) ;
		y1 = max(r1.y1,r2.y1) ;
		y2 = min(r1.y2,r2.y2) ;

		if( x1 >= x2 || y1>=y2 )
		{
			SetRectEmpty() ;
		}
	}

#ifndef FLT_EPSILON
#define FLT_EPSILON 1.192092896e-07f
#endif
	TRECT<int> Round() const {
		return TRECT<int>((int)floor(x1 + FLT_EPSILON),
			(int)floor(y1 + FLT_EPSILON),
			(int)ceil(x2 - FLT_EPSILON),
			(int)ceil(y2 - FLT_EPSILON));
	}
	template <typename S>
	TRECT<S> Convert() const {
		return TRECT<S>((S)x1, (S)y1, (S)x2, (S)y2);
	}
};

typedef TPOINT<float>	POINTFLT ;
typedef TPOINT<double> POINTDBL ;
typedef TPOINT<double> POINTD ;
typedef TPOINT<char> POINTCHAR ;

typedef TSIZE<int>		SIZEI;
typedef TSIZE<float>	SIZEFLT ;

typedef TRECT<int>		RECTI;
typedef TRECT<float>	RECTF ;
typedef TRECT<double>	RECTD ;

typedef POINTFLT* LPPOINTFLT ;
typedef RECTF* LPRECTF ;
typedef SIZEFLT* LPSIZEFLT ;

typedef TPOINT<int>	POINTINT ;
typedef TSIZE<int>	SIZEINT ;
typedef TRECT<int>	RECTINT ;

typedef POINTINT* LPPOINTINT ;
typedef RECTINT* LPRECTINT ;
typedef SIZEINT* LPSIZEINT ;


#endif

