#pragma once

#ifndef CARCHIVE_2_H
#define CARCHIVE_2_H

#include "mFile.h"
#include "mBaseType.h"

class CObject ;
struct CRuntimeClass;
class CMapPtrToPtr;
class CPtrArray;
class CArchive;

template<class TYPE> class CArray;

UINT  AfxReadStringLength(CArchive& ar, int& nCharSize);
void  AfxWriteStringLength(CArchive& ar, UINT nLength, BOOL bUnicode);
extern BOOL	IsFileExist(LPCTSTR lpszFilePath) ;
extern CObject* MakeObjCopy(CObject* pObj);

// 使用默认字符集进行转换
CStringA FromUnicode(const WCHAR* szWideStr);
CStringW ToUnicode(const char* szMbsStr);

class CArchive
{
protected:
	enum SchemaMapReservedRefs { objTypeArrayRef = 1 };
	enum LoadArrayObjType{ typeUndefined = 0, typeCRuntimeClass = 1, typeCObject = 2 };	

	// archive objects cannot be copied or assigned
	CArchive(const CArchive& arSrc);
	void operator=(const CArchive& arSrc);
	
	unsigned int m_nMode;
	bool m_bUserBuf;
	int m_nBufSize;
	CFile* m_pFile;
	unsigned char* m_lpBufCur;
	unsigned char* m_lpBufMax;
	unsigned char* m_lpBufStart;
	
	UINT m_nObjectSchema;

	// array/map for CObject* and CRuntimeClass* load/store
	UINT m_nMapCount;
	union
	{
		CPtrArray* m_pLoadArray;
		CMapPtrToPtr* m_pStoreMap;
	};
	// map to keep track of mismatched schemas
	CMapPtrToPtr* m_pSchemaMap;

	// advanced parameters (controls performance with large archives)
	UINT m_nGrowSize;
	UINT m_nHashSize;

	WORD m_wVersion;

	// Implementation
public:
	void FillBuffer(unsigned int nBytesNeeded);
	void CheckCount();  // throw exception if m_nMapCount is too large
	
	// special functions for reading and writing (16-bit compatible) counts
	DWORD ReadCount();
	void WriteCount(DWORD dwCount);
	
	CObject* ReadObject(const CRuntimeClass* pClass);
	void WriteObject(const CObject* pOb);

	void WriteClass(const CRuntimeClass* pClassRef);
	CRuntimeClass* ReadClass(const CRuntimeClass* pClassRefRequested = NULL,
		UINT* pSchema = NULL, DWORD* pObTag = NULL);
	void SerializeClass(const CRuntimeClass* pClassRef);
	void MapObject(const CObject* pOb);
	void EnsureSchemaMapExists(CArray<LoadArrayObjType>** ppObjTypeArray = NULL);


public:
// Flag values
	enum Mode { store = 0, load = 1, bNoFlushOnDelete = 2, bNoByteSwap = 4 };

	CArchive(CFile* pFile, unsigned int nMode, int nBufSize = 4096, void* lpBuf = NULL);
	~CArchive();

	// 添加签名及版本号支持,通常应该为序列化时第一个调用的函数,读取时返回FALSE
	// 表示不是此类型签名的文件或者版本号大于当前支持的版本号，签名最长255个字符
	BOOL SerializeSignature(const char* sig,WORD wVer);
	WORD GetVersion() const ;

// Attributes
	bool IsLoading() const { return (m_nMode & CArchive::load) != 0; }
	bool IsStoring() const { return (m_nMode & CArchive::load) == 0; }
	bool IsByteSwapping() const { return false; }
	bool IsBufferEmpty() const { return m_lpBufCur == m_lpBufMax; }
	CFile* GetFile() const { return m_pFile; }

// Operations
	unsigned int Read(void* lpBuf, unsigned int nMax);
	void EnsureRead(void *lpBuf, unsigned int nCount);
	void Write(const void* lpBuf, unsigned int nMax);
	void Flush();
	void Close();
	void Abort();   // close and shutdown without exceptions

public:

#define READBYBYTE(x)	(memcpy(&(x),m_lpBufCur,sizeof(x)))
#define WRITEBYBYTE(x)  (memcpy(m_lpBufCur,&(x),sizeof(x)))

