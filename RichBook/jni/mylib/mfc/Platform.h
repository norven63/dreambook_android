#pragma once

//////////////////////////////////////////////////////////////////////////
// 宏定义：以下3个宏都需要在编译环境中进行配置
// WINDOW： 指Windows PC和Win CE平台
//			WIN32: Windows PC平台
//          WINCE:指Win CE（包括Mobile和PND）
// ANDROID: 指Android平台
// IPHONE:  指Iphone平台

#if !defined(WINCE) && defined(_WIN32_WCE)
#define WINCE
#endif

#if !defined(WINDOW) && (defined(WINCE) || defined(WIN32))
#define WINDOW
#endif

// include 
#ifndef WINDOW
	#include <alloca.h>
	#define _DEBUG 
	#ifdef IPHONE
		#include <ctime>
	#else
		#include <time64.h>
	#endif
#endif
#include <cstddef>
#include <string>
#include <vector>
#include <map>
#include <assert.h>
#include <memory>
#include <ctype.h>

// undef
#undef _UNICODE
#undef UNICODE
#undef _T
#undef TRUE
#undef FALSE
#undef _countof
#undef INFINITE
#undef MAX_PATH
#undef INVALID_HANDLE_VALUE

// define
#define  _UNICODE
#define UNICODE
#define _countof(array) (sizeof(array)/sizeof(array[0]))
#define TRUE					(BOOL)1
#define FALSE					(BOOL)0
#define YES						(BOOL)1
#define NO						(BOOL)0
#define INFINITE            0xFFFFFFFF  // Infinite timeout
#define MAX_PATH				255
#define INVALID_HANDLE_VALUE	((HANDLE)(LONG)-1)
#define HANDLE	LPVOID
#define HWND	LPVOID
#define HDC		LPVOID
#define HINSTANCE LPVOID
#define HMODULE LPVOID
// 
#define MAKEWORD(a, b)      ((WORD)(((BYTE)((DWORD)(a) & 0xff)) | ((WORD)((BYTE)((DWORD)(b) & 0xff))) << 8))
#define MAKELONG(a, b)      ((LONG)(((WORD)((DWORD)(a) & 0xffff)) | ((DWORD)((WORD)((DWORD)(b) & 0xffff))) << 16))
#define LOWORD(l)           ((WORD)((DWORD)(l) & 0xffff))
#define HIWORD(l)           ((WORD)((DWORD)(l) >> 16))
#define LOBYTE(w)           ((BYTE)((DWORD)(w) & 0xff))
#define HIBYTE(w)           ((BYTE)((DWORD)(w) >> 8))
#define GET_X_LPARAM(lp)                        ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp)                        ((int)(short)HIWORD(lp))
// 颜色中的A分量255表示完全不透明，0表示完全透明
#define RGBA(r, g, b, a)		((COLORREF)(((BYTE)(b)|((WORD)((BYTE)(g))<<8))| (((DWORD)(BYTE)(r))<<16) | (((DWORD)(BYTE)(a))<<24) ))
#define RGB(r, g, b)			(RGBA(r,g,b,255))
#define GetAValue(rgba)			((BYTE)((rgba & 0xFF000000)>>24))
#define GetRValue(rgba)			((BYTE)((rgba & 0x00FF0000)>>16))
#define GetGValue(rgba)			((BYTE)((WORD(rgba))>>8))
#define GetBValue(rgba)			((BYTE)(byte(rgba)))
#define WINAPI __stdcall

#ifdef WINDOW                                                                              
	#define FOLDER_SEP _T("\\")                                                              
	#define LINE_BREAK _T("\r\n") 
	#define LAST_FOLDER _T("..\\")
#else                                                                                      
	#define FOLDER_SEP _T("/")                                                                 
	#define LINE_BREAK _T("/n")  
	#define LAST_FOLDER _T("../") 
#endif

// 类型重定义
#ifndef IPHONE
	typedef int             BOOL;
	#define max(a,b)    (((a) > (b)) ? (a) : (b))
	#define min(a,b)    (((a) < (b)) ? (a) : (b))
#else
	typedef signed char		BOOL;
	#define __int64 int64
	#define _wtoi wtoi
	#define _istalpha isalpha
	#define _istupper isupper
	#define _istlower islower
	template <class T, class T2> T max(T t1, T2 t2) { return t1 > t2 ? t1 : t2; }
	template <class T, class T2> T min(T t1, T2 t2) { return t1 < t2 ? t1 : t2; }
	template <class T>	T abs(T num) { return (num) > 0 ? (num) : -(num); }
