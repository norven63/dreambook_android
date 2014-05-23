#pragma once

#include <stdarg.h>
#include <string.h>
#include "cfunc.h"

//////////////////////////////////////////////////////////////////////////

//
int MSVCRT_vsnwprintf( WCHAR *str, unsigned int len,
					  const WCHAR *format, va_list valist );
int MSVCRT_vswprintf( WCHAR* str, const WCHAR* format, va_list args );
int swnprintf(WCHAR *buffer,size_t count,const WCHAR *format,...);
#ifndef WINDOW
	int swprintf(WCHAR *buffer,const WCHAR *format,...);
	double wcstod(const WCHAR* lpszStr, WCHAR** end);
// 	#ifndef IPHONE
// 		int wcstol(const WCHAR* str,WCHAR ** _EndPtr,int base);
// 	#endif
#endif
int64 atoi64(const char* sz);

#define nulltstr _T("")
#define nullwstr L""
#define nullstr ""

inline LONG InterlockedIncrement(LONG* pRef)
{
	return ++(*pRef) ;
}
inline LONG InterlockedDecrement(LONG* pRef)
{
	return --(*pRef) ;
}


inline LPTSTR XCharNext(LPTSTR psz)
{
	return psz+1 ;
}
inline LPCTSTR XCharNext(LPCTSTR psz)
{
	return psz+1 ;
}
inline LPSTR XCharNext(LPSTR psz)
{
	return psz+1 ;
}
inline LPCSTR XCharNext(LPCSTR psz)
{
	return psz+1 ;
}
inline void XCharLower(LPTSTR sz)
{
	while(*sz!=0)
	{
		*sz=tolower(*sz) ;
		sz++;
	}
}
inline void XCharLower(LPSTR sz)
{
	while(*sz!=0)
	{
		*sz=tolower(*sz) ;
		sz++;
	}
}
inline void XCharUpper(LPSTR sz)
{
	while(*sz!=0)
	{
		*sz=toupper(*sz) ;
		sz++;
	}
}


#ifndef WINCE
	#define PASCAL
#endif
#define ATLTRACE	;//
#define ATLTRACE2 ATLTRACE


///////////////////////////////////////////////////////////////////////////////
// CStringT - String class


struct CStringData
{
	long nRefs;     // reference count
	int nDataLength;
	int nAllocLength;
	// TCHAR data[nAllocLength]

	TCHAR* data()
	{ return (TCHAR*)(this + 1); }
};

// Globals

// For an empty string, m_pchData will point here
// (note: avoids special case of checking for NULL m_pchData)
// empty string data (and locked)
extern int rgInitData[] ;
extern CStringData* _atltmpDataNil ;
extern LPCTSTR _atltmpPchNil ;

template< typename BaseType = CHAR >
class ChTraitsBase
{
public:
	typedef char XCHAR;
	typedef LPSTR PXSTR;
	typedef LPCSTR PCXSTR;
	typedef WCHAR YCHAR;
	typedef LPWSTR PYSTR;
	typedef LPCWSTR PCYSTR;

	static void Format(PXSTR szDest,int nBufSize,PCXSTR pszFormat, va_list args)
	{		
		vsnprintf( szDest, (size_t) nBufSize, pszFormat, args );
	} 
};

template<>
class ChTraitsBase< WCHAR >
{
public:
	typedef WCHAR XCHAR;
	typedef LPWSTR PXSTR;
	typedef LPCWSTR PCXSTR;
	typedef char YCHAR;
	typedef LPSTR PYSTR;
	typedef LPCSTR PCYSTR;

	static void Format(PXSTR szDest,int nBufSize,PCXSTR pszFormat, va_list args)
	{
		MSVCRT_vsnwprintf(szDest,(unsigned int)nBufSize, pszFormat, args);
	}
};


template< typename BaseType,class StringTraits=ChTraitsBase<BaseType> >
class CStringT
{
public:
	typedef CStringT< BaseType> CThisString;
	typedef typename StringTraits::XCHAR XCHAR;
	typedef typename StringTraits::PXSTR PXSTR;
	typedef typename StringTraits::PCXSTR PCXSTR;
	typedef typename StringTraits::YCHAR YCHAR;
	typedef typename StringTraits::PYSTR PYSTR;
	typedef typename StringTraits::PCYSTR PCYSTR;

public:
	// Constructors
	CStringT()
	{
		Init();
	}

	CStringT(const CStringT& stringSrc)
	{
		if (stringSrc.GetData()->nRefs >= 0)
		{
			m_pchData = stringSrc.m_pchData;
			InterlockedIncrement(&GetData()->nRefs);
		}
		else
		{
			Init();
			*this = stringSrc.m_pchData;
		}
	}

	CStringT(XCHAR ch, int nRepeat = 1)
	{
		Init();
		if (nRepeat >= 1)
		{
			if(AllocBuffer(nRepeat))
			{
				for (int i = 0; i < nRepeat; i++)
					m_pchData[i] = ch;
			}
		}
	}

	CStringT(PCXSTR lpsz)
	{
		Init();

		int nLen = SafeStrlen(lpsz);
		if (nLen != 0)
		{
			if(AllocBuffer(nLen))
				memcpy(m_pchData, lpsz, nLen * sizeof(XCHAR));
		}
	}
	// 禁止隐式字符集转换
// 	CStringT(PCYSTR lpsz)
// 	{
// 		Init();
// 		*this = lpsz ;
// 	}
	CStringT(PCXSTR lpch, int nLength)
	{
		Init();
		if (nLength != 0)
		{
			if(AllocBuffer(nLength))
				memcpy(m_pchData, lpch, nLength * sizeof(XCHAR));
		}
	}
	void Append(PCXSTR sz)
	{
		*this += sz ;
	}
	void AppendFormat(PCXSTR lpszFormat, ...)
	{
		CThisString s;
		va_list argList;
		va_start(argList, lpszFormat);
		s.FormatV(lpszFormat, argList);
		va_end(argList);
		*this += s;
	}