	// insertion operations
	CArchive& operator<<(int i)		{ return CArchive::operator<<((long)i); }
	CArchive& operator<<(unsigned u)	{ return CArchive::operator<<((long)u); }
	CArchive& operator<<(short w)		{ return CArchive::operator<<((unsigned short)w); }
	CArchive& operator<<(char ch)		{ return CArchive::operator<<((unsigned char)ch); }
	CArchive& operator<<(signed char ch)		{ return CArchive::operator<<((unsigned char)ch); }
	//CArchive& operator<<(WCHAR ch)	{ return CArchive::operator<<((unsigned short)ch); }
	CArchive& operator<<(bool b)		{ return CArchive::operator <<((unsigned char)(b ? 1 : 0)); }
	CArchive& operator<<(CTime& time)	{ return CArchive::operator <<(double(time.GetTime())); }
	CArchive& operator<<(const RECT& rect)	{ Write(&rect, sizeof(RECT)); return *this; }
	CArchive& operator<<(const SIZE& sz)	{ Write(&sz, sizeof(SIZE)); return *this; }
	CArchive& operator<<(unsigned char by)
	{ 
		if (m_lpBufCur + sizeof(unsigned char) > m_lpBufMax) Flush();
		WRITEBYBYTE(by);
		m_lpBufCur += sizeof(unsigned char); return *this; 
	}

	CArchive& operator<<(uint64 dwdw)
	{ 
		if (m_lpBufCur + sizeof(uint64) > m_lpBufMax) Flush();
		WRITEBYBYTE(dwdw);
		m_lpBufCur += sizeof(long long); return *this; 
	}
	CArchive& operator<<(int64 n) //__int64
	{
		if (m_lpBufCur + sizeof(int64) > m_lpBufMax) Flush();
		WRITEBYBYTE(n);
		m_lpBufCur += sizeof(int64); return *this; 
	}
	CArchive& operator<<(unsigned short w)
	{ 
		if (m_lpBufCur + sizeof(unsigned short) > m_lpBufMax) Flush();
		WRITEBYBYTE(w);
		m_lpBufCur += sizeof(unsigned short); return *this; 
	}
	CArchive& operator<<(long l)
	{ 
		if (m_lpBufCur + sizeof(long) > m_lpBufMax) Flush();
		WRITEBYBYTE(l);
		m_lpBufCur += sizeof(long); return *this; 
	}
	CArchive& operator<<(unsigned long dw)
	{ 
		if (m_lpBufCur + sizeof(unsigned long) > m_lpBufMax) Flush();
		WRITEBYBYTE(dw);
		m_lpBufCur += sizeof(unsigned long); return *this; 
	}
	CArchive& operator<<(float f)
	{ 
		if (m_lpBufCur + sizeof(float) > m_lpBufMax) Flush();
		WRITEBYBYTE(f);
		m_lpBufCur += sizeof(float); return *this;
	}
	CArchive& operator<<(double d)
	{
		if (m_lpBufCur + sizeof(double) > m_lpBufMax) Flush();
		WRITEBYBYTE(d);
		m_lpBufCur += sizeof(double); return *this; 
	}

	// extraction operations
	CArchive& operator>>(int& i)		{ return CArchive::operator>>((long&)i); }
	CArchive& operator>>(unsigned& u)	{ return CArchive::operator>>((long&)u); }
	CArchive& operator>>(short& w)		{ return CArchive::operator>>((unsigned short&)w); } 
	CArchive& operator>>(char& ch)		{ return CArchive::operator>>((unsigned char&)ch); }
	CArchive& operator>>(signed char& ch)		{ return CArchive::operator>>((unsigned char&)ch); }
	//CArchive& operator>>(WCHAR& ch)	{ return CArchive::operator>>((unsigned short&)ch); }
	CArchive& operator>>(bool& b)		{ unsigned char by; CArchive& ar = CArchive::operator>>(by); b = (by ? true : false); return ar; }
	CArchive& operator>>(CTime& time)	{ double t; CArchive& ar = CArchive::operator>>(t); time.SetTime((long long)t); return ar; }
	CArchive& operator>>(RECT& rect)		{ EnsureRead(&rect, sizeof(RECT)); return *this; }
	CArchive& operator>>(SIZE& sz)  {EnsureRead(&sz, sizeof(SIZE)); return *this; }
	CArchive& operator>>(unsigned char& by)
	{ 
		if (m_lpBufCur + sizeof(unsigned char) > m_lpBufMax)
			FillBuffer((unsigned int)(sizeof(unsigned char) - (m_lpBufMax - m_lpBufCur)));
		READBYBYTE(by);
		m_lpBufCur += sizeof(unsigned char); return *this; 
	}

