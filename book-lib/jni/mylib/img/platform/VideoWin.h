// 仅在windows平台下使用
#pragma once
#include <tchar.h>
#include <DShow.h>
#pragma include_alias( "dxtrans.h", "qedit.h" )
#define __IDxtCompositor_INTERFACE_DEFINED__
#define __IDxtAlphaSetter_INTERFACE_DEFINED__
#define __IDxtJpeg_INTERFACE_DEFINED__
#define __IDxtKey_INTERFACE_DEFINED__

#include <atlbase.h>
#include "QEdit.h"
#include "video.h"
#include "audio.h"


class SampleGrabberCB : public ISampleGrabberCB
{
public:
	SampleGrabberCB()
	{
		mpVideoCB=NULL;
	}

	~SampleGrabberCB()
	{
	}

	STDMETHODIMP_(ULONG) AddRef() { return 1; }
	STDMETHODIMP_(ULONG) Release() { return 0; }
	STDMETHODIMP QueryInterface(REFIID riid, void ** ppv)
	{
		if( riid == IID_ISampleGrabberCB || riid == IID_IUnknown )
		{
			*ppv = (VOID *) static_cast<ISampleGrabberCB*> ( this );
			return NOERROR;
		}
		return E_NOINTERFACE;
	}

	STDMETHODIMP SampleCB(DOUBLE SampleTime, IMediaSample * pSample )
	{
		return S_OK;
	}

	STDMETHODIMP BufferCB(double SampleTime, BYTE *pBuffer, long bufferLen)
	{
		if(mpVideoCB)
			mpVideoCB->OnBufferCB(SampleTime,pBuffer,bufferLen);
		return S_OK;
	}

	void init(const AM_MEDIA_TYPE & mt)
	{
		VIDEOINFOHEADER *vih = (VIDEOINFOHEADER *)mt.pbFormat;
		if(mpVideoCB)
			mpVideoCB->OnInit(vih->bmiHeader.biWidth,
			vih->bmiHeader.biHeight);
	}

	void SetVideoCB(IVideoCB* pCB) { mpVideoCB=pCB; }
private:
	IVideoCB* mpVideoCB;
};


class VideoWin : public CVideo
{
public:
	VideoWin();
	~VideoWin();

	virtual bool LoadFile(LPCTSTR szVideoPath);
	virtual void Play(double time);
	virtual void Stop();
	virtual void OnSetCallback(IVideoCB* cb);

	virtual void SetVolume(double volume);
	virtual double GetDuration() const{ return duration_; }
	virtual double GetCurTime()const;

	virtual bool IsPlaying() { return isPlaying_; }


	//run thread
	void operator()();

private:
	//PlatformEnvPc * pEnv_;
	//mutable boost::mutex mutex_;
	bool isPlaying_;
	bool terminate_;
	//boost::thread decodeThread_;

	//VideoWinApi * apiVideo_;

	double curTime_;
	double duration_;
	IPin * findAudioOutputPin();

	AM_MEDIA_TYPE mt_;

	CComPtr<IGraphBuilder> graphBuilder_;
	CComPtr<IMediaControl> mediaControl_;
	CComPtr<IMediaSeeking> mediaSeeking_;
	CComPtr<IMediaEvent>  mediaEvent_;
	CComPtr<IMediaFilter> mediaFilter_;
	CComPtr<IBaseFilter> source_;
	CComPtr<IBaseFilter> audioRenderer_;
	CComPtr<IBasicAudio> basicAudio_;
	CComPtr<IBaseFilter> videoRenderer_;
	CComPtr<IBaseFilter> sampleGrabberF_;
	CComPtr<ISampleGrabber> sampleGrabber_;

	SampleGrabberCB sampleGrabberCb_;
};
