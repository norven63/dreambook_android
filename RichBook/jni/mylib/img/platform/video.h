#pragma once


#include <memory>
class CVideo;

class IVideoCB
{
protected:
	CVideo* mpVideo;
	int mnWidth;
	int mnHeight;
public:
	IVideoCB(CVideo* pv) :mpVideo(pv),mnWidth(0),mnHeight(0){}
	virtual ~IVideoCB(){}

	void SetVideo(CVideo* pVideo) {mpVideo=pVideo;}

	virtual void OnInit(int w,int h) { mnWidth=w;mnHeight=h;}
	virtual void OnStart() {}
	virtual void OnStop() {}
	virtual void OnBufferCB(double SampleTime, 
		BYTE *pBuffer, 
		long bufferLen)
	{
	}
};

class CVideo
{
protected:
	IVideoCB* m_pVideoCB;
	int mnWidth;
	int mnHeight;

public:
	CVideo() :m_pVideoCB(NULL),mnWidth(0),mnHeight(0) {}
	virtual ~CVideo() {}

	void SetCallBack(IVideoCB* cb) { m_pVideoCB = cb; OnSetCallback(cb); }
	
	int GetWidth() const { return mnWidth; }
	int GetHeight() const { return mnHeight; }

	virtual bool LoadFile(LPCTSTR szVideoPath)=0;
	virtual void Play(double time)=0;
	virtual void Stop()=0;
	virtual void OnSetCallback(IVideoCB* cb) {}

	virtual void SetVolume(double volume){}
	virtual double GetDuration() const{ return 0; }
	virtual double GetCurTime() const{ return 0; }

	virtual bool IsPlaying() { return false; }
};


extern CVideo* OpenVideoFile(LPCTSTR szFilePath,IVideoCB* pCB);