#endif
typedef char				CHAR;		// 1 byte
typedef unsigned short		MCHAR;		// 2 byte
typedef wchar_t				WCHAR;		// maybe 2 or 4 byte
typedef const WCHAR*		LPCWSTR;
typedef const CHAR* 		LPCSTR;
typedef CHAR*				LPSTR;
typedef WCHAR*				LPWSTR;
typedef LPWSTR				BSTR;
typedef unsigned long       DWORD;
typedef DWORD				LCID;
typedef unsigned long		SIZE_T;
typedef unsigned char       BYTE;
typedef unsigned char       UCHAR;
typedef unsigned short      WORD;
typedef float               FLOAT;
typedef long				LPARAM;
typedef int                 INT;
typedef unsigned int        UINT;
typedef unsigned char		byte;
typedef unsigned short      USHORT;
typedef long				LONG ;
typedef long long			int64;
typedef unsigned long long	uint64;
typedef int64				LONGLONG;
typedef uint64				ULONGLONG;
typedef DWORD				COLORREF;
typedef UINT				WPARAM;
typedef long				LPARAM;
typedef long				LRESULT;
typedef signed   short		INT16;
typedef unsigned short		UINT16;

typedef BYTE*				LPBYTE;
typedef UINT*				LPUINT;
typedef void*				LPVOID;
typedef void*				PVOID;
typedef FLOAT               *PFLOAT;
typedef BOOL	           *PBOOL;
typedef BOOL	            *LPBOOL;
typedef BYTE	           *PBYTE;
typedef BYTE	            *LPBYTE;
typedef int		            *PINT;
typedef int		             *LPINT;
typedef WORD	           *PWORD;
typedef WORD	            *LPWORD;
typedef long	            *LPLONG;
typedef DWORD	          *PDWORD;
typedef DWORD	           *LPDWORD;
typedef void	            *LPVOID;
typedef const void	      *LPCVOID;
typedef unsigned int        *PUINT;


#if defined(_UNICODE) || defined(UNICODE)
	typedef WCHAR*			LPTSTR;
	typedef WCHAR			TCHAR;
	typedef const WCHAR* 	LPCTSTR;
	#define _T(x)			L##x
#else
	typedef CHAR*			LPTSTR;
	typedef CHAR			TCHAR;
	typedef const CHAR* 	LPCTSTR;
	#define _T(x)			x
#endif


#ifdef _DEBUG
	#define TRACE	FormatLog
	
	#undef ASSERT
	#define ASSERT(x)				AndroidAssert(x,__FILE__,__LINE__)
		
	#undef VERIFY
	#define VERIFY(x)				assert((x))
	
	#define NOTIMPL(x)	((void)0) //		FormatLog("*****Unimplemented: %s in file (%s:%d)",LPCSTR(x),__FILE__,__LINE__);
#else
	#ifndef _MSC_VER

		#define TRACE	((void)0)		//FormatLog
		
		#undef ASSERT
		#define ASSERT(x)	((void)0)	

		#undef VERIFY
		#define VERIFY(f)	f
		
		#define NOTIMPL(x)	((void)0) //		FormatLog("*****Unimplemented: %s in file (%s:%d)",LPCSTR(x),__FILE__,__LINE__);
	#else
		#define TRACE	__noop

		#undef ASSERT
		#define ASSERT(x)	__noop

		#undef VERIFY
		#define VERIFY(f)	f

		#define NOTIMPL(x)	__noop
	#endif
#endif


// Structure
struct __POSITION {};
typedef __POSITION* POSITION;


typedef struct __SYSTEMTIME {
	WORD wYear;
	WORD wMonth;
	WORD wDayOfWeek;
	WORD wDay;
	WORD wHour;
	WORD wMinute;
	WORD wSecond;
	WORD wMilliseconds;
} SYSTEMTIME, *PSYSTEMTIME, *LPSYSTEMTIME;

typedef struct tagPOINT
{
	int x;
	int y;
} 	POINT;
typedef POINT* PPOINT;
typedef POINT* LPPOINT;

typedef struct tagRECT
{
	int left;
	int top;
	int right;
	int bottom;
} 	RECT;
typedef RECT* PRECT;
typedef RECT* LPRECT;

typedef struct tagSIZE
{
	int cx;
	int cy;
} 	SIZE;
typedef SIZE* PSIZE; 
typedef SIZE* LPSIZE;


// Log系统
void SetLogFile(const wchar_t* sFileName=NULL);
void OutputLog(const char* s);
void OutputLog(const wchar_t* s);
void FormatLog(const char *fmt,...) ;
void FormatLog(const wchar_t *fmt,...) ;
void AndroidAssert(int bIsTrue,const char* szFile,int szLine);