	// Attributes & Operations
	int GetLength() const   // as an array of characters
	{
		return GetData()->nDataLength;
	}

	BOOL IsEmpty() const
	{
		return GetData()->nDataLength == 0;
	}

	void Empty()   // free up the data
	{
		if (GetData()->nDataLength == 0)
			return;

		if (GetData()->nRefs >= 0)
			Release();
		else
			*this = _T("");

	}

	XCHAR GetAt(int nIndex) const   // 0 based
	{
		return m_pchData[nIndex];
	}
	XCHAR GetFirst() const
	{
		return m_pchData[0];
	}
	XCHAR GetLast() const
	{
		return m_pchData[GetLength()-1];
	}
#ifndef WIN32
// 	XCHAR operator [](int nIndex) const   // same as GetAt
// 	{
// 		// same as GetAt
// 		return m_pchData[nIndex];
// 	}
#endif
	XCHAR& operator [](int nIndex)
	{
		CopyBeforeWrite();
		return m_pchData[nIndex];
	}

	void SetAt(int nIndex, XCHAR ch)
	{
		CopyBeforeWrite();
		m_pchData[nIndex] = ch;
	}

	operator PCXSTR() const   // as a C string
	{
		return m_pchData;
	}

	// overloaded assignment
	CThisString& operator =(const CThisString& stringSrc)
	{
		if (m_pchData != stringSrc.m_pchData)
		{
			if ((GetData()->nRefs < 0 && GetData() != _atltmpDataNil) || stringSrc.GetData()->nRefs < 0)
			{
				// actual copy necessary since one of the strings is locked
				AssignCopy(stringSrc.GetData()->nDataLength, stringSrc.m_pchData);
			}
			else
			{
				// can just copy references around
				Release();
				m_pchData = stringSrc.m_pchData;
				InterlockedIncrement(&GetData()->nRefs);
			}
		}
		return *this;
	}

	CThisString& operator =(XCHAR ch)
	{
		AssignCopy(1, &ch);
		return *this;
	}

	CThisString& operator =(YCHAR ch)
	{
		*this = (XCHAR)ch;
		return *this;
	}

	CThisString& operator =(PCXSTR lpsz)
	{
		AssignCopy(SafeStrlen(lpsz), lpsz);
		return *this;
	}

	// 禁止隐式直接转换，请使用A2W或W2A显示转换
// 	CThisString& operator =(PCYSTR lpsz)
// 	{
// 		int nLen = tcslen(lpsz);
// 		if ( nLen>0 )
// 		{
// 			int nDestLen;
// 
// #pragma warning(push)
// #pragma warning(disable : 4996) //Disable warning "no return value"
// 			if (sizeof(XCHAR)==1)
// 			{
// 				PXSTR pbuf;
// 				nDestLen = nLen*4+1;
// 				pbuf=GetBuffer(nDestLen);
// 				wcstombs((char*)pbuf,(LPCWSTR)lpsz,nDestLen+1);
// 			} 
// 			else
// 			{
// 				PXSTR pbuf;
// 				nDestLen = nLen ;
// 				pbuf=GetBuffer(nDestLen);
// 				mbstowcs((wchar_t*)pbuf,(LPCSTR)lpsz,nLen+1);
// 			}
// #pragma warning(pop)
// 			ReleaseBuffer();
// 		} 
// 		else
// 		{
// 			Empty();
// 		}
// 		return *this;
// 	}

	// string concatenation
	CThisString& operator +=(const CThisString& string)
	{
		ConcatInPlace(string.GetData()->nDataLength, string.m_pchData);
		return *this;
	}

	CThisString& operator +=(XCHAR ch)
	{
		ConcatInPlace(1, &ch);
		return *this;
	}

	CThisString& operator +=(YCHAR ch)
	{
		*this += (XCHAR)ch;
		return *this;
	}

	CThisString& operator +=(PCXSTR lpsz)
	{
		ConcatInPlace(SafeStrlen(lpsz), lpsz);
		return *this;
	}

	// string comparison
	int Compare(PCXSTR lpsz) const   // straight character (MBCS/Unicode aware)
	{
		return _cstrcmp(m_pchData, lpsz);
	}

	int CompareNoCase(PCXSTR lpsz) const   // ignore case (MBCS/Unicode aware)
	{
		return _cstrcmpi(m_pchData, lpsz);
	}

	// simple sub-string extraction
	CThisString Mid(int nFirst, int nCount) const
	{
		// out-of-bounds requests return sensible things
		if (nFirst < 0)
			nFirst = 0;
		if (nCount < 0)
			nCount = 0;

		if (nFirst + nCount > GetData()->nDataLength)
			nCount = GetData()->nDataLength - nFirst;
		if (nFirst > GetData()->nDataLength)
			nCount = 0;

		CStringT dest;
		AllocCopy(dest, nCount, nFirst, 0);
		return dest;
	}

	CThisString Mid(int nFirst) const
	{
		return Mid(nFirst, GetData()->nDataLength - nFirst);
	}

	CThisString Left(int nCount) const
	{
		if (nCount < 0)
			nCount = 0;
		else if (nCount > GetData()->nDataLength)
			nCount = GetData()->nDataLength;

		CThisString dest;
		AllocCopy(dest, nCount, 0, 0);
		return dest;
	}

