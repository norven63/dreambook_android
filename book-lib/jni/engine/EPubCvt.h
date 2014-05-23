#pragma once

#include "RichTextLayout.h"

//////////////////////////////////////////////////////////////////////////
// Epub Function
class CEpubCvt
{
	CUnzipper mZip;
	CString msEpubPath;
	CString msContentPath;
	SBookInfo mInfo;

	CStringArray msaContentFullPath;	//所有内容项对应的全路径
	CStringArray msaContentTitle;

	void ParseMetaData(TiXmlElement* pElem);
public:

	BOOL OpenEpubFile(LPCTSTR szEpubPath);
	BOOL Convert2RichBook(CRichBook& book,BOOL bCalcLayout);
};
