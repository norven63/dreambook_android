#pragma once

#include "Platform.h"
#include "cstring.h"

//////////////////////////////////////////////////////////////////////////
// CSimpleList
// CTypedSimpleList
// CRuntimeClass
// CObject
// 
class CObject;
class CArchive;
struct CRuntimeClass;


/////////////////////////////////////////////////////////////////////////////
// CSimpleList (simple/small subset of CList)

class CSimpleList
{
public:
	CSimpleList(int nNextOffset = 0);
	void Construct(int nNextOffset);

	// Operations
	BOOL IsEmpty() const;
	void AddHead(void* p);
	void RemoveAll();
	void* GetHead() const;
	void* GetNext(void* p) const;
	BOOL Remove(void* p);

	// Implementation
	void* m_pHead;
	size_t m_nNextOffset;

	void** GetNextPtr(void* p) const;   // somewhat trusting...
};

inline void CSimpleList::AddHead(void* p)
{	
	*GetNextPtr(p) = m_pHead;
	m_pHead = p;
}

inline CSimpleList::CSimpleList(int nNextOffset)
{ m_pHead = NULL; m_nNextOffset = nNextOffset; }
inline void CSimpleList::Construct(int nNextOffset)
{ ASSERT(m_pHead == NULL); m_nNextOffset = nNextOffset; }
inline BOOL CSimpleList::IsEmpty() const
{ return m_pHead == NULL; }
inline void** CSimpleList::GetNextPtr(void* p) const
{ 
	return (void**)((BYTE*)p+m_nNextOffset); 
}
inline void CSimpleList::RemoveAll()
{ m_pHead = NULL; }
inline void* CSimpleList::GetHead() const
{ return m_pHead; }
inline void* CSimpleList::GetNext(void* prevElement) const
{ return *GetNextPtr(prevElement); }

//////////////////////////////////////////////////////////////////////////
// CTypedSimpleList
template<class TYPE>
class CTypedSimpleList : public CSimpleList
{
public:
	CTypedSimpleList(int nNextOffset = 0)
		: CSimpleList(nNextOffset) { }
	void AddHead(TYPE p)
	{ CSimpleList::AddHead(p); }
	TYPE GetHead()
	{ return (TYPE)CSimpleList::GetHead(); }
	TYPE GetNext(TYPE p)
	{ return (TYPE)CSimpleList::GetNext(p); }
	BOOL Remove(TYPE p)
	{ return CSimpleList::Remove((TYPE)p); }
	operator TYPE();		
};
template<class TYPE>
inline CTypedSimpleList<TYPE>::operator TYPE()
{ 
	return (TYPE)CSimpleList::GetHead(); 
}


//////////////////////////////////////////////////////////////////////////
void AfxClassInit(CRuntimeClass* pNewClass);
struct AFX_CLASSINIT
{
	AFX_CLASSINIT(CRuntimeClass* pNewClass) { AfxClassInit(pNewClass); } 
};

///////////////////////////////////////////////////////////////////////////
// CRuntimeClass

struct CRuntimeClass
{
	// Attributes
	LPCSTR m_lpszClassName;
	int m_nObjectSize;
	UINT m_wSchema; // schema number of the loaded class
	CObject* (*m_pfnCreateObject)(); // NULL => abstract class
	CRuntimeClass* m_pBaseClass;

	// Operations
	CObject* CreateObject();
	BOOL IsDerivedFrom(const CRuntimeClass* pBaseClass) const;

	// dynamic name lookup and creation
	static CRuntimeClass*  FromName(LPCSTR lpszClassName);
	static CObject*  CreateObject(LPCSTR lpszClassName);
	static CObject*  CreateObject(LPCWSTR lpszClassName);

	// Implementation
	void Store(CArchive& ar) const;
	static CRuntimeClass*  Load(CArchive& ar, UINT* pwSchemaNum);

	// CRuntimeClass objects linked together in simple list
	CRuntimeClass* m_pNextClass;       // linked list of registered classes
	const AFX_CLASSINIT* m_pClassInit;
};

//////////////////////////////////////////////////////////////////////////
// CObject

class CObject
{
public:

	// Object model (types, destruction, allocation)
	virtual CRuntimeClass* GetRuntimeClass() const;
	virtual ~CObject() {}  // virtual destructors are necessary

	// Diagnostic allocations
// 	void* PASCAL operator new(size_t nSize);
// 	void* PASCAL operator new(size_t, void* p);
// 	void PASCAL operator delete(void* p);

// #if defined(_DEBUG) && !defined(_AFX_NO_DEBUG_CRT)
// 	// for file name/line number tracking using DEBUG_NEW
// 	void* PASCAL operator new(size_t nSize, LPCSTR lpszFileName, int nLine);
// #if _MSC_VER >= 1200
// 	void PASCAL operator delete(void *p, LPCSTR lpszFileName, int nLine);
// #endif
// #endif