	CThisString Right(int nCount) const
	{
		if (nCount < 0)
			nCount = 0;
		else if (nCount > GetData()->nDataLength)
			nCount = GetData()->nDataLength;

		CThisString dest;
		AllocCopy(dest, nCount, GetData()->nDataLength-nCount, 0);
		return dest;
	}

	PCXSTR GetString() const 
	{
		return( m_pchData );
	}
	CThisString Tokenize( PCXSTR pszTokens, int& iStart ) const
	{
		ASSERT( iStart >= 0 );

		if( (pszTokens == NULL) || (*pszTokens == (XCHAR)0) )
		{
			if (iStart < GetLength())
			{
				return( CStringT( GetString()+iStart ) );
			}
		}
		else
		{
			PCXSTR pszPlace = GetString()+iStart;
			PCXSTR pszEnd = GetString()+GetLength();
			if( pszPlace < pszEnd )
			{
				int nIncluding = tcsspn( pszPlace,pszTokens );

				if( (pszPlace+nIncluding) < pszEnd )
				{
					pszPlace += nIncluding;
					int nExcluding = tcscspn( pszPlace, pszTokens );

					int iFrom = iStart+nIncluding;
					int nUntil = nExcluding;
					iStart = iFrom+nUntil+1;

					return( Mid( iFrom, nUntil ) );
				}
			}
		}

		// return empty string, done tokenizing
		iStart = -1;
		CThisString s;
		return s;
	}

	CThisString SpanIncluding(PCXSTR lpszCharSet) const   // strspn equivalent
	{
		return Left(_cstrspn(m_pchData, lpszCharSet));
	}

	CThisString SpanExcluding(PCXSTR lpszCharSet) const   // strcspn equivalent
	{
		return Left(_cstrcspn(m_pchData, lpszCharSet));
	}

	// upper/lower/reverse conversion
	CThisString& MakeUpper()
	{
		CopyBeforeWrite();
		XCharUpper(m_pchData);
		return *this;
	}

	CThisString& MakeLower()
	{
		CopyBeforeWrite();
		XCharLower(m_pchData);
		return *this;
	}

	CThisString& MakeReverse()
	{
		CopyBeforeWrite();
		_cstrrev(m_pchData);
		return *this;
	}

	CThisString& Trim()
	{
		TrimLeft();
		TrimRight();
		return *this;
	}

	// trimming whitespace (either side)
	CThisString& TrimRight()
	{
		CopyBeforeWrite();

		// find beginning of trailing spaces by starting at beginning (DBCS aware)
		PXSTR lpsz = m_pchData;
		PXSTR lpszLast = NULL;
		while (*lpsz != 0)
		{
			if (_cstrisspace(*lpsz))
			{
				if (lpszLast == NULL)
					lpszLast = lpsz;
			}
			else
			{
				lpszLast = NULL;
			}
			lpsz = lpsz+1;
		}

		if (lpszLast != NULL)
		{
			// truncate at trailing space start
			*lpszLast =0;
			GetData()->nDataLength = (int)(DWORD)(lpszLast - m_pchData);
		}
		return *this;
	}

	CThisString& TrimLeft()
	{
		CopyBeforeWrite();

		// find first non-space character
		PCXSTR lpsz = m_pchData;
		while (_cstrisspace(*lpsz))
			lpsz = lpsz+1;

		// fix up data and length
		int nDataLength = GetData()->nDataLength - (int)(DWORD)(lpsz - m_pchData);
		memmove(m_pchData, lpsz, (nDataLength + 1) * sizeof(XCHAR));
		GetData()->nDataLength = nDataLength;
		return *this;
	}

	// remove continuous occurrences of chTarget starting from right
	CThisString& TrimRight(XCHAR chTarget)
	{
		// find beginning of trailing matches
		// by starting at beginning (DBCS aware)

		CopyBeforeWrite();
		PXSTR lpsz = m_pchData;
		PXSTR lpszLast = NULL;

		while (*lpsz != 0)
		{
			if (*lpsz == chTarget)
			{
				if (lpszLast == NULL)
					lpszLast = lpsz;
			}
			else
				lpszLast = NULL;
			lpsz = lpsz+1;
		}

		if (lpszLast != NULL)
		{
			// truncate at left-most matching character
			*lpszLast = 0;
			GetData()->nDataLength = (int)(DWORD)(lpszLast - m_pchData);
		}
		return *this;
	}

	// remove continuous occcurrences of characters in passed string, starting from right
	CThisString& TrimRight(PCXSTR lpszTargetList)
	{
		// find beginning of trailing matches by starting at beginning (DBCS aware)

		CopyBeforeWrite();
		PXSTR lpsz = m_pchData;
		PXSTR lpszLast = NULL;

		while (*lpsz != 0)
		{
			XCHAR* pNext = lpsz+1;
			if(pNext > lpsz + 1)
			{
				if (_cstrchr_db(lpszTargetList, *lpsz, *(lpsz + 1)) != NULL)
				{
					if (lpszLast == NULL)
						lpszLast = lpsz;
				}
				else
				{
					lpszLast = NULL;
				}
			}
			else
			{
				if (_cstrchr(lpszTargetList, *lpsz) != NULL)
				{
					if (lpszLast == NULL)
						lpszLast = lpsz;
				}
				else
				{
					lpszLast = NULL;
				}
			}

			lpsz = pNext;
		}

		if (lpszLast != NULL)
		{
			// truncate at left-most matching character
			*lpszLast = 0;
			GetData()->nDataLength = (int)(DWORD)(lpszLast - m_pchData);
		}
		return *this;
	}

