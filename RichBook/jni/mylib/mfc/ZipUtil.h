#pragma once
 
#include "cstring.h"
#include "mCollection.h"

// create our own fileinfo struct to hide the underlying implementation
struct SZipFileInfo
{
	CString szFileName;
	CString szComment;

	DWORD dwVersion;  
	DWORD dwVersionNeeded;
	DWORD dwFlags;	 
	DWORD dwCompressionMethod; 
	DWORD dwDosDate;	
	DWORD dwCRC;   
	DWORD dwCompressedSize; 
	DWORD dwUncompressedSize;
	DWORD dwInternalAttrib; 
	DWORD dwExternalAttrib; 
	BOOL bFolder;
};

class CUnzipper  
{
public:
	CUnzipper(LPCTSTR szFilePath = NULL);
	virtual ~CUnzipper();

	BOOL OpenZip(LPCTSTR szFilePath);
	void CloseZip(); // for multiple reuse
	BOOL IsOpened() const { return m_uzFile!=0; }

	//
	int GetFileCount();
	CString GetFileName(int nfileindex);
	int GetFileIndex(const TCHAR *filename);
	BOOL GetFileInfo(int nfileindex,SZipFileInfo& info);

	// the caller must call free to deallocate memory
	BOOL GetFileData(int nfileindex,CByteArray& byteArray);
	BOOL GetFileData(const TCHAR *filename,CByteArray& byteArray);

	BOOL UnzipFile(CString sFileName,CString szFolder,CString sUnzippedName);
	BOOL UnzipTo(CString szFolder); // unzips to specified folder

protected:
	CArray<SZipFileInfo> m_aInfo;

	void ExtractFilenames();
	BOOL GetFileInfo(SZipFileInfo& info);
	BOOL GotoFile(int nFile);
	BOOL UnzipFile(int nIndex,CString szFolder,CString sUnzippedName);

	void* m_uzFile;
};


class CZipper  
{
public:
	CZipper(LPCTSTR szFilePath = NULL, LPCTSTR szRootFolder = NULL, BOOL bAppend = FALSE);
	virtual ~CZipper();

	// simple interface
	static BOOL ZipFile(LPCTSTR szFilePath); // saves as same name with .zip
	static BOOL ZipFolder(LPCTSTR szFilePath); // saves as same name with .zip

	// works with prior opened zip
	BOOL AddFileToZip(LPCTSTR szFileFullPath,LPCTSTR szPathInZip=NULL);
	BOOL AddFolderToZip(LPCTSTR szFolderFullPath,LPCTSTR szPathInZip=NULL);

	// extended interface
	BOOL OpenZip(LPCTSTR szFilePath, BOOL bAppend = FALSE);
	BOOL CloseZip(); // for multiple reuse

protected:
	void* m_uzFile;
};
