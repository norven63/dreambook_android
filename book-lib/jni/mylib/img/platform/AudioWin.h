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


class AudioWin
{
public:
	AudioWin(/*PlatformEnvPc * pEnv,*/ const char * fileName);
	~AudioWin();

	void play(double time);

	void stop();

	void setVolume(double volume);

	double duration() const;
	double currentTime() const;

	bool isPlaying() const {return isPlaying_;}
	bool isEnded() const;

	//run thread
	void operator()();

private:
// 	PlatformEnvPc * pEnv_;
// 	mutable boost::mutex mutex_;
	bool isPlaying_;
	bool terminate_;
// 	boost::thread decodeThread_;

	void loadFile(const char * fileName);


	double curTime_;
	double duration_;
	IPin * findAudioOutputPin();

	CComPtr<IGraphBuilder> graphBuilder_;
	CComPtr<IMediaControl> mediaControl_;
	CComPtr<IMediaSeeking> mediaSeeking_;
	CComPtr<IMediaEvent> mediaEvent_;
	CComPtr<IMediaFilter> mediaFilter_;
	CComPtr<IBaseFilter> source_;
	CComPtr<IBaseFilter> audioRenderer_;
	CComPtr<IBasicAudio> basicAudio_;

};

