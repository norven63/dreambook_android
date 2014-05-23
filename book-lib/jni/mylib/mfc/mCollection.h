/*
 *  Collection.h
 *  DNav
 *
 *  Created by letu on 1/7/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once


#include "mBaseType.h"
#include "mArchive.h"
#include "mCore.h"
#include <algorithm>
#include "cfunc.h"

#define BEFORE_START_POSITION ((POSITION)-1L)

template<class TYPE, class ARG_TYPE>
BOOL CompareElements(const TYPE* pElement1, const ARG_TYPE* pElement2)
{
	return *pElement1 == *pElement2;
}

template<class ARG_KEY>
UINT HashKey(ARG_KEY key)
{
	// default identity hash - works for most primitive values
	return (DWORD)(((DWORD)key)>>4);
}

template<class TYPE>
class CArray
{
public:
	CArray(){};
	CArray(const CArray<TYPE>& ary) { Append(ary); }
	virtual ~CArray(){};

	BOOL IsEmpty() const { return m_v.empty(); }

	int GetUpperBound() const { return (int)m_v.size()-1; }
	void SetSize(int nNewSize) { m_v.resize(nNewSize); }
	int GetSize() const {return (int) m_v.size();}
	TYPE& GetAt(int nIndex) {return m_v[nIndex];}
	TYPE& At(int nIndex) {return m_v[nIndex];}

	const TYPE& GetAt(int nIndex) const {return m_v[nIndex];}
	void SetAt(int nIndex,const TYPE& v) { m_v[nIndex]=v; }
	void InsertAt(int nIndex, TYPE newElement, int nCount = 1)
	{
		m_v.insert(m_v.begin()+nIndex,nCount,newElement) ;
	}

	TYPE& GetFirst() { return m_v.front(); }
	TYPE& GetLast() { return m_v.back(); }


	TYPE& operator[](int nIndex)
	{
		return m_v[nIndex];
	}
	const TYPE& operator[](int nIndex) const
	{
		return m_v[nIndex];
	}
	int GetCount() const {return (int)m_v.size();}
	int Count() const {return (int)m_v.size();}
	void Add(const TYPE& t){m_v.push_back(t);};
	void RemoveAll(){m_v.clear();};
	void RemoveAt(int nIndex)
	{
		if (nIndex < 0 || nIndex >= (int) m_v.size())
			return;
		
		m_v.erase(m_v.begin()+nIndex);
	};	
	int Find(const TYPE& el, int startAt=0) const 
	{
		for (int i = startAt; i < GetCount(); i++) {
			if (m_v[i] == el)
				return (int)i;
		}
		return -1;
	}
	BOOL Remove(const TYPE& el) 
	{
		int i = Find(el);
		if (-1 == i)
			return FALSE;
		RemoveAt(i);
		return TRUE;
	}
	void RemoveLast()
	{
		if (!m_v.empty())
		{
			RemoveAt(GetSize()-1);
		}
	}

	int Append(const CArray<TYPE>& src)
	{
		if(this == &src)
		{
			return (int)GetSize() ;
		}

		int nOldSize = (int)GetSize();
		
		for(int i =0;i<src.GetSize();i++)
		{
			Add(src[i]);
		}

		return nOldSize;
	}

	const TYPE* GetData() const
	{
		if(m_v.size()>0)
			return &m_v[0];
		else
			return NULL;
	}
	TYPE* GetData()
	{
		if(m_v.size()>0)
			return &m_v[0];
		else
			return NULL;
	}

	void Reverse()
	{
		int nSize=GetCount();
		int nLoop=nSize/2;
		TYPE temp;
		for (int i=0;i<nLoop;i++)
		{
			temp = m_v[nSize-i-1];
			m_v[nSize-i-1]=m_v[i];
			m_v[i]=temp;
		}
	}

	template<typename T>
	void Sort(int (*cmpFunc)(const T& a, const T& b)) 
	{
		std::sort(m_v.begin(),m_v.end(),cmpFunc);
	}
private:	
	std::vector<TYPE> m_v;
};



class CStringArray
{
public:
	int GetCount() const 
	{
		return (int)array.size();
	}
	int Count() const 
	{
		return (int)array.size();
	}

	int GetSize() const 
	{
		return (int)GetCount();
	}

	void SetSize(int nSize)
	{
		array.resize(nSize);
	}

	const CString& operator[](int i) const
	{
		assert(i<(int)array.size()) ;
		assert(i>=0);
		return array[i];
	}
	CString& operator[](int i)
	{
		assert(i<(int)array.size()) ;
		assert(i>=0);
		return array[i];
	}

	const CString& GetAt(int i) const
	{
		return array[i];
	}
	CString& At(int i) 
	{
		return array[i];
	}
	void SetAt(int index,LPCTSTR szString)
	{
		assert(index<(int)array.size());
		array[index] = szString;
	}

	void Add(CString str)
	{
		array.push_back(str);
	}

	void Append(const CStringArray& arr)
	{
		for (int i=0; i<(int)arr.GetSize(); ++i)
			array.push_back(arr[i]);
	}

	void RemoveAt(int nIndex)
	{
		if (nIndex < 0 || nIndex >= (int) array.size())
			return;

		array.erase(array.begin()+nIndex);
	}

	void RemoveAll()
	{
		array.clear();
	}

	BOOL IsEmpty() const
	{
		return array.size()==0;
	}
	int GetUpperBound() const
	{
		return GetSize()-1;
	}

	void Serialize(CArchive& ar)
	{
		if (ar.IsStoring())
		{
			ar.WriteCount(DWORD(array.size()));
			std::vector<CString>::iterator it;
			for( it = array.begin(); it != array.end(); ++it)
			{
				//type& t = *it;
				ar << (*it);
			}
		}
		else
		{
			DWORD dwCount = ar.ReadCount();
			array.resize(dwCount);
			std::vector<CString>::iterator it;
			for( it = array.begin(); it != array.end(); ++it )
			{
				//type& t = *it;
				ar >> (*it);
			}	
		}
	}
	int Find(const TCHAR *string, int startAt=0) const {
		for (int i = startAt; i < GetSize(); i++) {
			const TCHAR *item = (LPCTSTR) GetAt(i);
			if (tcscmp(string, item))
				return (int)i;
		}
		return -1;
	}

	int FindI(const TCHAR *string, int startAt=0) const {
		for (int i = startAt; i < GetSize(); i++) {
			const TCHAR *item = (LPCTSTR)GetAt(i);
			if (tcscmp(string, item)==0)
				return (int)i;
		}
		return -1;
	}
	void Sort()
	{
		std::sort(array.begin(),array.end(),cmpAscii);
	}
	void SortBy(int (*cmpfunc)(const TCHAR *a, const TCHAR *b))
	{
		std::sort(array.begin(),array.end(),cmpfunc);
	}
	CString Join(const TCHAR *joint=NULL) {
		CString tmp;
		int jointLen = joint ? tcslen(joint) : 0;
		for (int i = 0; i < GetSize(); i++) {
			const TCHAR *s = At(i);
			if (i > 0 && jointLen > 0)
				tmp.Append(joint);
			tmp.Append(s);
		}
		return tmp;
	}
private:
	static int cmpAscii(const void *a, const void *b)
	{
		return tcscmp(*(const TCHAR **)a, *(const TCHAR **)b);
	}
	
	std::vector<CString> array;
};

typedef CArray<UINT> CUIntArray;


/////////////////////////////////////////////////////////////////////////////
// CList<TYPE, ARG_TYPE>

struct CPlex     // warning variable length structure
{
	CPlex* pNext;
	DWORD dwReserved[1];    // align on 8 byte boundary

	// BYTE data[maxNum*elementSize];

	void* data() { return this+1; }

	static CPlex* PASCAL Create(CPlex*& pHead, UINT nMax, UINT cbElement)
	{
		ASSERT(nMax > 0 && cbElement > 0);
		if (nMax == 0 || cbElement == 0)
			return NULL ;

		CPlex* p = (CPlex*) new BYTE[sizeof(CPlex) + nMax * cbElement];
		// may throw exception
		p->pNext = pHead;
		pHead = p;  // change head (adds in reverse order for simplicity)
		return p;
	}
	// like 'calloc' but no zero fill
	// may throw memory exceptions

	void FreeDataChain()       // free this one and links
	{
		CPlex* p = this;
		while (p != NULL)
		{
			BYTE* bytes = (BYTE*) p;
			CPlex* pNext = p->pNext;
			delete[] bytes;
			p = pNext;
		}
	}
};

template<class TYPE, class ARG_TYPE = const TYPE&>
class CList 
{
protected:
	struct CNode
	{
		CNode* pNext;
		CNode* pPrev;
		TYPE data;
	};
public:
	// Construction
	/* explicit */ CList(int nBlockSize = 10);

	// Attributes (head and tail)
	// count of elements
	int GetCount() const;
	int GetSize() const;
	BOOL IsEmpty() const;

	// peek at head or tail
	TYPE& GetHead();
	const TYPE& GetHead() const;
	TYPE& GetTail();
	const TYPE& GetTail() const;

	// Operations
	// get head or tail (and remove it) - don't call on empty list !
	TYPE RemoveHead();
	TYPE RemoveTail();

	// add before head or after tail
	POSITION AddHead(ARG_TYPE newElement);
	POSITION AddTail(ARG_TYPE newElement);

	// add another list of elements before head or after tail
	void AddHead(CList* pNewList);
	void AddTail(CList* pNewList);

	// remove all elements
	void RemoveAll();

	// iteration
	POSITION GetHeadPosition() const;
	POSITION GetTailPosition() const;
	TYPE& GetNext(POSITION& rPosition); // return *Position++
	const TYPE& GetNext(POSITION& rPosition) const; // return *Position++
	TYPE& GetPrev(POSITION& rPosition); // return *Position--
	const TYPE& GetPrev(POSITION& rPosition) const; // return *Position--

	// getting/modifying an element at a given position
	TYPE& GetAt(POSITION position);
	const TYPE& GetAt(POSITION position) const;
	void SetAt(POSITION pos, ARG_TYPE newElement);
	void RemoveAt(POSITION position);

	// inserting before or after a given position
	POSITION InsertBefore(POSITION position, ARG_TYPE newElement);
	POSITION InsertAfter(POSITION position, ARG_TYPE newElement);

	// helper functions (note: O(n) speed)
	POSITION Find(ARG_TYPE searchValue, POSITION startAfter = NULL) const;
	// defaults to starting at the HEAD, return NULL if not found
	POSITION FindIndex(int nIndex) const;
	// get the 'nIndex'th element (may return NULL)

	// Implementation