	// remove continuous occurrences of chTarget starting from left
	CThisString& TrimLeft(XCHAR chTarget)
	{
		// find first non-matching character

		CopyBeforeWrite();
		PCXSTR lpsz = m_pchData;

		while (chTarget == *lpsz)
			lpsz = lpsz+1;

		if (lpsz != m_pchData)
		{
			// fix up data and length
			int nDataLength = GetData()->nDataLength - (int)(DWORD)(lpsz - m_pchData);
			memmove(m_pchData, lpsz, (nDataLength + 1) * sizeof(XCHAR));
			GetData()->nDataLength = nDataLength;
		}
		return *this;
	}
	
	// remove continuous occcurrences of characters in passed string, starting from left
	CThisString& TrimLeft(PCXSTR lpszTargets)
	{
		// if we're not trimming anything, we're not doing any work
		if (SafeStrlen(lpszTargets) == 0)
			return *this;

		CopyBeforeWrite();
		PCXSTR lpsz = m_pchData;

		while (*lpsz != 0)
		{
			XCHAR* pNext = (XCHAR*) (lpsz+1);
			if(pNext > lpsz + 1)
			{
				if (_cstrchr_db(lpszTargets, *lpsz, *(lpsz + 1)) == NULL)
					break;
			}
			else
			{
				if (_cstrchr(lpszTargets, *lpsz) == NULL)
					break;
			}
			lpsz = pNext;
		}

		if (lpsz != m_pchData)
		{
			// fix up data and length
			int nDataLength = GetData()->nDataLength - (int)(DWORD)(lpsz - m_pchData);
			memmove(m_pchData, lpsz, (nDataLength + 1) * sizeof(XCHAR));
			GetData()->nDataLength = nDataLength;
		}

		return *this;
	}

	// advanced manipulation
	// replace occurrences of chOld with chNew
	int Replace(XCHAR chOld, XCHAR chNew)
	{
		int nCount = 0;

		// short-circuit the nop case
		if (chOld != chNew)
		{
			// otherwise modify each character that matches in the string
			CopyBeforeWrite();
			PXSTR psz = m_pchData;
			PXSTR pszEnd = psz + GetData()->nDataLength;
			while (psz < pszEnd)
			{
				// replace instances of the specified character only
				if (*psz == chOld)
				{
					*psz = chNew;
					nCount++;
				}
				psz = (psz+1);
			}
		}
		return nCount;
	}

	// replace occurrences of substring lpszOld with lpszNew;
	// empty lpszNew removes instances of lpszOld
	int Replace(PCXSTR lpszOld, PCXSTR lpszNew)
	{
		// can't have empty or NULL lpszOld

		int nSourceLen = SafeStrlen(lpszOld);
		if (nSourceLen == 0)
			return 0;
		int nReplacementLen = SafeStrlen(lpszNew);

		// loop once to figure out the size of the result string
		int nCount = 0;
		PXSTR lpszStart = m_pchData;
		PXSTR lpszEnd = m_pchData + GetData()->nDataLength;
		PXSTR lpszTarget = NULL;
		while (lpszStart < lpszEnd)
		{
			while ((lpszTarget = (XCHAR*)_cstrstr(lpszStart, lpszOld)) != NULL)
			{
				nCount++;
				lpszStart = lpszTarget + nSourceLen;
			}
			lpszStart += tcslen(lpszStart) + 1;
		}

		// if any changes were made, make them
		if (nCount > 0)
		{
			CopyBeforeWrite();

			// if the buffer is too small, just allocate a new buffer (slow but sure)
			int nOldLength = GetData()->nDataLength;
			int nNewLength =  nOldLength + (nReplacementLen - nSourceLen) * nCount;
			if (GetData()->nAllocLength < nNewLength || GetData()->nRefs > 1)
			{
				CStringData* pOldData = GetData();
				PCXSTR pstr = m_pchData;
				if(!AllocBuffer(nNewLength))
					return -1;
				memcpy(m_pchData, pstr, pOldData->nDataLength * sizeof(XCHAR));
				CThisString::Release(pOldData);
			}
			// else, we just do it in-place
			lpszStart = m_pchData;
			lpszEnd = m_pchData + GetData()->nDataLength;

			// loop again to actually do the work
			while (lpszStart < lpszEnd)
			{
				while ((lpszTarget = (XCHAR*)_cstrstr(lpszStart, lpszOld)) != NULL)
				{
					int nBalance = nOldLength - ((int)(DWORD)(lpszTarget - m_pchData) + nSourceLen);
					memmove(lpszTarget + nReplacementLen, lpszTarget + nSourceLen, nBalance * sizeof(TCHAR));
					memcpy(lpszTarget, lpszNew, nReplacementLen * sizeof(XCHAR));
					lpszStart = lpszTarget + nReplacementLen;
					lpszStart[nBalance] = 0;
					nOldLength += (nReplacementLen - nSourceLen);
				}
				lpszStart += tcslen(lpszStart) + 1;
			}
			GetData()->nDataLength = nNewLength;
		}

		return nCount;
	}

	// remove occurrences of chRemove
	int Remove(XCHAR chRemove)
	{
		CopyBeforeWrite();

		PXSTR pstrSource = m_pchData;
		PXSTR pstrDest = m_pchData;
		PXSTR pstrEnd = m_pchData + GetData()->nDataLength;

		while (pstrSource < pstrEnd)
		{
			if (*pstrSource != chRemove)
			{
				*pstrDest = *pstrSource;
				pstrDest = (pstrDest+1);
			}
			pstrSource = (pstrSource+1);
		}
		*pstrDest = 0;
		int nCount = (int)(DWORD)(pstrSource - pstrDest);
		GetData()->nDataLength -= nCount;

		return nCount;
	}

