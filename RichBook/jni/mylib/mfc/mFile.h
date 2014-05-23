#pragma once

#include "mBaseType.h"
#include <string>
#include <stdio.h>

// can create multi-level directory
BOOL WriteDirectory(LPCTSTR Directory);

// can delete directory which has sub file or sub folder
BOOL DeleteDirectory(const wchar_t* strDes);

// copy file, both should be full path
BOOL CopyFile(LPCTSTR szFrom,LPCTSTR szTo,BOOL bFailIfExist);

struct CFileStatus
{
	CTime m_ctime;          // creation date/time of file
	CTime m_mtime;          // last modification date/time of file
	CTime m_atime;          // last access date/time of file
	ULONGLONG m_size;            // logical size of file in bytes
	BYTE m_attribute;       // logical OR of CFile::Attribute enum values
	BYTE _m_padding;        // pad the structure to a WORD
	TCHAR m_szFullName[MAX_PATH]; // absolute path name
};
class CFileException
{
public:
	int m_nErr;
};

class CFile
{
public:
	// Flag values
	enum OpenFlags {
		modeRead =         (int) 0x00000,
		modeWrite =        (int) 0x00001,
		modeReadWrite =    (int) 0x00002,
		shareCompat =      (int) 0x00000,
		shareExclusive =   (int) 0x00010,
		shareDenyWrite =   (int) 0x00020,
		shareDenyRead =    (int) 0x00030,
		shareDenyNone =    (int) 0x00040,
		modeNoInherit =    (int) 0x00080,
		modeCreate =       (int) 0x01000,
		modeNoTruncate =   (int) 0x02000,
		typeText =         (int) 0x04000, // typeText and typeBinary are
		typeBinary =       (int) 0x08000, // used in derived classes only
		osNoBuffer =       (int) 0x10000,
		osWriteThrough =   (int) 0x20000,
		osRandomAccess =   (int) 0x40000,
		osSequentialScan = (int) 0x80000,
	};

	enum Attribute {
		normal =    0x00,
		readOnly =  0x01,
		hidden =    0x02,
		system =    0x04,
		volume =    0x08,
		directory = 0x10,
		archive =   0x20
	};

	static const FILE* hFileNull;
	static BOOL Remove(const TCHAR* strFile);
	static void Rename(LPCTSTR lpszOldName,LPCTSTR lpszNewName);
	static BOOL GetStatus(LPCTSTR lpszFileName,CFileStatus& rStatus);


	enum SeekPosition { begin = SEEK_SET, current = SEEK_CUR, end = SEEK_END };
	
	operator FILE*() const
	{
		return m_hFile;
	}

	// Constructors
	CFile();
	CFile(FILE* hFile);
	CFile(const TCHAR* lpszFileName, unsigned int nOpenFlags);

	// Attributes
	FILE* m_hFile;

	BOOL IsOpen() const { return m_hFile!=hFileNull; }

	virtual uint64 GetPosition() const;
	virtual void SetFilePath(LPCTSTR lpszNewName) { m_strFileName = lpszNewName; }

	// Operations
	virtual BOOL Open(LPCTSTR lpszFileName, UINT nOpenFlags);
	virtual BOOL OpenFile(CString strFileName, UINT nOpenFlags = modeReadWrite);
	
	uint64 SeekToEnd() { return Seek(0, CFile::end); }
	void SeekToBegin() { Seek(0, CFile::begin); }

	virtual uint64 Seek(int64 lOff, unsigned int nFrom);
	virtual void SetLength(uint64 dwNewLen);
	virtual uint64 GetLength() const;

	virtual unsigned int Read(void* lpBuf, unsigned int nCount);
	virtual UINT Write(const void* lpBuf, unsigned int nCount);

	virtual void Abort();
	virtual void Flush();
	virtual void Close();

	BOOL GetStatus(CFileStatus& rStatus) const;

	// Implementation
public:
	virtual ~CFile();

	enum BufferCommand { bufferRead, bufferWrite, bufferCommit, bufferCheck };
	enum BufferFlags 
	{ 
		bufferDirect = 0x01,
		bufferBlocking = 0x02
	};

protected:
	bool m_bCloseOnDelete;
	CString m_strFileName;
};



/////////////////////////////////////////////////////////////////////////////
// STDIO file implementation

class CStdioFile : public CFile
{
public:
	// Constructors
	CStdioFile();
	CStdioFile(LPCTSTR lpszFileName, UINT nOpenFlags);


	// Operations
	// reading and writing strings
	virtual void WriteString(LPCTSTR lpsz);
	virtual LPTSTR ReadString(LPTSTR lpsz, UINT nMax);
	virtual BOOL ReadString(CString& rString);

	// Implementation
public:
	virtual ~CStdioFile();
};


//////////////////////////////////////////////////////////////////////////
// CMemoryFile

class CMemFile : public CFile
{
public:
	// Constructors
	/* explicit */ CMemFile(UINT nGrowBytes = 1024);
	CMemFile(BYTE* lpBuffer, UINT nBufferSize, UINT nGrowBytes = 0);

	// Operations
	void Attach(BYTE* lpBuffer, UINT nBufferSize, UINT nGrowBytes = 0);
	BYTE* Detach();

	// Advanced Overridables
protected:
	virtual BYTE* Alloc(SIZE_T nBytes);
	virtual BYTE* Realloc(BYTE* lpMem, SIZE_T nBytes);
	virtual BYTE* Memcpy(BYTE* lpMemTarget, const BYTE* lpMemSource, SIZE_T nBytes);
	virtual void Free(BYTE* lpMem);
	virtual void GrowFile(SIZE_T dwNewLen);

	// Implementation
protected:
	SIZE_T m_nGrowBytes;
	SIZE_T m_nPosition;
	SIZE_T m_nBufferSize;
	SIZE_T m_nFileSize;
	BYTE* m_lpBuffer;
	BOOL m_bAutoDelete;

public:
	virtual ~CMemFile();
	virtual ULONGLONG GetPosition() const;
	BOOL GetStatus(CFileStatus& rStatus) const;
	virtual ULONGLONG Seek(LONGLONG lOff, UINT nFrom);
	virtual void SetLength(ULONGLONG dwNewLen);
	virtual UINT Read(void* lpBuf, UINT nCount);
	virtual UINT Write(const void* lpBuf, UINT nCount);
	virtual void Abort();
	virtual void Flush();
	virtual void Close();
	virtual UINT GetBufferPtr(UINT nCommand, UINT nCount = 0,
		void** ppBufStart = NULL, void** ppBufMax = NULL);
	virtual ULONGLONG GetLength() const;
};

// You can use CIMemoryStream when istream is needed
// usage:
//		char* buffer;	int nSize=100;
//		CMemoryBuf mem(buffer,nSize);
//		std::istream in(&mem);
// 
#include <streambuf>
struct CMemoryBuf: std::streambuf
{
	CMemoryBuf(char* b, unsigned nSize) 
	{
		char* e = b+nSize ;
		this->setg(b, b, e); 
	}
};