protected:
	CNode* m_pNodeHead;
	CNode* m_pNodeTail;
	int m_nCount;
	CNode* m_pNodeFree;
	struct CPlex* m_pBlocks;
	int m_nBlockSize;

	CNode* NewNode(CNode*, CNode*);
	void FreeNode(CNode*);

public:
	~CList();
};

/////////////////////////////////////////////////////////////////////////////
// CList<TYPE, ARG_TYPE>  functions

template<class TYPE, class ARG_TYPE>
 int CList<TYPE, ARG_TYPE>::GetCount() const
{ return m_nCount; }
template<class TYPE, class ARG_TYPE>
 int CList<TYPE, ARG_TYPE>::GetSize() const
{ return m_nCount; }
template<class TYPE, class ARG_TYPE>
 BOOL CList<TYPE, ARG_TYPE>::IsEmpty() const
{ return m_nCount == 0; }
template<class TYPE, class ARG_TYPE>
 TYPE& CList<TYPE, ARG_TYPE>::GetHead()
{ ASSERT(m_pNodeHead != NULL);
return m_pNodeHead->data; }
template<class TYPE, class ARG_TYPE>
 const TYPE& CList<TYPE, ARG_TYPE>::GetHead() const
{ ASSERT(m_pNodeHead != NULL);
return m_pNodeHead->data; }
template<class TYPE, class ARG_TYPE>
 TYPE& CList<TYPE, ARG_TYPE>::GetTail()
{ ASSERT(m_pNodeTail != NULL);
return m_pNodeTail->data; }
template<class TYPE, class ARG_TYPE>
 const TYPE& CList<TYPE, ARG_TYPE>::GetTail() const
{ ASSERT(m_pNodeTail != NULL);
return m_pNodeTail->data; }
template<class TYPE, class ARG_TYPE>
 POSITION CList<TYPE, ARG_TYPE>::GetHeadPosition() const
{ return (POSITION) m_pNodeHead; }
template<class TYPE, class ARG_TYPE>
 POSITION CList<TYPE, ARG_TYPE>::GetTailPosition() const
{ return (POSITION) m_pNodeTail; }
template<class TYPE, class ARG_TYPE>
 TYPE& CList<TYPE, ARG_TYPE>::GetNext(POSITION& rPosition) // return *Position++
{ CNode* pNode = (CNode*) rPosition;
rPosition = (POSITION) pNode->pNext;
return pNode->data; }
template<class TYPE, class ARG_TYPE>
 const TYPE& CList<TYPE, ARG_TYPE>::GetNext(POSITION& rPosition) const // return *Position++
{ CNode* pNode = (CNode*) rPosition;
rPosition = (POSITION) pNode->pNext;
return pNode->data; }
template<class TYPE, class ARG_TYPE>
 TYPE& CList<TYPE, ARG_TYPE>::GetPrev(POSITION& rPosition) // return *Position--
{ CNode* pNode = (CNode*) rPosition;
rPosition = (POSITION) pNode->pPrev;
return pNode->data; }
template<class TYPE, class ARG_TYPE>
 const TYPE& CList<TYPE, ARG_TYPE>::GetPrev(POSITION& rPosition) const // return *Position--
{ CNode* pNode = (CNode*) rPosition;
rPosition = (POSITION) pNode->pPrev;
return pNode->data; }
template<class TYPE, class ARG_TYPE>
 TYPE& CList<TYPE, ARG_TYPE>::GetAt(POSITION position)
{ CNode* pNode = (CNode*) position;
return pNode->data; }
template<class TYPE, class ARG_TYPE>
 const TYPE& CList<TYPE, ARG_TYPE>::GetAt(POSITION position) const
{ CNode* pNode = (CNode*) position;
return pNode->data; }
template<class TYPE, class ARG_TYPE>
 void CList<TYPE, ARG_TYPE>::SetAt(POSITION pos, ARG_TYPE newElement)
{ CNode* pNode = (CNode*) pos;
pNode->data = newElement; }

