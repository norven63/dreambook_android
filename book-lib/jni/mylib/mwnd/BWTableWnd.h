/*
	ʵ��������iphone�еı��ؼ���
*/
#pragma once

class CBWTableCell : public CBaseWnd
{
public:
	CBWTableCell() { m_bgTransparent=TRUE; }

	// ������ͨ����Ҫʵ��CalcHeight���������²���
	// ���������ʵ���У����ȸ��ݵ�ǰ��ȶ��Ӵ��ڽ��в���
	// ��󷵻ص�ǰcell�ĸ߶�
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