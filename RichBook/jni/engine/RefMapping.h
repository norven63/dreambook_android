#pragma once


struct SResFile
{
	int nType;		//资源类型EResType
	CString sResPath;
	CString	sNewPath;	//

	SResFile(int _type,CString sFile,CString _sNewPath=L""): 
		nType(_type),sResPath(sFile),sNewPath(_sNewPath) {}
};


class CResMapping
{
	std::map<CString,CString> mOrigin2New;
	std::map<CString,CString> mNew2Origin;
	int mnID;
	CString EncodeFilename(CString sFileName);
public:
	CResMapping();

	CString MapResFile(CString sOriginalPath);
	CString FindOriginFile(CString sNew);

	void GetAllOriginFiles(CArray<SResFile>& saFiles);
};