template<class TYPE, class ARG_TYPE>
CList<TYPE, ARG_TYPE>::CList(int nBlockSize)
{
	ASSERT(nBlockSize > 0);

	m_nCount = 0;
	m_pNodeHead = m_pNodeTail = m_pNodeFree = NULL;
	m_pBlocks = NULL;
	m_nBlockSize = nBlockSize;
}

template<class TYPE, class ARG_TYPE>
void CList<TYPE, ARG_TYPE>::RemoveAll()
{
	// destroy elements
	CNode* pNode;
	for (pNode = m_pNodeHead; pNode != NULL; pNode = pNode->pNext)
		pNode->data.~TYPE();

	m_nCount = 0;
	m_pNodeHead = m_pNodeTail = m_pNodeFree = NULL;
	m_pBlocks->FreeDataChain();
	m_pBlocks = NULL;
}

template<class TYPE, class ARG_TYPE>
CList<TYPE, ARG_TYPE>::~CList()
{
	RemoveAll();
	ASSERT(m_nCount == 0);
}

/////////////////////////////////////////////////////////////////////////////
// Node helpers
//
// Implementation note: CNode's are stored in CPlex blocks and
//  chained together. Free blocks are maintained in a singly linked list
//  using the 'pNext' member of CNode with 'm_pNodeFree' as the head.
//  Used blocks are maintained in a doubly linked list using both 'pNext'
//  and 'pPrev' as links and 'm_pNodeHead' and 'm_pNodeTail'
//   as the head/tail.
//
// We never free a CPlex block unless the List is destroyed or RemoveAll()
//  is used - so the total number of CPlex blocks may grow large depending
//  on the maximum past size of the list.
//

template<class TYPE, class ARG_TYPE>
typename CList<TYPE, ARG_TYPE>::CNode*
CList<TYPE, ARG_TYPE>::NewNode(CNode* pPrev, CNode* pNext)
{
	if (m_pNodeFree == NULL)
	{
		// add another block
		CPlex* pNewBlock = CPlex::Create(m_pBlocks, m_nBlockSize,
			sizeof(CNode));

		// chain them into free list
		CNode* pNode = (CNode*) pNewBlock->data();
		// free in reverse order to make it easier to debug
		pNode += m_nBlockSize - 1;
		for (int i = m_nBlockSize-1; i >= 0; i--, pNode--)
		{
			pNode->pNext = m_pNodeFree;
			m_pNodeFree = pNode;
		}
	}
	CNode* pNode = m_pNodeFree;
	m_pNodeFree = m_pNodeFree->pNext;
	pNode->pPrev = pPrev;
	pNode->pNext = pNext;
	m_nCount++;
	ASSERT(m_nCount > 0);  // make sure we don't overflow

#pragma push_macro("new")
#undef new
	::new( (void*)( &pNode->data ) ) TYPE;
#pragma pop_macro("new")
	return pNode;
}

template<class TYPE, class ARG_TYPE>
void CList<TYPE, ARG_TYPE>::FreeNode(CNode* pNode)
{
	pNode->data.~TYPE();
	pNode->pNext = m_pNodeFree;
	m_pNodeFree = pNode;
	m_nCount--;
	ASSERT(m_nCount >= 0);  // make sure we don't underflow

	// if no more elements, cleanup completely
	if (m_nCount == 0)
		RemoveAll();
}

template<class TYPE, class ARG_TYPE>
POSITION CList<TYPE, ARG_TYPE>::AddHead(ARG_TYPE newElement)
{

	CNode* pNewNode = NewNode(NULL, m_pNodeHead);
	pNewNode->data = newElement;
	if (m_pNodeHead != NULL)
		m_pNodeHead->pPrev = pNewNode;
	else
		m_pNodeTail = pNewNode;
	m_pNodeHead = pNewNode;
	return (POSITION) pNewNode;
}

template<class TYPE, class ARG_TYPE>
POSITION CList<TYPE, ARG_TYPE>::AddTail(ARG_TYPE newElement)
{

	CNode* pNewNode = NewNode(m_pNodeTail, NULL);
	pNewNode->data = newElement;
	if (m_pNodeTail != NULL)
		m_pNodeTail->pNext = pNewNode;
	else
		m_pNodeHead = pNewNode;
	m_pNodeTail = pNewNode;
	return (POSITION) pNewNode;
}

template<class TYPE, class ARG_TYPE>
void CList<TYPE, ARG_TYPE>::AddHead(CList* pNewList)
{
	// add a list of same elements to head (maintain order)
	POSITION pos = pNewList->GetTailPosition();
	while (pos != NULL)
		AddHead(pNewList->GetPrev(pos));
}

template<class TYPE, class ARG_TYPE>
void CList<TYPE, ARG_TYPE>::AddTail(CList* pNewList)
{

	// add a list of same elements
	POSITION pos = pNewList->GetHeadPosition();
	while (pos != NULL)
		AddTail(pNewList->GetNext(pos));
}

template<class TYPE, class ARG_TYPE>
TYPE CList<TYPE, ARG_TYPE>::RemoveHead()
{
	ASSERT(m_pNodeHead != NULL);  // don't call on empty list !!!

	CNode* pOldNode = m_pNodeHead;
	TYPE returnValue = pOldNode->data;

	m_pNodeHead = pOldNode->pNext;
	if (m_pNodeHead != NULL)
		m_pNodeHead->pPrev = NULL;
	else
		m_pNodeTail = NULL;
	FreeNode(pOldNode);
	return returnValue;
}

template<class TYPE, class ARG_TYPE>
TYPE CList<TYPE, ARG_TYPE>::RemoveTail()
{
	ASSERT(m_pNodeTail != NULL);  // don't call on empty list !!!

	CNode* pOldNode = m_pNodeTail;
	TYPE returnValue = pOldNode->data;

	m_pNodeTail = pOldNode->pPrev;
	if (m_pNodeTail != NULL)
		m_pNodeTail->pNext = NULL;
	else
		m_pNodeHead = NULL;
	FreeNode(pOldNode);
	return returnValue;
}

template<class TYPE, class ARG_TYPE>
POSITION CList<TYPE, ARG_TYPE>::InsertBefore(POSITION position, ARG_TYPE newElement)
{

	if (position == NULL)
		return AddHead(newElement); // insert before nothing -> head of the list

	// Insert it before position
	CNode* pOldNode = (CNode*) position;
	CNode* pNewNode = NewNode(pOldNode->pPrev, pOldNode);
	pNewNode->data = newElement;

	if (pOldNode->pPrev != NULL)
	{
		pOldNode->pPrev->pNext = pNewNode;
	}
	else
	{
		ASSERT(pOldNode == m_pNodeHead);
		m_pNodeHead = pNewNode;
	}
	pOldNode->pPrev = pNewNode;
	return (POSITION) pNewNode;
}