	// Disable the copy constructor and assignment by default so you will get
	//   compiler errors instead of unexpected behaviour if you pass objects
	//   by value or assign objects.
protected:
	CObject(){}
private:
	CObject(const CObject& objectSrc);              // no implementation
	void operator=(const CObject& objectSrc);       // no implementation

	// Attributes
public:
	BOOL IsSerializable() const 
	{
		return (GetRuntimeClass()->m_wSchema != 0xffff); 
	}
	BOOL IsKindOf(const CRuntimeClass* pClass) const
	{
		CRuntimeClass* pClassThis = GetRuntimeClass();
		return pClassThis->IsDerivedFrom(pClass);
	}

	// Overridables
	virtual void Serialize(CArchive& ar)
	{
	}

	// Implementation
public:
	static const CRuntimeClass classCObject;
};

//////////////////////////////////////////////////////////////////////////
// Helper macros
#define _RUNTIME_CLASS(class_name) ((CRuntimeClass*)(&class_name::class##class_name))
#define RUNTIME_CLASS(class_name) _RUNTIME_CLASS(class_name)

//////////////////////////////////////////////////////////////////////////////
// Helper macros for declaring CRuntimeClass compatible classes

#define DECLARE_DYNAMIC(class_name) \
public: \
	static const CRuntimeClass class##class_name; \
	virtual CRuntimeClass* GetRuntimeClass() const; \

#define _DECLARE_DYNAMIC(class_name) \
public: \
	static CRuntimeClass class##class_name; \
	virtual CRuntimeClass* GetRuntimeClass() const; \

// not serializable, but dynamically constructable
#define DECLARE_DYNCREATE(class_name) \
	DECLARE_DYNAMIC(class_name) \
	static CObject* PASCAL CreateObject();

#define _DECLARE_DYNCREATE(class_name) \
	_DECLARE_DYNAMIC(class_name) \
	static CObject* PASCAL CreateObject();

#define DECLARE_SERIAL(class_name) \
	_DECLARE_DYNCREATE(class_name) \
	 friend CArchive&  operator>>(CArchive& ar, class_name* &pOb);

//////////////////////////////////////////////////////////////////////////
// IMPLEMENT
#define IMPLEMENT_RUNTIMECLASS(class_name, base_class_name, wSchema, pfnNew, class_init) \
	 const CRuntimeClass class_name::class##class_name = { \
#class_name, sizeof(class class_name), wSchema, pfnNew, \
	RUNTIME_CLASS(base_class_name), NULL, class_init }; \
	CRuntimeClass* class_name::GetRuntimeClass() const \
{ return RUNTIME_CLASS(class_name); } \

#define _IMPLEMENT_RUNTIMECLASS(class_name, base_class_name, wSchema, pfnNew, class_init) \
	 CRuntimeClass class_name::class##class_name = { \
#class_name, sizeof(class class_name), wSchema, pfnNew, \
	RUNTIME_CLASS(base_class_name), NULL, class_init }; \
	CRuntimeClass* class_name::GetRuntimeClass() const \
{ return RUNTIME_CLASS(class_name); } \

#define IMPLEMENT_RUNTIMECLASS_T(class_name, T1, base_class_name, wSchema, pfnNew, class_init) \
	 const CRuntimeClass class_name<T1>::class##class_name = { \
#class_name, sizeof(class class_name<T1>), wSchema, pfnNew, \
	RUNTIME_CLASS(base_class_name), NULL, class_init }; \
	CRuntimeClass* class_name<T1>::GetRuntimeClass() const \
{ return RUNTIME_CLASS_T(class_name, T1); } \

#define _IMPLEMENT_RUNTIMECLASS_T(class_name, T1, base_class_name, wSchema, pfnNew, class_init) \
	 CRuntimeClass class_name<T1>::class##class_name = { \
#class_name, sizeof(class class_name<T1>), wSchema, pfnNew, \
	RUNTIME_CLASS(base_class_name), NULL, class_init }; \
	CRuntimeClass* class_name<T1>::GetRuntimeClass() const \
{ return RUNTIME_CLASS_T(class_name, T1); } \

#define IMPLEMENT_RUNTIMECLASS_T2(class_name, T1, T2, base_class_name, wSchema, pfnNew, class_init) \
	 const CRuntimeClass class_name<T1, T2>::class##class_name = { \
#class_name, sizeof(class class_name<T1, T2>), wSchema, pfnNew, \
	RUNTIME_CLASS(base_class_name), NULL, class_init }; \
	CRuntimeClass* class_name<T1, T2>::GetRuntimeClass() const \
{ return RUNTIME_CLASS_T2(class_name, T1, T2); } \

#define _IMPLEMENT_RUNTIMECLASS_T2(class_name, T1, T2, base_class_name, wSchema, pfnNew, class_init) \
	 CRuntimeClass class_name<T1, T2>::class##class_name = { \
