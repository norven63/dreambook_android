/*
	实现类似于iphone中的表格控件，
*/
#pragma once

class CBWTableCell : public CBaseWnd
{
public:
	CBWTableCell() { m_bgTransparent=TRUE; }

	// 派生类通常需要实现CalcHeight来进行重新布局
	// 在派生类的实现中，首先根据当前宽度对子窗口进行布局
	// 最后返回当前cell的高度
	virtual int UpdateLayout(int nWidth) = 0;
};
class CBWTableSection : public CBaseWnd
{
	DECLARE_DYNAMIC(CBWTableSection)
public:
	CBWTableSection() { m_bgTransparent=TRUE; }
	virtual ~CBWTableSection() { }

	virtual int CalcHeight(int nWidth);
};

class CBWTableWnd : public CBWScrollWnd
{
	DECLARE_SERIAL(CBWTableWnd)
	typedef CBWScrollWnd BaseClass;

public:

	CBWTableWnd();
	virtual ~CBWTableWnd();

	void ReloadData();

	virtual int GetSectionCount() { return 1;}
	virtual CBWTableSection* GetSection(int nSection) { return new CBWTableSection(); }
	virtual int GetCellCount(int nSection) { return 0; }
	virtual CBWTableCell* GetCell(int nSection,int nCell) { return NULL; }

protected:
	void UpdateLayout();
	virtual void OnSize(int cx,int cy) 
	{
		BaseClass::OnSize(cx,cy);

		UpdateLayout();
	}

	int m_nLastWidth;
};