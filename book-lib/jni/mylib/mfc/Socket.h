// This Winsock Wrapper replaces all MFC socket classes (CSocket, CAsyncSocket
// CCeSocket). It offers async notifications, buffered read and easy data access
// functions.
//
// Coded by Marco Zaratti / marco.zaratti@gmail.it / 2006.04.05
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CESOCKET_H__DEAC64AC_C034_4162_9C57_5C75CBEDE2BB__INCLUDED_)
#define AFX_CESOCKET_H__DEAC64AC_C034_4162_9C57_5C75CBEDE2BB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define HTTP_REQ_SIZE 512
#define HTTP_RES_SIZE 512

class cSocket  
{
public:
	//! Possible socket states
	enum socketState {NONE=0, DISCONNECTING, CREATED, CONNECTED};
	enum commerro{emNoException = 1,emServerNoConnect = 3,emFileNotExistOnServer= 32,emFileCanNotBeSaved = 33,emFileCanNotBeRead = 34};

	cSocket();
	~cSocket();
	bool Create(int socketType);
	bool Connect(const char* hostStr, unsigned int remotePort);
	void Disconnect();
	int Send(const char* buf, int len);
	unsigned char*	 Receive(int& nSize,int nTimeOut);
	int DownLoadFile(FILE* fileHandle,const char* buf, int len,int nTimeOut);
	int UpLoadFile(FILE* fileHandle,const char* FNSBuf, int FNSlen,const char* buf, int len,int nTimeOut = 60);// FNS = file name on server


	bool ConnectHTTP(const char* pHttpCom);
	int	SendHTTP(const char* buf, int len);
	unsigned char* ReceiveHttp(int& nSize,int nDatalen,int nTimeOut);
	const char*		GetResponseHeader(int &Length,int nTimeOut);
	int		GetServerState();											
	int		GetField(const char* pLenField,int nFieldSize, char* sValue,int nMaxLen);
	int		GetContenLenField();
	int		GetResponseLine(char *pLine,int nMaxLength);
	int		GetRequestHeader(char *pHeader,int nMaxLength) const;
	void	FormatPostRequestHeader(const char *pServer,const char *pObject,int nPort,int nDataLen);
	int HttpDownLoadFile(const char* pServer,const char* pObj,int nPort,FILE* fileHandle,const char* buf, int len,int nTimeOut);
	int HttpUpLoadFile(const char* pServer,const char* pObj,int nPort,FILE* fileHandle,const char* buf, int len,int nTimeOut);
private:
	bool IsConnectTimeOut(int& s, int nTimeOut);
	bool IsTimeOut(int& s,int nTimeOut);
	bool NewTCPSocket(int& s);
	int SendRequest(int& s,const char* buf, int len);
	int AfterUploadAspect(unsigned char*& pBuf,int& s,int erro);
private:
	bool m_bWSAStarted;
	unsigned char*	m_pbuffer;
	int m_s;
	int m_socketType;
	socketState m_socketState;
	char* m_premoteAddress;
	char* m_plocalAddress;
	char m_requestheader[HTTP_REQ_SIZE];
	int m_nReqSize;
	char m_ResponseHeader[HTTP_RES_SIZE];
	int m_nRespSize;
	int m_nCurIndex;
	bool m_bResponsed;
};

#endif