	// insert character at zero-based index; concatenates if index is past end of string
	int Insert(int nIndex, XCHAR ch)
	{
		CopyBeforeWrite();

		if (nIndex < 0)
			nIndex = 0;

		int nNewLength = GetData()->nDataLength;
		if (nIndex > nNewLength)
			nIndex = nNewLength;
		nNewLength++;

		if (GetData()->nAllocLength < nNewLength)
		{
			CStringData* pOldData = GetData();
			PXSTR pstr = m_pchData;
			if(!AllocBuffer(nNewLength))
				return -1;
			memcpy(m_pchData, pstr, (pOldData->nDataLength + 1) * sizeof(XCHAR));
			CStringT::Release(pOldData);
		}

		// move existing bytes down
		memmove(m_pchData + nIndex + 1, m_pchData + nIndex, (nNewLength - nIndex) * sizeof(XCHAR));
		m_pchData[nIndex] = ch;
		GetData()->nDataLength = nNewLength;

		return nNewLength;
	}

	// insert substring at zero-based index; concatenates if index is past end of string
	int Insert(int nIndex, PCXSTR pstr)
	{
		if (nIndex < 0)
			nIndex = 0;

		int nInsertLength = SafeStrlen(pstr);
		int nNewLength = GetData()->nDataLength;
		if (nInsertLength > 0)
		{
			CopyBeforeWrite();
			if (nIndex > nNewLength)
				nIndex = nNewLength;
			nNewLength += nInsertLength;

			if (GetData()->nAllocLength < nNewLength)
			{
				CStringData* pOldData = GetData();
				PXSTR pstr = m_pchData;
				if(!AllocBuffer(nNewLength))
					return -1;
				memcpy(m_pchData, pstr, (pOldData->nDataLength + 1) * sizeof(XCHAR));
				CThisString::Release(pOldData);
			}

			// move existing bytes down
			memmove(m_pchData + nIndex + nInsertLength, m_pchData + nIndex, (nNewLength - nIndex - nInsertLength + 1) * sizeof(XCHAR));
			memcpy(m_pchData + nIndex, pstr, nInsertLength * sizeof(XCHAR));
			GetData()->nDataLength = nNewLength;
		}

		return nNewLength;
	}

	// delete nCount characters starting at zero-based index
	int Delete(int nIndex, int nCount = 1)
	{
		if (nIndex < 0)
			nIndex = 0;
		int nLength = GetData()->nDataLength;
		if (nCount > 0 && nIndex < nLength)
		{
			if((nIndex + nCount) > nLength)
				nCount = nLength - nIndex;
			CopyBeforeWrite();
			int nBytesToCopy = nLength - (nIndex + nCount) + 1;

			memmove(m_pchData + nIndex,  m_pchData + nIndex + nCount, nBytesToCopy * sizeof(XCHAR));
			nLength -= nCount;
			GetData()->nDataLength = nLength;
		}

		return nLength;
	}

	BOOL StartWith(PCXSTR lpsz)
	{
		if(lpsz==NULL)
			return FALSE;
		int nlen = tcslen(lpsz);
		return tcsncmp(m_pchData,lpsz,nlen)==0;
	}
	BOOL StartWithI(PCXSTR lpsz)
	{
		if(lpsz==NULL)
			return FALSE;
		int nlen = tcslen(lpsz);
		return tcsincmp(m_pchData,lpsz,nlen)==0;
	}
	BOOL EndWith(PCXSTR lpsz)
	{
		if(lpsz==NULL)
			return FALSE;
		int nlen = tcslen(lpsz);
		if(GetLength()<nlen)
			return FALSE;

		return tcsncmp(m_pchData+GetLength()-nlen,lpsz,nlen)==0;
	}
	BOOL EndWithI(PCXSTR lpsz)
	{
		if(lpsz==NULL)
			return FALSE;
		int nlen = tcslen(lpsz);
		if(GetLength()<nlen)
			return FALSE;

		return tcsincmp(m_pchData+GetLength()-nlen,lpsz,nlen)==0;
	}
	// searching (return starting index, or -1 if not found)
	// look for a single character match
	int Find(XCHAR ch) const   // like "C" strchr
	{
		return Find(ch, 0);
	}


	int ReverseFind(XCHAR ch) const
	{
		// find last single character
		PCXSTR lpsz = _cstrrchr(m_pchData, ch);

		// return -1 if not found, distance from beginning otherwise
		return (lpsz == NULL) ? -1 : (int)(lpsz - m_pchData);
	}
	int ReverseFindOneOf(PCXSTR lpszCharSet) const
	{
		PCXSTR lpsz = tcsReverseFindOneOf(m_pchData, lpszCharSet);
		return (lpsz == NULL) ? -1 : (int)(lpsz - m_pchData);
	}

	int Find(XCHAR ch, int nStart) const   // starting at index
	{
		int nLength = GetData()->nDataLength;
		if (nStart < 0 || nStart >= nLength)
			return -1;

		// find first single character
		PCXSTR lpsz = _cstrchr(m_pchData + nStart, (XCHAR)ch);

		// return -1 if not found and index otherwise
		return (lpsz == NULL) ? -1 : (int)(lpsz - m_pchData);
	}

	int FindOneOf(PCXSTR lpszCharSet, int nStart) const
	{
		int nLength = GetData()->nDataLength;
		if (nStart < 0 || nStart >= nLength)
			return -1;

		PCXSTR lpsz = _cstrpbrk(m_pchData+nStart, lpszCharSet);
		return (lpsz == NULL) ? -1 : (int)(lpsz - m_pchData);
	}


	// look for a specific sub-string
	// find a sub-string (like strstr)
	int Find(PCXSTR lpszSub) const   // like "C" strstr
	{
		return Find(lpszSub, 0);
	}

