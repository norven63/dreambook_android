#ifndef SOCKETIOSTREAM
#define SOCKETIOSTREAM
#include "endianDef.h"
class ReaderFromBuffer
{
public: 
	ReaderFromBuffer(void* pBuffer, int nBufLen)
	:m_pBuffer(pBuffer),m_nLen(nBufLen),m_nOffset(0)
	{
	}
	~ReaderFromBuffer()
	{
	}
	void Skip(int nLen)
	{
		m_nOffset += nLen;
	}
	ReaderFromBuffer& operator>>(short& value)
	{
		ReadData((unsigned char*)&value, sizeof(value));
		return *this;
	}

	ReaderFromBuffer& operator>>(int& value)
	{
		ReadData((unsigned char*)&value, sizeof(value));
		return *this;
	}
private:
	void ReadData(unsigned char* pData, int nLen)
	{
		emEndian endian = cEndianDef::Instance()->GetEndian();
		if (endian == emBigEndian)
			Read(pData, nLen);
		else// if(endian == emLittelEndian)
			for (int i = 0; i < (int) nLen; i++)
				Read((unsigned char*)pData + nLen - 1 - i, 1);		
	}
	int Read(unsigned char* pBuffer, int nLen)
	{
		int nReadLen = nLen;
		if (nReadLen > (m_nLen - m_nOffset))
		{
			nReadLen = m_nLen - m_nOffset;
		}
		for (int i = 0; i < nReadLen; i++)
		{
			pBuffer[i] = *((unsigned char*)m_pBuffer + m_nOffset + i);
		}
		m_nOffset += nReadLen;
		return nReadLen;
	}
private:
	void* m_pBuffer;
	int m_nLen;
	int m_nOffset;

};
class WriterToBuffer
{
public: 
	WriterToBuffer()
	{
		m_pBuffer = NULL;
		m_nOffset = 0;
	}
	~WriterToBuffer()
	{
		if (m_pBuffer != NULL)
		{
			free(m_pBuffer);
			m_pBuffer = NULL;
		}
	}
	unsigned char* GetIntBuffer(int& value)
	{
		m_pBuffer = (unsigned char*)malloc(sizeof(int));
		WriteData((unsigned char*)&value, sizeof(value));
		return (unsigned char*)m_pBuffer;
	}
	unsigned char* GetShortBuffer(short& value)
	{
		m_pBuffer = (unsigned char*)malloc(sizeof(short));
		WriteData((unsigned char*)&value, sizeof(value));
		return (unsigned char*)m_pBuffer;
	}
private:
	void WriteData(const unsigned char* pData, int nLen)
	{
		emEndian endian = cEndianDef::Instance()->GetEndian();
		if (endian == emBigEndian)
			Write(pData, nLen);
		else
			for (int i = 0; i < (int) nLen; i++)
				Write(pData + nLen - 1 - i, 1);
	}
	void Write(const unsigned char* pBuffer, int nLen)
	{
		for (int i = 0 ; i < nLen ; i++)
		{
			((unsigned char*)m_pBuffer)[m_nOffset + i] = *(pBuffer+i);
		}
		m_nOffset += nLen;
	}
private:
	void* m_pBuffer;
	int m_nOffset;
};
#endif