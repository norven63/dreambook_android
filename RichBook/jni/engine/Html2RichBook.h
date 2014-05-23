#pragma once

#include "RichTextLayout.h"

using namespace htmlcxx;

class CHtml2RichBook
{
	CStringA* msHtmlData;
	CBookWriter& mwriter;
	CStringA msDefaultCharset;	//转换时的字符集
	CString msFolder;
public:

	CHtml2RichBook(CStringA* htmlData,CBookWriter& writer,CStringA sCharset);
	~CHtml2RichBook();

	void SetFolder(CString sFolder);

	void MakeConvert();

protected:
	CString ConvertStr(const char* s)
	{
		if(msDefaultCharset.IsEmpty())
			return FromUtf8(s);
			
		return ToUnicode(msDefaultCharset,s);
	}
	void ConvertBody(tree<HTML::Node>::iterator itBody);
	void HandleHtmlTag(tree<HTML::Node>::sibling_iterator it,LPCSTR szTagName);
	void HandleSubTag(tree<HTML::Node>::sibling_iterator it);
	void HandleText(const char* szText);
};
