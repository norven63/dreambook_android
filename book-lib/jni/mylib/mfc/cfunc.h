#pragma once

#include "Platform.h"

#define wcslen tcslen
#define wcsncpy tcsncpy
#undef iswdigit
#define iswdigit isdigit
#undef lstrlen
#define lstrlen tcslen

inline void *memdup(const void *data, size_t len)
{
	void *dup = malloc(len);
	if (dup)
		memcpy(dup, data, len);
	return dup;
}

template<class T>
T* tmemset(T *dest,WORD c,size_t count)
{
	if( dest==NULL )
	{
		ASSERT(FALSE);
		return dest;
	}

	for (size_t i=0;i<count;i++)
	{
		dest[i] = c ;
	}
	return dest ;
}

template<class T>
int tmemcmp(const T * buf1,
			const T * buf2,
			size_t count
			)
{
	for (size_t i=0;i<count;i++)
	{
		if ( buf1[i]!=buf2[i] )
		{
			return buf1[i]>buf2[i]?1:-1;
		}
	}
	return 0;
}

template<class T>
T* tcsncpy(T *strDest,const T* strSource,size_t count)
{
	bool bEnd = false;
	for(size_t i=0;i<count;i++)
	{
		if( bEnd )
		{
			strDest[i]=0;
		}else
		{
			if( strSource[i]==0 )
				bEnd=true;
			strDest[i]=strSource[i];
		}

	}
	return strDest;
}


template<class T>
int tcslen(const T* sz)
{
	if( sz==NULL )
	{
		return 0;
	}

	int nLen = 0 ;
	while( sz[nLen]!=0 )
		nLen++ ;
	return nLen ;
}
template<class T>
void tcstolower(T *s)
{
	if (!s) return;
	for (; *s; s++)
		*s = (T) towlower(*s);
}
template<class T>
void tcstoupper(T *s)
{
	if (!s) return;
	for (; *s; s++)
		*s = toupper(*s);
}
template<class T>
T* tcsdup(const T* sz)
{
	if( sz==NULL )
	{
		return NULL;
	}

	int nSize=(tcslen(sz)+1)*sizeof(T);

	return (T*)memdup(sz,nSize);
}
template<class T>
T* tcsdupn(const T* sz,int n)
{
	if( sz==NULL )
	{
		return NULL;
	}

	T* p=(T*)memdup(sz,(n+1)*sizeof(T));
	if(p)
		p[n]=0;

	return p;
}
template<class T>
T* tcscat(T* strDestination,const T* strSource )
{
	if ( strDestination==NULL || strSource==NULL )
	{
		ASSERT(FALSE);
		return strDestination;
	}
	T* p = strDestination ;
	while(*p!=0)
		p++ ;

	while(*strSource!=0)
	{
		*p++ = *strSource++ ;
	}
	*p = 0 ;
	return strDestination ;
}
template<class T>
T* tcscpy(T* strDestination,const T* strSource )
{
	if ( strDestination==NULL || strSource==NULL )
	{
		ASSERT(FALSE);
		return strDestination;
	}
	T* p = strDestination ;

	while(*strSource!=0)
	{
		*p++ = *strSource++ ;
	}
	*p = 0 ;
	return strDestination ;
}
// nDestLen:目标串中能容纳的字符串长度(不包括结尾的0)
template<class T>
T* tcscpy_n(T* strDestination,const T* strSource,int nDestLen )
{
	if ( strDestination==NULL || strSource==NULL )
	{
		ASSERT(FALSE);
		return strDestination;
	}
	T* p = strDestination ;

	while(nDestLen>0 && *strSource!=0)
	{
		*p++ = *strSource++ ;
		nDestLen--;
	}
	*p = 0 ;
	return strDestination ;
}
template<class T>
const T *tcschr(const T* str,T c)
{
	if( str==NULL )
	{
		ASSERT(FALSE);
		return str;
	}
	for (;*str!=0;str++)
	{
		if( *str==c )
			return str ;
	}
	return NULL ;
}
template<class T>
const T *tcsrchr(const T *str,T c)
{
	if ( str==NULL )
	{
		ASSERT(FALSE);
		return str;
	}
	int nLen = tcslen(str) ;
	for (int i=nLen-1;i>=0;i--)
	{
		if( str[i]==c )
			return str+i ;
	}
	return NULL ;
}

template<class T>
T* tcsrev(T* pStr)
{
	int nLen = tcslen(pStr) ;
	int nMaxIndex = nLen-1 ;
	int nHalfLen = nLen>>2 ;
	for (int i=0;i<nHalfLen;i++)
	{
		pStr[i] = pStr[nMaxIndex-i] ;
	}
}

template<class T>
T* tcsstr(const T *str1,const T* str2)
{
	T *cp = (T*) str1;
	T *s1, *s2;

	if ( !*str2 )
		return((T*)str1);

	while (*cp)
	{
		s1 = cp;
		s2 = (T*) str2;

		while ( *s1 && *s2 && !(*s1-*s2) )
			s1++, s2++;

		if (!*s2)
			return(cp);

		cp++;
	}

	return(NULL);  
}