	CArchive& operator>>(uint64& dwdw) // ULONGLONG
	{ 
		if (m_lpBufCur + sizeof(uint64) > m_lpBufMax)
			FillBuffer(sizeof(uint64) - (unsigned int)(m_lpBufMax - m_lpBufCur));
		READBYBYTE(dwdw);
		m_lpBufCur += sizeof(uint64); return *this; 
	}
	CArchive& operator>>(int64& n) 
	{ 
		if (m_lpBufCur + sizeof(int64) > m_lpBufMax)
			FillBuffer(sizeof(int64) - (unsigned int)(m_lpBufMax - m_lpBufCur));
		READBYBYTE(n);
		m_lpBufCur += sizeof(int64); return *this; 
	}
	CArchive& operator>>(unsigned short& w)
	{ 
		if (m_lpBufCur + sizeof(unsigned short) > m_lpBufMax)
			FillBuffer((unsigned int)(sizeof(unsigned short) - (m_lpBufMax - m_lpBufCur)));
		READBYBYTE(w);
		m_lpBufCur += sizeof(unsigned short); return *this; 
	}
	CArchive& operator>>(unsigned long& dw)
	{ 
		if (m_lpBufCur + sizeof(unsigned long) > m_lpBufMax)
			FillBuffer((unsigned int)(sizeof(unsigned long) - (m_lpBufMax - m_lpBufCur)));
		READBYBYTE(dw);
		m_lpBufCur += sizeof(unsigned long); return *this; 
	}
	CArchive& operator>>(float& f)
	{ 
		if (m_lpBufCur + sizeof(float) > m_lpBufMax)
			FillBuffer((unsigned int)(sizeof(float) - (m_lpBufMax - m_lpBufCur)));
		READBYBYTE(f);
		m_lpBufCur += sizeof(float); return *this; 
	}
	CArchive& operator>>(double& d)
	{ 
		if (m_lpBufCur + sizeof(double) > m_lpBufMax)
			FillBuffer((unsigned int)(sizeof(double) - (m_lpBufMax - m_lpBufCur)));
		READBYBYTE(d);
		m_lpBufCur += sizeof(double); return *this; 
	}
	CArchive& operator>>(long& l)
	{ 
		if (m_lpBufCur + sizeof(long) > m_lpBufMax)
			FillBuffer((unsigned int)(sizeof(long) - (m_lpBufMax - m_lpBufCur)));
		READBYBYTE(l);
		m_lpBufCur += sizeof(long); return *this; 
	}

	//Used CStringT to serialize CSimpleStringT because it supports conversions (char <--> WCHAR).
	template< class BaseType, class StringTraits >
	CArchive& operator<<(const CStringT<BaseType,StringTraits>& str)
	{		
		int nCharSize = sizeof(BaseType);
		AfxWriteStringLength(*this, str.GetLength(), nCharSize>=2);

		if ( nCharSize>2 )
		{
			for (int i=0;i<str.GetLength();i++)
			{
				*this << (unsigned short)(str[i]);
			}
		}else
		{
			Write(str, str.GetLength()*sizeof(BaseType));
		}
		return *this;
	}   

	template< class BaseType, class StringTraits >
	CArchive& operator>>(CStringT<BaseType,StringTraits>& str)
	{
		int nCharSize;  // 1 = char, 2 = wchar_t
		UINT nLength = UINT( AfxReadStringLength(*this, nCharSize) );

		if (nCharSize == sizeof(BaseType))
		{
			BaseType* p = str.GetBuffer(nLength) ;
			Read(p,nLength*nCharSize);
			p[nLength]=0;
			str.ReleaseBuffer(nLength);
		}
		else 
		{
			if ( nCharSize==1 )
			{
				char* pc=new char[nLength+1];
				Read(pc,nLength);
				pc[nLength]=0;
				str = (typename StringTraits::PCXSTR)(ToUnicode(pc)) ;
				delete[] pc;
			} 
			else if( nCharSize==2 )
			{
				MCHAR* pm=new MCHAR[nLength+1];
				Read(pm,nLength*sizeof(MCHAR));

				if ( sizeof(WCHAR)==2 )
				{
					pm[nLength]=0;
					str = (WCHAR*) pm;
				} 
				else
				{
					WCHAR* pw=new WCHAR[nLength+1];
					for (UINT i=0;i<nLength;i++)
					{
						pw[i]=pm[i];
					}
					pw[nLength]=0;
					str = pw;
					delete[] pw;
				}
				delete[] pm;
			}
		}
		return *this;
	}
};

