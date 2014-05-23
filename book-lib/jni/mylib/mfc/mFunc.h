/********************************************************************
	created:	2010/07/08
	created:	8:7:2010   14:26
	file base:	mFunc
	file ext:	h
	author:		zhenglibao
	
	purpose:	提供Windows函数在Android平台的实现
*********************************************************************/
#pragma once

#include "cstring.h"
#include "mCollection.h"

BOOL IsFloatEqual(float f1,float f2);
//////////////////////////////////////////////////////////////////////////
// file directory
BOOL	IsFileExist(LPCTSTR lpszFilePath) ;
BOOL	IsFolderExist(LPCTSTR lpszFilePath);
BOOL    IsFileFolderExist(LPCTSTR lpszFilePath);
BOOL	IsRelativePath(CString sPath);
BOOL	IsAbsolutePath(CString sPath);
inline bool IsDriverDisk(CString s)
{
	return s.GetLength()==2 && s[1]==L':';
}
CString GetFileNameFromPath(CString sPath);
CString GetFolderFromPath(CString sPath);	// 获取文件所在的文件夹
CString GetFileExtNameFromPath(CString sPath);
CString GetDirectoryFromPath(CString sAbsFilePath);
void SplitPath2Vec(CString sPath,CStringArray& saVec);
CString AbsoluteToRelativePath(CString sDir,
							   CString sFile);
CString RelativeToAbsolutePath(CString sDir,
							   CString sRelFile);


BOOL	XCreateDirectory(LPCTSTR lpPathName,LPVOID pSec=NULL);
BOOL	XDeleteFile(LPCTSTR lpFileName);

CString GetExePath() ;
void SetExePath(LPCTSTR szExePath);

CStringA EncodeURL(const CStringA &URL);
CStringA DecodeURL(const CStringA &URL);

// 通配符匹配，支持'*'和'?'，匹配成功返回非零值
int PatternMatch(const char *wild,const char *string);
int PatternMatch(const WCHAR *wild,const WCHAR *string);

CString GetWorkingDir();
void SetWorkingDir(LPCTSTR szWorkDir);

//
enum EEnumFileFlag
{
	enumFile		= 0x1,	//搜索文件夹
	enumFolder		= 0x2,	//搜索文件
	enumRecursive	= 0x4,	//递归搜索
	enumFullPath	= 0x8,	//返回全路径
};
// 里边需要使用通配符*
BOOL EnumFilesInFolder(CString strFolderPath,
					   CString strFilter,
					   CStringArray& saFile,
					   DWORD dwFlag=(enumFile|enumFullPath)) ;


DWORD XGetTickCount();
DWORD GetSysRunTime();

void XSleep(DWORD dwMiliSec);
UINT XGetLastError();
void XSetLastError(UINT nErrorCode);
void XZeroMemory(void* Destination,unsigned int Length);


//////////////////////////////////////////////////////////////////////////
// time
void XGetLocalTime( LPSYSTEMTIME lpSystemTime);
void XGetSystemTime(LPSYSTEMTIME lpSystemTime);

//////////////////////////////////////////////////////////////////////////
// 屏幕尺寸、dpi、单位转换
// 
// 绘图时的单位以96dpi的屏幕的1像素为单位，即LP（逻辑单位）
//
inline float InchToMM(float fInch) { return fInch*25.4f; }
inline float MMToInch(float fMM)  { return fMM*0.03937007874f; }
inline float InchToLP(float fInch){ return fInch*96.0f; }
inline float LPToInch(float fLP)  { return fLP*0.0104166667f; }
inline float MMToLP(float fMM) { return InchToLP(MMToInch(fMM)); }
inline float LPToMM(float fLP) { return InchToMM(LPToInch(fLP)); }

//inline float Pound

#define DEFAULT_DPI		96.0f
void SetScreenParameter(int nWidth,int nHeight,float fDpi);
CSize GetScreenSize();	//设备单位，像素
CSize GetScreenLpSize(); //逻辑单位，换算成电脑的尺寸
float GetScreenDpi();
float ScreenDp2Lp(float fDp);
float ScreenLp2Dp(float fLp);
int ScreenDp2Lp(int nDp);
int ScreenLp2Dp(int nLp);

inline void ScreenDp2Lp(CPoint& pt) { pt.x=ScreenDp2Lp(pt.x); pt.y=ScreenDp2Lp(pt.y); }
inline void ScreenLp2Dp(CPoint& pt) { pt.x=ScreenLp2Dp(pt.x); pt.y=ScreenLp2Dp(pt.y); }
inline void ScreenDp2Lp(CRect& rc) { rc.left=ScreenDp2Lp(rc.left); rc.top=ScreenDp2Lp(rc.top); rc.right=ScreenDp2Lp(rc.right); rc.bottom=ScreenDp2Lp(rc.bottom); }
inline void ScreenLp2Dp(CRect& rc) { rc.left=ScreenLp2Dp(rc.left); rc.top=ScreenLp2Dp(rc.top); rc.right=ScreenLp2Dp(rc.right); rc.bottom=ScreenLp2Dp(rc.bottom); }
inline void ScreenDp2Lp(CSize& sz) { sz.cx=ScreenDp2Lp(sz.cx); sz.cy=ScreenDp2Lp(sz.cy); }
inline void ScreenLp2Dp(CSize& sz) { sz.cx=ScreenLp2Dp(sz.cx); sz.cy=ScreenLp2Dp(sz.cy); }

//////////////////////////////////////////////////////////////////////////


typedef void (* TIMERPROC)(HWND, UINT, UINT, DWORD);
UINT
XSetTimer(
		 HWND hWnd,
		 UINT nIDEvent,
		 UINT uElapse,
		 TIMERPROC lpTimerFunc);

BOOL
XKillTimer(
		  HWND hWnd,
		  UINT uIDEvent);


BOOL
SetLocalTime(
			  SYSTEMTIME *lpSystemTime
			 );


#ifndef MAKELONG
	#define MAKELONG(a, b)      ((LONG)(((WORD)((DWORD)(a) & 0xffff)) | ((DWORD)((WORD)((DWORD)(b) & 0xffff))) << 16))
#endif

#ifndef MAKELPARAM
	#define MAKELPARAM(l, h)      ((LPARAM)(DWORD)MAKELONG(l, h))
#endif

#define ASSERT_KINDOF(class_name, object) \
	ASSERT((object)->IsKindOf(RUNTIME_CLASS(class_name)))


//////////////////////////////////////////////////////////////////////////
// 
// 记录函数执行时间

class CRecFuncTime ;
struct SExRecUITime /*: public CNoTrackObject*/
{
	CRecFuncTime*	pCurrent ;
	long			nTabCount ;
	SExRecUITime() { pCurrent=NULL ; nTabCount=-1 ; }
	~SExRecUITime(){ }
};

class CRecFuncTime
{
	long			lStartTick ;
	DWORD			dwCalledSysTime ;
	CString			strTraceInfo ;
	CRecFuncTime*	pParent ;
	CString			strChildTrace ;

public:
	CRecFuncTime(LPCTSTR lpszTraceInfo) ;
	~CRecFuncTime() ;
};