	int Find(PCXSTR lpszSub, int nStart) const   // starting at index
	{
		int nLength = GetData()->nDataLength;
		if (nStart < 0 || nStart > nLength)
			return -1;

		// find first matching substring
		PCXSTR lpsz = _cstrstr(m_pchData + nStart, lpszSub);

		// return -1 for not found, distance from beginning otherwise
		return (lpsz == NULL) ? -1 : (int)(lpsz - m_pchData);
	}


	// simple formatting
	// formatting (using wsprintf style formatting)
	BOOL Format(PCXSTR lpszFormat, ...)
	{
		va_list argList;
		va_start(argList, lpszFormat);
		BOOL bRet = FormatV(lpszFormat, argList);
		va_end(argList);
		return bRet;
	}
	BOOL FormatV(PCXSTR lpszFormat, va_list argList)
	{
		int nLength = 1024;
		PXSTR pBuf = GetBuffer(nLength);
		StringTraits::Format(pBuf,1024,lpszFormat,argList);
		ReleaseBuffer();
		return TRUE;
	}

	// Access to string implementation buffer as "C" character array
	PXSTR GetBuffer(int nMinBufLength=-1)
	{
		if( nMinBufLength<0 )
			nMinBufLength = GetLength() ;

		if (GetData()->nRefs > 1 || nMinBufLength > GetData()->nAllocLength)
		{
			// we have to grow the buffer
			CStringData* pOldData = GetData();
			int nOldLen = GetData()->nDataLength;   // AllocBuffer will tromp it
			if (nMinBufLength < nOldLen)
				nMinBufLength = nOldLen;

			if(!AllocBuffer(nMinBufLength))
				return NULL;

			memcpy(m_pchData, pOldData->data(), (nOldLen + 1) * sizeof(XCHAR));
			GetData()->nDataLength = nOldLen;
			CStringT::Release(pOldData);
		}
		return m_pchData;
	}

	void ReleaseBuffer(int nNewLength = -1)
	{
		CopyBeforeWrite();   // just in case GetBuffer was not called

		if (nNewLength == -1)
			nNewLength = tcslen(m_pchData);   // zero terminated

		GetData()->nDataLength = nNewLength;
		m_pchData[nNewLength] = 0;
	}

	PXSTR GetBufferSetLength(int nNewLength)
	{
		if(GetBuffer(nNewLength) == NULL)
			return NULL;

		GetData()->nDataLength = nNewLength;
		m_pchData[nNewLength] = 0;
		return m_pchData;
	}

	void FreeExtra()
	{
		if (GetData()->nDataLength != GetData()->nAllocLength)
		{
			CStringData* pOldData = GetData();
			if(AllocBuffer(GetData()->nDataLength))
			{
				memcpy(m_pchData, pOldData->data(), pOldData->nDataLength * sizeof(XCHAR));
				CStringT::Release(pOldData);
			}
		}
	}

	// Use LockBuffer/UnlockBuffer to turn refcounting off
	PXSTR LockBuffer()
	{
		PXSTR lpsz = GetBuffer(0);
		if(lpsz != NULL)
			GetData()->nRefs = -1;
		return lpsz;
	}

	void UnlockBuffer()
	{
		if (GetData() != _atltmpDataNil)
			GetData()->nRefs = 1;
	}

	// Implementation
public:
	~CStringT()   //  free any attached data
	{
		if (GetData() != _atltmpDataNil)
		{
			if (InterlockedDecrement(&GetData()->nRefs) <= 0)
				delete[] (BYTE*)GetData();
		}
	}

	int GetAllocLength() const
	{
		return GetData()->nAllocLength;
	}

	static BOOL  _IsValidString(PCXSTR lpsz, int /*nLength*/ = -1)
	{
		return (lpsz != NULL) ? TRUE : FALSE;
	}


	// friend inline 

	 
 	 friend bool  operator ==(const CStringT& s1, const CStringT& s2)
 	 { return s1.Compare(s2) == 0; }
	 
 	 friend bool  operator ==(const CStringT& s1, PCXSTR s2)
 	 { return s1.Compare(s2) == 0; }
	 
	 friend bool  operator ==(PCXSTR s1, const CStringT& s2)
	 { return s2.Compare(s1) == 0; }

// 	 friend bool  operator ==(const CStringT& s1, PCYSTR s2)
// 	 { CStringT temp(s2); return s1 == temp; }
// 
// 	 friend bool  operator ==(PCYSTR s1, const CStringT& s2)
// 	 { CStringT temp(s1); return s2==temp; }

	 friend bool  operator !=(const CStringT& s1, const CStringT& s2)
	 { return s1.Compare(s2) != 0; }
	 
	 friend bool  operator !=(const CStringT& s1, PCXSTR s2)
	 { return s1.Compare(s2) != 0; }
	 
	 friend bool  operator !=(PCXSTR s1, const CStringT& s2)
	 { return s2.Compare(s1) != 0; }
	 
	 friend bool  operator <(const CStringT& s1, const CStringT& s2)
	 { return s1.Compare(s2) < 0; }
	 
	 friend bool  operator <(const CStringT& s1, PCXSTR s2)
	 { return s1.Compare(s2) < 0; }
	 
	 friend bool  operator <(PCXSTR s1, const CStringT& s2)
	 { return s2.Compare(s1) > 0; }
	 
	 friend bool  operator >(const CStringT& s1, const CStringT& s2)
	 { return s1.Compare(s2) > 0; }
	 
	 friend bool  operator >(const CStringT& s1, PCXSTR s2)
	 { return s1.Compare(s2) > 0; }
	 
	 friend bool  operator >(PCXSTR s1, const CStringT& s2)
	 { return s2.Compare(s1) < 0; }
	 