inline CArchive& operator<<(CArchive& ar, const CObject* pOb)
{
	ar.WriteObject(pOb); return ar; 
}
inline CArchive& operator>>(CArchive& ar, CObject*& pOb)
{
	pOb = ar.ReadObject(NULL); return ar; 
}


inline CArchive& operator<<(CArchive& ar, const POINTFLT& pt)
{
	return ar<<pt.x<<pt.y;
}

inline CArchive& operator>>(CArchive& ar, POINTFLT& pt)
{
	return ar>>pt.x>>pt.y;
}
inline CArchive& operator<<(CArchive& ar, const RECTF& rc)
{
	return ar<<rc.x1<<rc.y1<<rc.x2<<rc.y2;
}

inline CArchive& operator>>(CArchive& ar, RECTF& rc)
{
	return ar>>rc.x1>>rc.y1>>rc.x2>>rc.y2;
}


template<class TYPE>
void SerializeElements(CArchive& ar, TYPE* pElements, int nCount)
{
	// default is bit-wise read/write
	if (ar.IsStoring())
	{
		TYPE* pData;
		UINT nElementsLeft;

		nElementsLeft = nCount;
		pData = pElements;
		while( nElementsLeft > 0 )
		{
			UINT nElementsToWrite;

			nElementsToWrite = UINT(min(nElementsLeft, INT_MAX/sizeof(TYPE)));
			ar.Write(pData, nElementsToWrite*sizeof(TYPE));
			nElementsLeft -= nElementsToWrite;
			pData += nElementsToWrite;
		}
	}
	else
	{
		TYPE* pData;
		UINT nElementsLeft;

		nElementsLeft = nCount;
		pData = pElements;
		while( nElementsLeft > 0 )
		{
			UINT nElementsToRead;

			nElementsToRead = UINT(min(nElementsLeft, INT_MAX/sizeof(TYPE)));
			ar.EnsureRead(pData, nElementsToRead*sizeof(TYPE));
			nElementsLeft -= nElementsToRead;
			pData += nElementsToRead;
		}
	}
}




template<class T>
BOOL WriteToTextFile(LPCTSTR szFilePath,const T& str,BOOL bAppend=FALSE)
{
	if ( !IsFileExist(szFilePath) )
		bAppend = FALSE ;

	CFile file;
	if(!file.Open(szFilePath,CFile::modeWrite|(bAppend?0:CFile::modeCreate)))
		return FALSE ;

	BOOL bIsUnicode = sizeof(typename T::XCHAR)==2 ;

	if( bAppend )
		file.SeekToEnd();
	else if ( bIsUnicode )
	{
		WORD wFlag = 0xfeff ;
		file.Write(&wFlag,2);
	}
#ifndef ANDROID
	try
#endif
	{
		file.Write((typename T::PCXSTR)str,str.GetLength()*sizeof(typename T::XCHAR)) ;
		file.Close() ;
	}
#ifndef ANDROID
	catch (...)
	{
		return FALSE;
	}
#endif

	return TRUE ;
}


// T必须支持Serialize方法，且有public的构造函数
template<class T>
T* MakeTObjCopy(T* pT,WORD nVersion)
{
	CMemFile mFile;

	{
		CArchive ar(&mFile,CArchive::store);
		ar.SerializeSignature("",nVersion);
		pT->Serialize(ar);
		ar.Flush();
		ar.Close();
	}
	T* pNew = new T() ;
	{
		mFile.SeekToBegin();
		CArchive ar(&mFile,CArchive::load);
		ar.SerializeSignature("",nVersion);
		pNew->Serialize(ar);
	}
	return pNew ;
}

#endif