template<class TYPE, class ARG_TYPE>
POSITION CList<TYPE, ARG_TYPE>::InsertAfter(POSITION position, ARG_TYPE newElement)
{
	if (position == NULL)
		return AddTail(newElement); // insert after nothing -> tail of the list

	// Insert it before position
	CNode* pOldNode = (CNode*) position;
	CNode* pNewNode = NewNode(pOldNode, pOldNode->pNext);
	pNewNode->data = newElement;

	if (pOldNode->pNext != NULL)
	{
		pOldNode->pNext->pPrev = pNewNode;
	}
	else
	{
		ASSERT(pOldNode == m_pNodeTail);
		m_pNodeTail = pNewNode;
	}
	pOldNode->pNext = pNewNode;
	return (POSITION) pNewNode;
}

template<class TYPE, class ARG_TYPE>
void CList<TYPE, ARG_TYPE>::RemoveAt(POSITION position)
{
	CNode* pOldNode = (CNode*) position;

	// remove pOldNode from list
	if (pOldNode == m_pNodeHead)
	{
		m_pNodeHead = pOldNode->pNext;
	}
	else
	{
		pOldNode->pPrev->pNext = pOldNode->pNext;
	}
	if (pOldNode == m_pNodeTail)
	{
		m_pNodeTail = pOldNode->pPrev;
	}
	else
	{
		pOldNode->pNext->pPrev = pOldNode->pPrev;
	}
	FreeNode(pOldNode);
}

template<class TYPE, class ARG_TYPE>
POSITION CList<TYPE, ARG_TYPE>::FindIndex(int nIndex) const
{

	if (nIndex >= m_nCount || nIndex < 0)
		return NULL;  // went too far

	CNode* pNode = m_pNodeHead;
	while (nIndex--)
	{
		pNode = pNode->pNext;
	}
	return (POSITION) pNode;
}

