#pragma once

#include "iconv.h"
#include "cstring.h"

//
const char* GetDefaultCharset();
void SetDefaultCharset(const char* szCharset);

/*
>0ʱ����ת�����ַ���������ʱ����0
<=0ʱ����, -1: no such translation
-2: space not enough
-3: convert error
���nMultiBytes==0����ʹ��lpMultiStr��
ֻ�Ƿ�����Ҫ���ַ��ռ䣨������������
����CHARΪ��λ
ת������ַ�����0��β

*/
int FromUnicode(const char* szCodePage,		//Ҫת���ɵ��ַ���
				const WCHAR* lpWideStr,
				int nWideChar,
				char* lpMultiStr,
				int nMultiBytes,
				BOOL bTryBest=TRUE,
				const char* szDefaultChar=" ");

/*
>0ʱ����ת�����ַ���������ʱ����0
<=0ʱ����, -1: no such translation
-2: space not enough
-3: convert error
���cchWideChar==0����ʹ��lpWideCharStr��
ֻ�Ƿ�����Ҫ���ַ��ռ䣨������������
����WCHARΪ��λ

*/
int ToUnicode(const char* szCodePage,		//ԭʼ�ַ���
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

// ʹ��GBK�ַ�������ת��
CStringA FromUnicode(const WCHAR* szWideStr);

// ʹ��GBK�ַ�������ת��
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