#class_name, sizeof(class class_name<T1, T2>), wSchema, pfnNew, \
	RUNTIME_CLASS(base_class_name), NULL, class_init }; \
	CRuntimeClass* class_name<T1, T2>::GetRuntimeClass() const \
{ return RUNTIME_CLASS_T2(class_name, T1, T2); } \


#define IMPLEMENT_DYNAMIC(class_name, base_class_name) \
	IMPLEMENT_RUNTIMECLASS(class_name, base_class_name, 0xFFFF, NULL, NULL)

#define IMPLEMENT_DYNCREATE(class_name, base_class_name) \
	CObject* PASCAL class_name::CreateObject() \
		{ return new class_name; } \
		IMPLEMENT_RUNTIMECLASS(class_name, base_class_name, 0xFFFF, \
		class_name::CreateObject, NULL)

#define IMPLEMENT_SERIAL(class_name, base_class_name, wSchema) \
	CObject* PASCAL class_name::CreateObject() \
		{ return new class_name; } \
		extern AFX_CLASSINIT _init_##class_name; \
		_IMPLEMENT_RUNTIMECLASS(class_name, base_class_name, wSchema, \
		class_name::CreateObject, &_init_##class_name) \
		AFX_CLASSINIT _init_##class_name(RUNTIME_CLASS(class_name)); \
		CArchive&  operator>>(CArchive& ar, class_name* &pOb) \
		{ pOb = (class_name*) ar.ReadObject(RUNTIME_CLASS(class_name)); \
		return ar; } \

#define IMPLEMENT_DYNAMIC_T(class_name, T1, base_class_name) \
	template class class_name<T1>; \
	IMPLEMENT_RUNTIMECLASS_T(class_name, T1, base_class_name, 0xFFFF, NULL, NULL)

#define IMPLEMENT_DYNCREATE_T(class_name, T1, base_class_name) \
	template class class_name<T1>; \
	CObject* PASCAL class_name<T1>::CreateObject() \
		{ return new class_name<T1>; } \
		IMPLEMENT_RUNTIMECLASS_T(class_name, T1, base_class_name, 0xFFFF, \
		class_name<T1>::CreateObject, NULL)

#define IMPLEMENT_SERIAL_T(class_name, T1, base_class_name, wSchema) \
	template class class_name<T1>; \
	CObject* PASCAL class_name<T1>::CreateObject() \
		{ return new class_name<T1>; } \
		extern AFX_CLASSINIT _init_##class_name##T1; \
		_IMPLEMENT_RUNTIMECLASS_T(class_name, T1, base_class_name, wSchema, \
		class_name<T1>::CreateObject, &_init_##class_name##T1) \
		AFX_CLASSINIT _init_##class_name##T1(RUNTIME_CLASS_T(class_name, T1)); \
		CArchive&  operator>>(CArchive& ar, class_name<T1>* &pOb) \
		{ pOb = (class_name<T1>*) ar.ReadObject(RUNTIME_CLASS_T(class_name, T1)); \
		return ar; } \

#define IMPLEMENT_DYNAMIC_T2(class_name, T1, T2, base_class_name) \
	template class class_name<T1, T2>; \
	IMPLEMENT_RUNTIMECLASS_T2(class_name, T1, T2, base_class_name, 0xFFFF, NULL, NULL)

#define IMPLEMENT_DYNCREATE_T2(class_name, T1, T2, base_class_name) \
	template class class_name<T1, T2>; \
	CObject* PASCAL class_name<T1, T2>::CreateObject() \
		{ return new class_name<T1, T2>; } \
		IMPLEMENT_RUNTIMECLASS_T2(class_name, T1, T2, base_class_name, 0xFFFF, \
		class_name<T1, T2>::CreateObject, NULL)

#define IMPLEMENT_SERIAL_T2(class_name, T1, T2, base_class_name, wSchema) \
	template class class_name<T1, T2>; \
	CObject* PASCAL class_name<T1, T2>::CreateObject() \
		{ return new class_name<T1, T2>; } \
		extern AFX_CLASSINIT _init_##class_name##T1##T2; \
		_IMPLEMENT_RUNTIMECLASS_T2(class_name, T1, T2, base_class_name, wSchema, \
		class_name<T1, T2>::CreateObject, &_init_##class_name##T1##T2) \
		AFX_CLASSINIT _init_##class_name##T1##T2(RUNTIME_CLASS_T2(class_name, T1, T2)); \
		CArchive&  operator>>(CArchive& ar, class_name<T1, T2>* &pOb) \
		{ pOb = (class_name<T1, T2>*) ar.ReadObject(RUNTIME_CLASS_T2(class_name, T1, T2)); \
		return ar; } \

// optional bit for schema number that enables object versioning
#define VERSIONABLE_SCHEMA  (0x80000000)



