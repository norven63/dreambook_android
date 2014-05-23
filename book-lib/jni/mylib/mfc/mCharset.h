#pragma once

#include "iconv.h"
#include "cstring.h"

//
const char* GetDefaultCharset();
void SetDefaultCharset(const char* szCharset);

/*
>0时返回转换的字符数，出错时返回0
<=0时出错, -1: no such translation
-2: space not enough
-3: convert error
如果nMultiBytes==0，则不使用lpMultiStr，
只是返回需要的字符空间（包括结束符）
均已CHAR为单位
转换完的字符串以0结尾

*/
int FromUnicode(const char* szCodePage,		//要转换成的字符集
				const WCHAR* lpWideStr,
				int nWideChar,
				char* lpMultiStr,
				int nMultiBytes,
				BOOL bTryBest=TRUE,
				const char* szDefaultChar=" ");

/*
>0时返回转换的字符数，出错时返回0
<=0时出错, -1: no such translation
-2: space not enough
-3: convert error
如果cchWideChar==0，则不使用lpWideCharStr，
只是返回需要的字符空间（包括结束符）
均已WCHAR为单位

*/
int ToUnicode(const char* szCodePage,		//原始字符集
			  const char* lpMultiByteStr,  
			  int cbMultiByte,        
			  WCHAR* lpWideCharStr,  
			  int cchWideChar,
			  BOOL bTryBest=TRUE,
			  WCHAR cDefaultChar=L' ');

CStringA FromUnicode(const char* szCodePage, 
					 const WCHAR* lpWideStr,
					 const char* szDefaultChar="?");

CStringW ToUnicode(const char* szCodePage, 
				   const char* lpMultiByteStr,
				   WCHAR cDefaultChar =L'?');

// 使用GBK字符集进行转换
CStringA FromUnicode(const WCHAR* szWideStr);

// 使用GBK字符集进行转换
CStringW ToUnicode(const char* szMbsStr);

std::string FromUnicode(std::wstring ws);
std::wstring ToUnicode(std::string s);


// unicode support macro
#define A2W(x)	((LPCWSTR)ToUnicode(x))
#define W2A(x)	((LPCSTR)FromUnicode(x))


#if defined(_UNICODE) || defined(UNICODE)
#define A2T(x)	A2W(x)
#define W2T(x)	x
#define T2A(x)	W2A(x)
#define T2W(x)	x

#define ttoi(x) wtoi(x)
#ifndef WINCE
#define _ttoi ttoi
#endif

#else
#define A2T(x)	x
#define W2T(x)	W2A(x)
#define T2A(x)	x
#define T2W(x)	A2W(x)

#define ttoi(x) atoi(x)
#ifndef WINCE
#define _ttoi ttoi
#endif
#endif

#define CA2T(x)	A2T(x)
#define CW2T(x)	W2T(x)
#define CT2A(x)	T2A(x)
#define CT2W(x)	T2W(x)

inline int ToInt(LPCTSTR sz) { return _ttoi(sz); }
inline float ToFloat(LPCTSTR sz) { return (float)atof(T2A(sz)); }