template<class TYPE, class ARG_TYPE>
POSITION CList<TYPE, ARG_TYPE>::Find(ARG_TYPE searchValue, POSITION startAfter) const
{

	CNode* pNode = (CNode*) startAfter;
	if (pNode == NULL)
	{
		pNode = m_pNodeHead;  // start at head
	}
	else
	{
		pNode = pNode->pNext;  // start after the one specified
	}

	for (; pNode != NULL; pNode = pNode->pNext)
		if (CompareElements<TYPE>(&pNode->data, &searchValue))
			return (POSITION)pNode;
	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
class CMap 
{
public:
	// CPair
	struct CPair
	{
		const KEY key;
		VALUE value;
	protected:
		CPair( ARG_KEY keyval ) : key( keyval )	{}
	};

protected:
	// Association
	class CAssoc : public CPair
	{
		friend class CMap<KEY,ARG_KEY,VALUE,ARG_VALUE>;
		CAssoc* pNext;
		UINT nHashValue;  // needed for efficient iteration
	public:
		CAssoc( ARG_KEY key ) : CPair( key ) {}
	};

public:
	// Construction
	/* explicit */ CMap(int nBlockSize = 10);

	// Attributes
	// number of elements
	int GetCount() const;
	int GetSize() const;
	BOOL IsEmpty() const;

	// Lookup
	BOOL Lookup(ARG_KEY key, VALUE& rValue) const;
	const CPair *PLookup(ARG_KEY key) const;
	CPair *PLookup(ARG_KEY key);

	// Operations
	// Lookup and add if not there
	VALUE& operator[](ARG_KEY key);

	// add a new (key, value) pair
	void SetAt(ARG_KEY key, ARG_VALUE newValue);

	// removing existing (key, ?) pair
	BOOL RemoveKey(ARG_KEY key);
	void RemoveAll();

	// iterating all (key, value) pairs
	POSITION GetStartPosition() const;

	const CPair *PGetFirstAssoc() const;
	CPair *PGetFirstAssoc();

	void GetNextAssoc(POSITION& rNextPosition, KEY& rKey, VALUE& rValue) const;

	const CPair *PGetNextAssoc(const CPair *pAssocRet) const;
	CPair *PGetNextAssoc(const CPair *pAssocRet);

	// advanced features for derived classes
	UINT GetHashTableSize() const;
	void InitHashTable(UINT hashSize, BOOL bAllocNow = TRUE);

	void Serialize(CArchive& ar)
	{
		if (ar.IsStoring())
		{
			ar.WriteCount(m_nCount);
			if (m_nCount == 0)
				return;  // nothing more to do

			ASSERT(m_pHashTable != NULL);
			for (UINT nHash = 0; nHash < m_nHashTableSize; nHash++)
			{
				CAssoc* pAssoc;
				for (pAssoc = m_pHashTable[nHash]; pAssoc != NULL;
					pAssoc = pAssoc->pNext)
				{
					KEY* pKey;
					VALUE* pValue;
					/* 
					* in some cases the & operator might be overloaded, and we cannot use it to 
					* obtain the address of a given object.  We then use the following trick to 
					* get the address
				 */
					pKey = reinterpret_cast< KEY* >( &reinterpret_cast< int& >( const_cast< KEY& > ( static_cast< const KEY& >( pAssoc->key ) ) ) );
					pValue = reinterpret_cast< VALUE* >( &reinterpret_cast< int& >( static_cast< VALUE& >( pAssoc->value ) ) );
					SerializeElements<KEY>(ar, pKey, 1);
					SerializeElements<VALUE>(ar, pValue, 1);
				}
			}
		}
		else
		{
			DWORD nNewCount = ar.ReadCount();
			while (nNewCount--)
			{
				KEY newKey[1];
				VALUE newValue[1];
				SerializeElements<KEY>(ar, newKey, 1);
				SerializeElements<VALUE>(ar, newValue, 1);
				SetAt(newKey[0], newValue[0]);
			}
		}
	}

	// Implementation
protected:
	CAssoc** m_pHashTable;
	UINT m_nHashTableSize;
	int m_nCount;
	CAssoc* m_pFreeList;
	struct CPlex* m_pBlocks;
	int m_nBlockSize;

	CAssoc* NewAssoc(ARG_KEY key);
	void FreeAssoc(CAssoc*);
	CAssoc* GetAssocAt(ARG_KEY, UINT&, UINT&) const;

public:
	~CMap();
};

/////////////////////////////////////////////////////////////////////////////
// CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>  functions

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
 int CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::GetCount() const
{ return m_nCount; }

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
 int CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::GetSize() const
{ return m_nCount; }

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
 BOOL CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::IsEmpty() const
{ return m_nCount == 0; }

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
 void CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::SetAt(ARG_KEY key, ARG_VALUE newValue)
{ (*this)[key] = newValue; }

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
 POSITION CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::GetStartPosition() const
{ return (m_nCount == 0) ? NULL : BEFORE_START_POSITION; }

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
const typename CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::CPair* CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::PGetFirstAssoc() const
{ 
	if(m_nCount == 0) return NULL;

	ASSERT(m_pHashTable != NULL);  // never call on empty map

	CAssoc* pAssocRet = (CAssoc*)BEFORE_START_POSITION;

	// find the first association
	for (UINT nBucket = 0; nBucket < m_nHashTableSize; nBucket++)
		if ((pAssocRet = m_pHashTable[nBucket]) != NULL)
			break;
	ASSERT(pAssocRet != NULL);  // must find something

	return pAssocRet;
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
typename CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::CPair* CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::PGetFirstAssoc()
{ 
	if(m_nCount == 0) return NULL;

	ASSERT(m_pHashTable != NULL);  // never call on empty map

	CAssoc* pAssocRet = (CAssoc*)BEFORE_START_POSITION;

	// find the first association
	for (UINT nBucket = 0; nBucket < m_nHashTableSize; nBucket++)
		if ((pAssocRet = m_pHashTable[nBucket]) != NULL)
			break;
	ASSERT(pAssocRet != NULL);  // must find something

	return pAssocRet;
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
 UINT CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::GetHashTableSize() const
{ return m_nHashTableSize; }

/////////////////////////////////////////////////////////////////////////////
// CMap<KEY, ARG_KEY, VALUE, ARG_VALUE> out-of-line functions

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::CMap(int nBlockSize)
{
	ASSERT(nBlockSize > 0);

	m_pHashTable = NULL;
	m_nHashTableSize = 17;  // default size
	m_nCount = 0;
	m_pFreeList = NULL;
	m_pBlocks = NULL;
	m_nBlockSize = nBlockSize;
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
void CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::InitHashTable(
	UINT nHashSize, BOOL bAllocNow)
	//
	// Used to force allocation of a hash table or to override the default
	//   hash table size of (which is fairly small)
{
	ASSERT(m_nCount == 0);
	ASSERT(nHashSize > 0);

	if (m_pHashTable != NULL)
	{
		// free hash table
		delete[] m_pHashTable;
		m_pHashTable = NULL;
	}

	if (bAllocNow)
	{
		m_pHashTable = new CAssoc* [nHashSize];
		memset(m_pHashTable, 0, sizeof(CAssoc*) * nHashSize);
	}
	m_nHashTableSize = nHashSize;
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
void CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::RemoveAll()
{
	if (m_pHashTable != NULL)
	{
		// destroy elements (values and keys)
		for (UINT nHash = 0; nHash < m_nHashTableSize; nHash++)
		{
			CAssoc* pAssoc;
			for (pAssoc = m_pHashTable[nHash]; pAssoc != NULL;
				pAssoc = pAssoc->pNext)
			{
				pAssoc->CAssoc::~CAssoc();
				//DestructElements<VALUE>(&pAssoc->value, 1);
				//DestructElements<KEY>((KEY*)&pAssoc->key, 1);
			}
		}
	}

	// free hash table
	delete[] m_pHashTable;
	m_pHashTable = NULL;

	m_nCount = 0;
	m_pFreeList = NULL;
	m_pBlocks->FreeDataChain();
	m_pBlocks = NULL;
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::~CMap()
{
	RemoveAll();
	ASSERT(m_nCount == 0);
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
typename CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::CAssoc*
CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::NewAssoc(ARG_KEY key)
{
	if (m_pFreeList == NULL)
	{
		// add another block
		CPlex* newBlock = CPlex::Create(m_pBlocks, m_nBlockSize, sizeof(CAssoc));
		// chain them into free list
		CAssoc* pAssoc = (CAssoc*) newBlock->data();
		// free in reverse order to make it easier to debug
		pAssoc += m_nBlockSize - 1;
		for (int i = m_nBlockSize-1; i >= 0; i--, pAssoc--)
		{
			pAssoc->pNext = m_pFreeList;
			m_pFreeList = pAssoc;
		}
	}

	CAssoc* pAssoc = m_pFreeList;

	// zero the memory
	CAssoc* pTemp = pAssoc->pNext;
	memset( pAssoc, 0, sizeof(CAssoc) );
	pAssoc->pNext = pTemp;

	m_pFreeList = m_pFreeList->pNext;
	m_nCount++;
	ASSERT(m_nCount > 0);  // make sure we don't overflow
#pragma push_macro("new")
#undef new
	::new(pAssoc) CAssoc(key);
#pragma pop_macro("new")
	//	ConstructElements<KEY>(&pAssoc->key, 1);
	//	ConstructElements<VALUE>(&pAssoc->value, 1);   // special construct values
	return pAssoc;
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
void CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::FreeAssoc(CAssoc* pAssoc)
{
	pAssoc->CAssoc::~CAssoc();
	//	DestructElements<VALUE>(&pAssoc->value, 1);
	//	DestructElements<KEY>(&pAssoc->key, 1);
	pAssoc->pNext = m_pFreeList;
	m_pFreeList = pAssoc;
	m_nCount--;
	ASSERT(m_nCount >= 0);  // make sure we don't underflow

	// if no more elements, cleanup completely
	if (m_nCount == 0)
		RemoveAll();
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
typename CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::CAssoc*
CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::GetAssocAt(ARG_KEY key, UINT& nHashBucket, UINT& nHashValue) const
// find association (or return NULL)
{
	nHashValue = HashKey<ARG_KEY>(key);
	nHashBucket = nHashValue % m_nHashTableSize;

	if (m_pHashTable == NULL)
		return NULL;

	// see if it exists
	CAssoc* pAssoc;
	for (pAssoc = m_pHashTable[nHashBucket]; pAssoc != NULL; pAssoc = pAssoc->pNext)
	{
		if (pAssoc->nHashValue == nHashValue && CompareElements(&pAssoc->key, &key))
			return pAssoc;
	}
	return NULL;
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
BOOL CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::Lookup(ARG_KEY key, VALUE& rValue) const
{
	UINT nHashBucket, nHashValue;
	CAssoc* pAssoc = GetAssocAt(key, nHashBucket, nHashValue);
	if (pAssoc == NULL)
		return FALSE;  // not in map

	rValue = pAssoc->value;
	return TRUE;
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
const typename CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::CPair* CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::PLookup(ARG_KEY key) const
{
	UINT nHashBucket, nHashValue;
	CAssoc* pAssoc = GetAssocAt(key, nHashBucket, nHashValue);
	return pAssoc;
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
typename CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::CPair* CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::PLookup(ARG_KEY key)
{
	UINT nHashBucket, nHashValue;
	CAssoc* pAssoc = GetAssocAt(key, nHashBucket, nHashValue);
	return pAssoc;
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
VALUE& CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::operator[](ARG_KEY key)
{
	UINT nHashBucket, nHashValue;
	CAssoc* pAssoc;
	if ((pAssoc = GetAssocAt(key, nHashBucket, nHashValue)) == NULL)
	{
		if (m_pHashTable == NULL)
			InitHashTable(m_nHashTableSize);

		// it doesn't exist, add a new Association
		pAssoc = NewAssoc(key);
		pAssoc->nHashValue = nHashValue;
		//'pAssoc->value' is a constructed object, nothing more

		// put into hash table
		pAssoc->pNext = m_pHashTable[nHashBucket];
		m_pHashTable[nHashBucket] = pAssoc;
	}
	return pAssoc->value;  // return new reference
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
BOOL CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::RemoveKey(ARG_KEY key)
// remove key - return TRUE if removed
{
	if (m_pHashTable == NULL)
		return FALSE;  // nothing in the table

	UINT nHashValue;
	CAssoc** ppAssocPrev;
	nHashValue = HashKey<ARG_KEY>(key);
	ppAssocPrev = &m_pHashTable[nHashValue%m_nHashTableSize];

	CAssoc* pAssoc;
	for (pAssoc = *ppAssocPrev; pAssoc != NULL; pAssoc = pAssoc->pNext)
	{
		if ((pAssoc->nHashValue == nHashValue) && CompareElements(&pAssoc->key, &key))
		{
			// remove it
			*ppAssocPrev = pAssoc->pNext;  // remove from list
			FreeAssoc(pAssoc);
			return TRUE;
		}
		ppAssocPrev = &pAssoc->pNext;
	}
	return FALSE;  // not found
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
void CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::GetNextAssoc(POSITION& rNextPosition,
														KEY& rKey, VALUE& rValue) const
{
	CAssoc* pAssocRet = (CAssoc*)rNextPosition;

	if (pAssocRet == (CAssoc*) BEFORE_START_POSITION)
	{
		// find the first association
		for (UINT nBucket = 0; nBucket < m_nHashTableSize; nBucket++)
		{
			if ((pAssocRet = m_pHashTable[nBucket]) != NULL)
			{
				break;
			}
		}
	}

	// find next association
	CAssoc* pAssocNext;
	if ((pAssocNext = pAssocRet->pNext) == NULL)
	{
		// go to next bucket
		for (UINT nBucket = (pAssocRet->nHashValue % m_nHashTableSize) + 1;
			nBucket < m_nHashTableSize; nBucket++)
			if ((pAssocNext = m_pHashTable[nBucket]) != NULL)
				break;
	}

	rNextPosition = (POSITION) pAssocNext;

	// fill in return data
	rKey = pAssocRet->key;
	rValue = pAssocRet->value;
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
const typename CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::CPair*
CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::PGetNextAssoc(const typename CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::CPair* pPairRet) const
{
	CAssoc* pAssocRet = (CAssoc*)pPairRet;

	ASSERT(m_pHashTable != NULL);  // never call on empty map
	ASSERT(pAssocRet != NULL);

	if(m_pHashTable == NULL || pAssocRet == NULL)
		return NULL;

	ASSERT(pAssocRet != (CAssoc*)BEFORE_START_POSITION);

	// find next association
	CAssoc* pAssocNext;
	if ((pAssocNext = pAssocRet->pNext) == NULL)
	{
		// go to next bucket
		for (UINT nBucket = (pAssocRet->nHashValue % m_nHashTableSize) + 1;
			nBucket < m_nHashTableSize; nBucket++)
			if ((pAssocNext = m_pHashTable[nBucket]) != NULL)
				break;
	}

	return pAssocNext;
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
typename CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::CPair*
CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::PGetNextAssoc(const typename CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::CPair* pPairRet)
{

	CAssoc* pAssocRet = (CAssoc*)pPairRet;

	ASSERT(m_pHashTable != NULL);  // never call on empty map
	ASSERT(pAssocRet != NULL);

	if(m_pHashTable == NULL || pAssocRet == NULL)
		return NULL;

	ASSERT(pAssocRet != (CAssoc*)BEFORE_START_POSITION);

	// find next association
	CAssoc* pAssocNext;
	if ((pAssocNext = pAssocRet->pNext) == NULL)
	{
		// go to next bucket
		for (UINT nBucket = (pAssocRet->nHashValue % m_nHashTableSize) + 1;
			nBucket < m_nHashTableSize; nBucket++)
			if ((pAssocNext = m_pHashTable[nBucket]) != NULL)
				break;
	}

	return pAssocNext;
}



//////////////////////////////////////////////////////////////////////////
//
class CMapPtrToPtr : public CObject
{
	DECLARE_DYNAMIC(CMapPtrToPtr)
protected:
	// Association
	struct CAssoc
	{
		CAssoc* pNext;

		void* key;
		void* value;
	};

public:

	// Construction
	/* explicit */ CMapPtrToPtr(int nBlockSize = 10);

	// Attributes
	// number of elements
	int GetCount() const;
	int GetSize() const;
	BOOL IsEmpty() const;

	// Lookup
	BOOL Lookup(void* key, void*& rValue) const;

	// Operations
	// Lookup and add if not there
	void*& operator[](void* key);

	// add a new (key, value) pair
	void SetAt(void* key, void* newValue);

	// removing existing (key, ?) pair
	BOOL RemoveKey(void* key);
	void RemoveAll();

	// iterating all (key, value) pairs
	POSITION GetStartPosition() const;
	void GetNextAssoc(POSITION& rNextPosition, void*& rKey, void*& rValue) const;

	// advanced features for derived classes
	UINT GetHashTableSize() const;
	void InitHashTable(UINT hashSize, BOOL bAllocNow = TRUE);

	// Overridables: special non-virtual (see map implementation for details)
	// Routine used to user-provided hash keys
	UINT HashKey(void* key) const;

	// Implementation
protected:
	CAssoc** m_pHashTable;
	UINT m_nHashTableSize;
	int m_nCount;
	CAssoc* m_pFreeList;
	struct CPlex* m_pBlocks;
	int m_nBlockSize;

	CAssoc* NewAssoc();
	void FreeAssoc(CAssoc*);
	CAssoc* GetAssocAt(void*, UINT&, UINT&) const;

public:
	~CMapPtrToPtr();

	void* GetValueAt(void* key) const;


protected:
	// local typedefs for CTypedPtrMap class template
	typedef void* BASE_KEY;
	typedef void* BASE_ARG_KEY;
	typedef void* BASE_VALUE;
	typedef void* BASE_ARG_VALUE;
};


inline int CMapPtrToPtr::GetCount() const
{ return m_nCount; }
inline int CMapPtrToPtr::GetSize() const
{ return m_nCount; }
inline BOOL CMapPtrToPtr::IsEmpty() const
{ return m_nCount == 0; }
inline void CMapPtrToPtr::SetAt(void* key, void* newValue)
{ (*this)[key] = newValue; }
inline POSITION CMapPtrToPtr::GetStartPosition() const
{ return (m_nCount == 0) ? NULL : BEFORE_START_POSITION; }
inline UINT CMapPtrToPtr::GetHashTableSize() const
{ return m_nHashTableSize; }


//////////////////////////////////////////////////////////////////////////
//

class CPtrArray : public CObject
{

	DECLARE_DYNAMIC(CPtrArray)
public:

	// Construction
	CPtrArray();

	// Attributes
	int  GetSize() const;
	int  GetCount() const;
	BOOL IsEmpty() const;
	int  GetUpperBound() const;
	void SetSize(int  nNewSize, int  nGrowBy = -1);

	// Operations
	// Clean up
	void FreeExtra();
	void RemoveAll();

	// Accessing elements
	void* GetAt(int  nIndex) const;
	void SetAt(int  nIndex, void* newElement);

	void*& ElementAt(int  nIndex);

	// Direct Access to the element data (may return NULL)
	const void** GetData() const;
	void** GetData();

	// Potentially growing the array
	void SetAtGrow(int  nIndex, void* newElement);

	int  Add(void* newElement);

	int  Append(const CPtrArray& src);
	void Copy(const CPtrArray& src);

	// overloaded operator helpers
	void* operator[](int  nIndex) const;
	void*& operator[](int  nIndex);

	// Operations that move elements around
	void InsertAt(int  nIndex, void* newElement, int  nCount = 1);

	void RemoveAt(int  nIndex, int  nCount = 1);
	void InsertAt(int  nStartIndex, CPtrArray* pNewArray);

	// Implementation
protected:
	void** m_pData;   // the actual array of data
	int  m_nSize;     // # of elements (upperBound - 1)
	int  m_nMaxSize;  // max allocated
	int  m_nGrowBy;   // grow amount


public:
	~CPtrArray();

protected:
	// local typedefs for class templates
	typedef void* BASE_TYPE;
	typedef void* BASE_ARG_TYPE;
};


inline int CPtrArray::GetSize() const
{ return m_nSize; }
inline int CPtrArray::GetCount() const
{ return m_nSize; }
inline BOOL CPtrArray::IsEmpty() const
{ return m_nSize == 0; }
inline int CPtrArray::GetUpperBound() const
{ return m_nSize-1; }
inline void CPtrArray::RemoveAll()
{ SetSize(0); }
inline void* CPtrArray::GetAt(int nIndex) const
{
	ASSERT(nIndex >= 0 && nIndex < m_nSize);
	return m_pData[nIndex]; 
}

inline void CPtrArray::SetAt(int nIndex, void* newElement)
{
	ASSERT(nIndex >= 0 && nIndex < m_nSize);
	m_pData[nIndex] = newElement; 
}

inline void*& CPtrArray::ElementAt(int nIndex)
{
	ASSERT(nIndex >= 0 && nIndex < m_nSize);
	return m_pData[nIndex]; 
}
inline const void** CPtrArray::GetData() const
{ return (const void**)m_pData; }
inline void** CPtrArray::GetData()
{ return (void**)m_pData; }
inline int CPtrArray::Add(void* newElement)
{ int nIndex = m_nSize;
SetAtGrow(nIndex, newElement);
return nIndex; }

inline void* CPtrArray::operator[](int nIndex) const
{ return GetAt(nIndex); }
inline void*& CPtrArray::operator[](int nIndex)
{ return ElementAt(nIndex); }


//////////////////////////////////////////////////////////////////////////
//

class CMapStringToPtr : public CObject
{
	DECLARE_DYNAMIC(CMapStringToPtr)
protected:
	// Association
	struct CAssoc
	{
		CAssoc* pNext;
		UINT nHashValue;  // needed for efficient iteration
		CString key;
		void* value;
	};

public:

	// Construction
	/* explicit */ CMapStringToPtr(int nBlockSize = 10);

	// Attributes
	// number of elements
	int GetCount() const;
	int GetSize() const;
	BOOL IsEmpty() const;

	// Lookup
	BOOL Lookup(LPCTSTR key, void*& rValue) const;
	BOOL LookupKey(LPCTSTR key, LPCTSTR& rKey) const;

	// Operations
	// Lookup and add if not there
	void*& operator[](LPCTSTR key);

	// add a new (key, value) pair
	void SetAt(LPCTSTR key, void* newValue);

	// removing existing (key, ?) pair
	BOOL RemoveKey(LPCTSTR key);
	void RemoveAll();

	// iterating all (key, value) pairs
	POSITION GetStartPosition() const;
	void GetNextAssoc(POSITION& rNextPosition, CString& rKey, void*& rValue) const;

	// advanced features for derived classes
	UINT GetHashTableSize() const;
	void InitHashTable(UINT hashSize, BOOL bAllocNow = TRUE);

	// Overridables: special non-virtual (see map implementation for details)
	// Routine used to user-provided hash keys
	UINT HashKey(LPCTSTR key) const;

	// Implementation
protected:
	CAssoc** m_pHashTable;
	UINT m_nHashTableSize;
	int m_nCount;
	CAssoc* m_pFreeList;
	struct CPlex* m_pBlocks;
	int m_nBlockSize;

	CAssoc* NewAssoc();
	void FreeAssoc(CAssoc*);
	CAssoc* GetAssocAt(LPCTSTR, UINT&, UINT&) const;

public:
	~CMapStringToPtr();

protected:
	// local typedefs for CTypedPtrMap class template
	typedef CString BASE_KEY;
	typedef LPCTSTR BASE_ARG_KEY;
	typedef void* BASE_VALUE;
	typedef void* BASE_ARG_VALUE;
};
inline int CMapStringToPtr::GetCount() const
{ return m_nCount; }
inline int CMapStringToPtr::GetSize() const
{ return m_nCount; }
inline BOOL CMapStringToPtr::IsEmpty() const
{ return m_nCount == 0; }
inline void CMapStringToPtr::SetAt(LPCTSTR key, void* newValue)
{ (*this)[key] = newValue; }
inline POSITION CMapStringToPtr::GetStartPosition() const
{ return (m_nCount == 0) ? NULL : BEFORE_START_POSITION; }
inline UINT CMapStringToPtr::GetHashTableSize() const
{ return m_nHashTableSize; }


//////////////////////////////////////////////////////////////////////////
//
class CMapStringToString : public CObject
{
	DECLARE_SERIAL(CMapStringToString)
public:
	// Association
	struct CPair
	{
		const CString key;
		CString value;
	protected:
		CPair(LPCTSTR keyval) : key(keyval) {}
	};
protected:
	class CAssoc : public CPair
	{
		friend class CMapStringToString;
		CAssoc* pNext;
		UINT nHashValue;  // needed for efficient iteration
	public:
		CAssoc( LPCTSTR keyval ) : CPair( keyval ) {}
	};

public:

	// Construction
	/* explicit */ CMapStringToString(int nBlockSize = 10);

	// Attributes
	// number of elements
	int GetCount() const;
	int GetSize() const;
	BOOL IsEmpty() const;

	// Lookup
	BOOL Lookup(LPCTSTR key, CString& rValue) const;
	const CPair *PLookup(LPCTSTR key) const;
	CPair *PLookup(LPCTSTR key);
	BOOL LookupKey(LPCTSTR key, LPCTSTR& rKey) const;

	// Operations
	// Lookup and add if not there
	CString& operator[](LPCTSTR key);

	// add a new (key, value) pair
	void SetAt(LPCTSTR key, LPCTSTR newValue);

	// removing existing (key, ?) pair
	BOOL RemoveKey(LPCTSTR key);
	void RemoveAll();

	// iterating all (key, value) pairs
	POSITION GetStartPosition() const;

	const CPair *PGetFirstAssoc() const;
	CPair *PGetFirstAssoc();

	void GetNextAssoc(POSITION& rNextPosition, CString& rKey, CString& rValue) const;

	const CPair *PGetNextAssoc(const CPair* pAssoc) const;
	CPair *PGetNextAssoc(const CPair* pAssoc);

	// advanced features for derived classes
	UINT GetHashTableSize() const;
	void InitHashTable(UINT hashSize, BOOL bAllocNow = TRUE);

	// Overridables: special non-virtual (see map implementation for details)
	// Routine used to user-provided hash keys
	UINT HashKey(LPCTSTR key) const;

	// Implementation
protected:
	CAssoc** m_pHashTable;
	UINT m_nHashTableSize;
	int m_nCount;
	CAssoc* m_pFreeList;
	struct CPlex* m_pBlocks;
	int m_nBlockSize;

	CAssoc* NewAssoc(LPCTSTR key);
	void FreeAssoc(CAssoc*);
	CAssoc* GetAssocAt(LPCTSTR, UINT&, UINT&) const;

public:
	~CMapStringToString();

	void Serialize(CArchive&);

protected:
	// local typedefs for CTypedPtrMap class template
	typedef CString BASE_KEY;
	typedef LPCTSTR BASE_ARG_KEY;
	typedef CString BASE_VALUE;
	typedef LPCTSTR BASE_ARG_VALUE;
};
 inline int CMapStringToString::GetCount() const
{ return m_nCount; }
inline  int CMapStringToString::GetSize() const
{ return m_nCount; }
 inline BOOL CMapStringToString::IsEmpty() const
{ return m_nCount == 0; }
 inline void CMapStringToString::SetAt(LPCTSTR key, LPCTSTR newValue)
{ (*this)[key] = newValue; }
 inline POSITION CMapStringToString::GetStartPosition() const
{ return (m_nCount == 0) ? NULL : BEFORE_START_POSITION; }
 inline UINT CMapStringToString::GetHashTableSize() const
{ return m_nHashTableSize; }


 //////////////////////////////////////////////////////////////////////////
 //

 class CMapWordToPtr : public CObject
 {

	 DECLARE_DYNAMIC(CMapWordToPtr)
 protected:
	 // Association
	 struct CAssoc
	 {
		 CAssoc* pNext;
		 void* value;
		 WORD key;
	 };

 public:

	 // Construction
	 /* explicit */ CMapWordToPtr(int nBlockSize = 10);

	 // Attributes
	 // number of elements
	 int GetCount() const;
	 int GetSize() const;
	 BOOL IsEmpty() const;

	 // Lookup
	 BOOL Lookup(WORD key, void*& rValue) const;

	 // Operations
	 // Lookup and add if not there
	 void*& operator[](WORD key);

	 // add a new (key, value) pair
	 void SetAt(WORD key, void* newValue);

	 // removing existing (key, ?) pair
	 BOOL RemoveKey(WORD key);
	 void RemoveAll();

	 // iterating all (key, value) pairs
	 POSITION GetStartPosition() const;
	 void GetNextAssoc(POSITION& rNextPosition, WORD& rKey, void*& rValue) const;

	 // advanced features for derived classes
	 UINT GetHashTableSize() const;
	 void InitHashTable(UINT hashSize, BOOL bAllocNow = TRUE);

	 // Overridables: special non-virtual (see map implementation for details)
	 // Routine used to user-provided hash keys
	 UINT HashKey(WORD key) const;

	 // Implementation
 protected:
	 CAssoc** m_pHashTable;
	 UINT m_nHashTableSize;
	 int m_nCount;
	 CAssoc* m_pFreeList;
	 struct CPlex* m_pBlocks;
	 int m_nBlockSize;

	 CAssoc* NewAssoc();
	 void FreeAssoc(CAssoc*);
	 CAssoc* GetAssocAt(WORD, UINT&, UINT&) const;

 public:
	 ~CMapWordToPtr();

 protected:
	 // local typedefs for CTypedPtrMap class template
	 typedef WORD BASE_KEY;
	 typedef WORD BASE_ARG_KEY;
	 typedef void* BASE_VALUE;
	 typedef void* BASE_ARG_VALUE;
 };


  inline int CMapWordToPtr::GetCount() const
 { return m_nCount; }
  inline int CMapWordToPtr::GetSize() const
 { return m_nCount; }
  inline BOOL CMapWordToPtr::IsEmpty() const
 { return m_nCount == 0; }
  inline void CMapWordToPtr::SetAt(WORD key, void* newValue)
 { (*this)[key] = newValue; }
  inline POSITION CMapWordToPtr::GetStartPosition() const
 { return (m_nCount == 0) ? NULL : BEFORE_START_POSITION; }
  inline UINT CMapWordToPtr::GetHashTableSize() const
 { return m_nHashTableSize; }

  //////////////////////////////////////////////////////////////////////////


  class CByteArray 
  {
  public:
	  // Construction
	  CByteArray();

	  // Attributes
	  int GetSize() const;
	  int GetCount() const;
	  BOOL IsEmpty() const;
	  int GetUpperBound() const;
	  void SetSize(int nNewSize, int nGrowBy = -1);

	  // Operations
	  // Clean up
	  void FreeExtra();
	  void RemoveAll();

	  // Accessing elements
	  BYTE GetAt(int nIndex) const;
	  void SetAt(int nIndex, BYTE newElement);

	  BYTE& ElementAt(int nIndex);

	  // Direct Access to the element data (may return NULL)
	  const BYTE* GetData() const;
	  BYTE* GetData();
	  void SetData(const BYTE* data,int nSize);

	  // Potentially growing the array
	  void SetAtGrow(int nIndex, BYTE newElement);

	  int Add(BYTE newElement);

	  int Append(const CByteArray& src);
	  void Copy(const CByteArray& src);

	  // overloaded operator helpers
	  BYTE operator[](int nIndex) const;
	  BYTE& operator[](int nIndex);

	  // Operations that move elements around
	  void InsertAt(int nIndex, BYTE newElement, int nCount = 1);

	  void RemoveAt(int nIndex, int nCount = 1);
	  void InsertAt(int nStartIndex, CByteArray* pNewArray);

	  // Implementation
  protected:
	  BYTE* m_pData;   // the actual array of data
	  int m_nSize;     // # of elements (upperBound - 1)
	  int m_nMaxSize;  // max allocated
	  int m_nGrowBy;   // grow amount


  public:
	  ~CByteArray();

  protected:
	  // local typedefs for class templates
	  typedef BYTE BASE_TYPE;
	  typedef BYTE BASE_ARG_TYPE;
  };


  inline int CByteArray::GetSize() const
  { return m_nSize; }
  inline int CByteArray::GetCount() const
  { return m_nSize; }
  inline BOOL CByteArray::IsEmpty() const
  { return m_nSize == 0; }
  inline int CByteArray::GetUpperBound() const
  { return m_nSize-1; }
  inline void CByteArray::RemoveAll()
  { SetSize(0); }
  inline BYTE CByteArray::GetAt(int nIndex) const
  { ASSERT(nIndex >= 0 && nIndex < m_nSize);
  return m_pData[nIndex]; }
  inline void CByteArray::SetAt(int nIndex, BYTE newElement)
  { 
	  ASSERT(nIndex >= 0 && nIndex < m_nSize);
	  m_pData[nIndex] = newElement;
  }

  inline BYTE& CByteArray::ElementAt(int nIndex)
  { 
	  ASSERT(nIndex >= 0 && nIndex < m_nSize);
	  return m_pData[nIndex]; 
  }
  inline const BYTE* CByteArray::GetData() const
  { return (const BYTE*)m_pData; }
  inline BYTE* CByteArray::GetData()
  { return (BYTE*)m_pData; }
  inline int CByteArray::Add(BYTE newElement)
  { int nIndex = m_nSize;
  SetAtGrow(nIndex, newElement);
  return nIndex; }

  inline BYTE CByteArray::operator[](int nIndex) const
  { return GetAt(nIndex); }
  inline BYTE& CByteArray::operator[](int nIndex)
  { return ElementAt(nIndex); }


void SplitStr2Ary(LPCTSTR pszTxt,CStringArray& aryOut,LPCTSTR pszSep,BOOL bRemoveEmptyItems = TRUE);