template<class T>
int tcsspn(const T* str,const T* strCharSet)
{
	if ( str==NULL || strCharSet==NULL )
	{
		ASSERT(FALSE);
		return -1;
	}
	int i=0;
	for (;str[i]!=0;i++)
	{
		T c=str[i] ;
		bool bFound = false;
		for (int j=0;strCharSet[j]!=0;j++)
		{
			if( c==strCharSet[j] )
			{
				bFound=true;
				break ;
			}
		}
		if( !bFound )
			return i;
	}
	return i ;
}
template<class T>
int tcscspn(const T* str,const T* strCharSet)
{
	if ( str==NULL || strCharSet==NULL )
	{
		ASSERT(FALSE);
		return -1;
	}
	int i = 0 ;
	for (;str[i]!=0;i++)
	{
		T c=str[i] ;
		for (int j=0;strCharSet[j]!=0;j++)
		{
			if( c==strCharSet[j] )
			{
				return i ;
			}
		}
	}
	return i ;
}
template<class T>
T* tcspbrk(const T* str,const T* strCharSet)
{	
	if ( str==NULL || strCharSet==NULL )
	{
		ASSERT(FALSE);
		return NULL;
	}
	for (int i = 0;str[i]!=0;i++)
	{
		T c=str[i] ;
		for (int j=0;strCharSet[j]!=0;j++)
		{
			if( c==strCharSet[j] )
			{
				return (T*)(str+i) ;
			}
		}
	}
	return NULL ;
}
template<class T>
T* tcsReverseFindOneOf(const T* str,const T* strCharSet)
{	
	if ( str==NULL || strCharSet==NULL )
	{
		ASSERT(FALSE);
		return NULL;
	}
	int len = tcslen(str);
	for (int i = len-1;i>=0;i--)
	{
		T c=str[i] ;
		for (int j=0;strCharSet[j]!=0;j++)
		{
			if( c==strCharSet[j] )
			{
				return (T*)(str+i) ;
			}
		}
	}
	return NULL ;
}
template<class T>
int xisdigit(T c)
{
	return c>=_T('0') && c<=_T('9') ;
}

template<class T>
int xisalpha(T c)
{
	return (c>=_T('a') && c<=_T('z'))||(c>=_T('A') && c<=_T('Z')) ;
}

template<class T>
int xisalnum(T c)
{
	return xisalpha(c) || (c>=_T('0') && c<=_T('9')) ;
}

int xisspace(int c);


template<class T1,class T2>
int tcscmp(const T1* string1,const T2* string2 )
{
	if ( string1==NULL || string2==NULL )
	{
		ASSERT(FALSE);
		return string1==(T1*)string2?0:(string1!=NULL?1:-1);
	}
	int  i ;
	for (i=0;string1[i]!=0 && string2[i]!=0; i++)
	{
		T1 c1 = string1[i] ;
		T2 c2 = string2[i] ;
		if( c1!=c2 )
		{
			if( c1<c2 )
				return -1 ;
			if( c1>c2 )
				return 1 ;
		}
	}
	if( string1[i]==0 )
	{
		if( string2[i]==0 )
			return 0 ;
		return -1 ;
	}
	return 1 ;
}
template<class T>
int tcsncmp(const T* string1,const T* string2, int nCount )
{
	if ( string1==NULL || string2==NULL )
	{
		ASSERT(FALSE);
		return string1==string2?0:(string1!=NULL?1:-1);
	}
	int  i ;
	for (i=0;string1[i]!=0 && string2[i]!=0 && i<nCount; i++)
	{
		T c1 = string1[i] ;
		T c2 = string2[i] ;
		if( c1!=c2 )
		{
			if( c1<c2 )
				return -1 ;
			if( c1>c2 )
				return 1 ;
		}
	}
	if( i>=nCount )
		return 0;

	if( string1[i]==0 )
	{
		if( string2[i]==0 )
			return 0 ;
		return -1 ;
	}
	return 1 ;
}

template<class T>
int tcsicmp(const T* string1,const T* string2 )
{
	if ( string1==NULL || string2==NULL )
	{
		ASSERT(FALSE);
		return string1==string2?0:(string1!=NULL?1:-1);
	}
	int ch1, ch2;
	do
	{
		if ( ((ch1 = (*(string1++))) >= _T('A')) &&(ch1 <= _T('Z')) )
			ch1 += 0x20;
		if ( ((ch2 = (*(string2++))) >= _T('A')) &&(ch2 <= _T('Z')) )
			ch2 += 0x20;

	} while ( ch1 && (ch1 == ch2) );
	return(ch1 - ch2);
}
template<class T>
int tcsincmp(const T* string1,const T* string2, int nCount )
{
	if ( string1==NULL || string2==NULL )
	{
		ASSERT(FALSE);
		return string1==string2?0:(string1!=NULL?1:-1);
	}
	int  i ;
	for (i=0;string1[i]!=0 && string2[i]!=0 && i<nCount; i++)
	{
		T c1 = tolower(string1[i]) ;
		T c2 = tolower(string2[i]) ;
		if( c1!=c2 )
		{
			if( c1<c2 )
				return -1 ;
			if( c1>c2 )
				return 1 ;
		}
	}
	if( i>=nCount )
		return 0;

	if( string1[i]==0 )
	{
		if( string2[i]==0 )
			return 0 ;
		return -1 ;
	}
	return 1 ;
}
template<class T>
BOOL tcsstartwith(const T* string1,const T* string2 )
{
	return tcsncmp(string1,string2,tcslen(string2))==0;
}
template<class T>
BOOL tcsstartwithi(const T* string1,const T* string2 )
{
	return tcsincmp(string1,string2,tcslen(string2))==0;
}
inline LONG wtoi( const WCHAR* str )
{
	if ( str==NULL )
	{
		ASSERT(FALSE);
		return 0;
	}
	LONG RunningTotal = 0;
	char bMinus = 0;

	while (xisspace(*str)) {
		str++;
	} /* while */

	if (*str == '+') {
		str++;
	} else if (*str == '-') {
		bMinus = 1;
		str++;
	} /* if */

	while (*str >= '0' && *str <= '9') {
		RunningTotal = RunningTotal * 10 + *str - '0';
		str++;
	} /* while */

	return bMinus ? -RunningTotal : RunningTotal;
}