	 friend bool  operator <=(const CStringT& s1, const CStringT& s2)
	 { return s1.Compare(s2) <= 0; }
	 
	 friend bool  operator <=(const CStringT& s1, PCXSTR s2)
	 { return s1.Compare(s2) <= 0; }
	 
	 friend bool  operator <=(PCXSTR s1, const CStringT& s2)
	 { return s2.Compare(s1) >= 0; }
	 
	 friend bool  operator >=(const CStringT& s1, const CStringT& s2)
	 { return s1.Compare(s2) >= 0; }
	 
	 friend bool  operator >=(const CStringT& s1, PCXSTR s2)
	 { return s1.Compare(s2) >= 0; }
	 
	 friend bool  operator >=(PCXSTR s1, const CStringT& s2)
	 { return s2.Compare(s1) <= 0; }
	 
	 
	 // CStringT "operator +" functions
	 
	 friend CStringT  operator +(const CStringT& string1, const CStringT& string2)
	 {
	 	CStringT s;
	 	s.ConcatCopy(string1.GetData()->nDataLength, string1.m_pchData, string2.GetData()->nDataLength, string2.m_pchData);
	 	return s;
	 }
	 
	 friend CStringT  operator +(const CStringT& string, XCHAR ch)
	 {
	 	CStringT s;
	 	s.ConcatCopy(string.GetData()->nDataLength, string.m_pchData, 1, &ch);
	 	return s;
	 }
	 
	 friend CStringT  operator +(XCHAR ch, const CStringT& string)
	 {
	 	CStringT s;
	 	s.ConcatCopy(1, &ch, string.GetData()->nDataLength, string.m_pchData);
	 	return s;
	 }
	 
	 friend  CStringT  operator +(const CStringT& string, YCHAR ch)
	 {
	 	return string + (XCHAR)ch;
	 }
	 
	 friend CStringT  operator +(YCHAR ch, const CStringT& string)
	 {
	 	return (TCHAR)ch + string;
	 }
	 
	 friend CStringT  operator +(const CStringT& string, PCXSTR lpsz)
	 {
	 	CStringT s;
	 	s.ConcatCopy(string.GetData()->nDataLength, string.m_pchData, CStringT::SafeStrlen(lpsz), lpsz);
	 	return s;
	 }
	 
	 friend CStringT  operator +(PCXSTR lpsz, const CStringT& string)
	 {
	 	CStringT s;
	 	s.ConcatCopy(CStringT::SafeStrlen(lpsz), lpsz, string.GetData()->nDataLength, string.m_pchData);
	 	return s;
	 }
	 



protected:
	PXSTR m_pchData;   // pointer to ref counted string data

	// implementation helpers
	CStringData* GetData() const
	{
		return ((CStringData*)m_pchData) - 1;
	}

	
	
	
	void Init()
	{
		m_pchData = _GetEmptyString().m_pchData;
	}

	BOOL AllocCopy(CThisString& dest, int nCopyLen, int nCopyIndex, int nExtraLen) const
	{
		// will clone the data attached to this string
		// allocating 'nExtraLen' characters
		// Places results in uninitialized string 'dest'
		// Will copy the part or all of original data to start of new string

		BOOL bRet = FALSE;
		int nNewLen = nCopyLen + nExtraLen;
		if (nNewLen == 0)
		{
			dest.Init();
			bRet = TRUE;
		}
		else if(nNewLen >= nCopyLen)
		{
			if(dest.AllocBuffer(nNewLen))
			{
				memcpy(dest.m_pchData, m_pchData + nCopyIndex, nCopyLen * sizeof(XCHAR));
				bRet = TRUE;
			}
		}

		return bRet;
	}

	// always allocate one extra character for '\0' termination
	// assumes [optimistically] that data length will equal allocation length
	BOOL AllocBuffer(int nLen)
	{
		if (nLen == 0)
		{
			Init();
		}
		else
		{
			CStringData* pData = NULL;
			pData = (CStringData*)new BYTE[sizeof(CStringData) + (nLen + 1) * sizeof(XCHAR)];
			if(pData == NULL)
				return FALSE;

			pData->nRefs = 1;
			pData->nDataLength = nLen;
			pData->nAllocLength = nLen;
			m_pchData = (PXSTR) pData->data();
			m_pchData[nLen]=0;
		}

		return TRUE;
	}

	// Assignment operators
	//  All assign a new value to the string
	//      (a) first see if the buffer is big enough
	//      (b) if enough room, copy on top of old buffer, set size and type
	//      (c) otherwise free old string data, and create a new one
	//
	//  All routines return the new string (but as a 'const CStringT&' so that
	//      assigning it again will cause a copy, eg: s1 = s2 = "hi there".
	//
	void AssignCopy(int nSrcLen, PCXSTR lpszSrcData)
	{
		if(AllocBeforeWrite(nSrcLen))
		{
			memcpy(m_pchData, lpszSrcData, nSrcLen * sizeof(XCHAR));
			GetData()->nDataLength = nSrcLen;
			m_pchData[nSrcLen] = 0;
		}
	}

	// Concatenation
	// NOTE: "operator +" is done as friend functions for simplicity
	//      There are three variants:
	//          CStringT + CStringT
	// and for ? = TCHAR, LPCTSTR
	//          CStringT + ?
	//          ? + CStringT
	BOOL ConcatCopy(int nSrc1Len, PCXSTR lpszSrc1Data, int nSrc2Len, PCXSTR lpszSrc2Data)
	{
		// -- master concatenation routine
		// Concatenate two sources
		// -- assume that 'this' is a new CStringT object

		BOOL bRet = TRUE;
		int nNewLen = nSrc1Len + nSrc2Len;
		if(nNewLen < nSrc1Len || nNewLen < nSrc2Len)
		{
			bRet = FALSE;
		}
		else if(nNewLen != 0)
		{
			bRet = AllocBuffer(nNewLen);
			if (bRet)
			{
				memcpy(m_pchData,lpszSrc1Data, nSrc1Len * sizeof(XCHAR));
				memcpy(m_pchData + nSrc1Len, lpszSrc2Data, nSrc2Len * sizeof(XCHAR));
			}
		}
		return bRet;
	}

