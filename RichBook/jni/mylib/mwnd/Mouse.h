#pragma once

class IMouseEvent;
class IGesture;
class CGestureBase;

class IMouseEvent
{
public:
	virtual BOOL Click(CPoint pt) { return FALSE; }
	virtual BOOL LongClick(CPoint pt) { return FALSE; }
	virtual BOOL Drag(CPoint ptFrom,CPoint ptCur) { return FALSE; }
};


class IGesture
{
	virtual BOOL OnGesture(CGestureBase* pGesture) { return FALSE; }
};

class CGestureBase : public CObject
{
protected:
	CArray<CGestureBase*> maToWait;
	IGesture* miEventTarget;
	BOOL mbRecognized;
public:

	CGestureBase(IGesture* pEventTarget);

	void StartRecognize();
	BOOL IsRecognized();

	BOOL IsNeedToWait();
	void NeedWait(CGestureBase* pGesture);

	// be called only when pressed
	virtual void HandleMouse(const CMouse& mouse) {}
};