	void ConcatInPlace(int nSrcLen, PCXSTR lpszSrcData)
	{
		//  -- the main routine for += operators

		// concatenating an empty string is a no-op!
		if (nSrcLen == 0)
			return;

		// if the buffer is too small, or we have a width mis-match, just
		//   allocate a new buffer (slow but sure)
		if (GetData()->nRefs > 1 || GetData()->nDataLength + nSrcLen > GetData()->nAllocLength)
		{
			// we have to grow the buffer, use the ConcatCopy routine
			CStringData* pOldData = GetData();
			if (ConcatCopy(GetData()->nDataLength, m_pchData, nSrcLen, lpszSrcData))
			{
				CThisString::Release(pOldData);
			}
		}
		else
		{
			// fast concatenation when buffer big enough
			memcpy(m_pchData + GetData()->nDataLength, lpszSrcData, nSrcLen * sizeof(XCHAR));
			GetData()->nDataLength += nSrcLen;
			m_pchData[GetData()->nDataLength] = 0;
		}
	}

	void CopyBeforeWrite()
	{
		if (GetData()->nRefs > 1)
		{
			CStringData* pData = GetData();
			Release();
			if(AllocBuffer(pData->nDataLength))
				memcpy(m_pchData, pData->data(), (pData->nDataLength + 1) * sizeof(XCHAR));
		}
	}

	BOOL AllocBeforeWrite(int nLen)
	{
		BOOL bRet = TRUE;
		if (GetData()->nRefs > 1 || nLen > GetData()->nAllocLength)
		{
			Release();
			bRet = AllocBuffer(nLen);
		}
		return bRet;
	}

	void Release()
	{
		if (GetData() != _atltmpDataNil)
		{
			if (InterlockedDecrement(&GetData()->nRefs) <= 0)
				delete[] (BYTE*)GetData();
			Init();
		}
	}

	static void PASCAL Release(CStringData* pData)
	{
		if (pData != _atltmpDataNil)
		{
			if (InterlockedDecrement(&pData->nRefs) <= 0)
				delete[] (BYTE*)pData;
		}
	}

	static int PASCAL SafeStrlen(PCXSTR lpsz)
	{
		return (lpsz == NULL) ? 0 : tcslen(lpsz);
	}


	static const CThisString&  _GetEmptyString()
	{
		return *(CThisString*)&_atltmpPchNil;
	}


	static const XCHAR* _cstrchr(const XCHAR* p, XCHAR ch)
	{
		return tcschr(p, ch);
	}

	static const XCHAR* _cstrrchr(const XCHAR* p, XCHAR ch)
	{
		return tcsrchr(p, ch);
	}

	static XCHAR* _cstrrev(XCHAR* pStr)
	{
		return tcsrev(pStr);
	}

	static const XCHAR* _cstrstr(const XCHAR* pStr, const XCHAR* pCharSet)
	{
		return tcsstr(pStr, pCharSet);
	}

	static int _cstrspn(const XCHAR* pStr, const XCHAR* pCharSet)
	{
		return (int)tcsspn(pStr, pCharSet);
	}

	static int _cstrcspn(const XCHAR* pStr, const XCHAR* pCharSet)
	{
		return (int)tcscspn(pStr, pCharSet);
	}

	static const XCHAR* _cstrpbrk(const XCHAR* p, const XCHAR* lpszCharSet)
	{
		return tcspbrk(p, lpszCharSet);
	}

	static int _cstrisdigit(XCHAR ch)
	{
		return isdigit(ch);
	}

	static int _cstrisspace(XCHAR ch)
	{
#ifndef ANDROID
		if(sizeof(ch) == sizeof(wchar_t))
			return iswspace(ch);
		else
#endif
			return isspace(ch);
	}

	static int _cstrcmp(const XCHAR* pstrOne, const XCHAR* pstrOther)
	{
		return tcscmp(pstrOne, pstrOther);
	}

	static int _cstrcmpi(const XCHAR* pstrOne, const XCHAR* pstrOther)
	{
		return tcsicmp(pstrOne, pstrOther);
	}

	static int _cstrcoll(const XCHAR* pstrOne, const XCHAR* pstrOther)
	{
		return strcoll(pstrOne, pstrOther);
	}

	static int _cstrtoi(const XCHAR* nptr)
	{
		return atoi(nptr);
	}

	static const XCHAR* _cstrchr_db(const XCHAR* p, XCHAR ch1, XCHAR ch2)
	{
		const XCHAR* lpsz = NULL;
		while (*p != 0)
		{
			if (*p == ch1 && *(p + 1) == ch2)
			{
				lpsz = p;
				break;
			}
			p = (p+1);
		}
		return lpsz;
	}
};

typedef CStringT<CHAR,ChTraitsBase<CHAR> >	CStringA;
typedef CStringT<WCHAR,ChTraitsBase<WCHAR> >	CStringW;
#ifdef UNICODE
	typedef CStringW CString;
#else
	typedef CStringA CString;
#endif


	CString MakeFilePath(CString sDir,CString sFileBaseName,CString sExtName);
	void SplitFilePath(CString sFileFullPath,
		CString* sDirectory,
		CString* sFileName,
		CString* sFileBaseName,
		CString* sFileExtName);