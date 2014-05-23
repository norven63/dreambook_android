#pragma once

class CAudio;

class IAudioCB
{
	CAudio* mpAudio;
public:
	IAudioCB(CAudio* pa) :mpAudio(pa){}

	virtual void OnStart() {}
	virtual void OnStop() {}
};
class CAudio
{
	IAudioCB* m_pAudioCB;
public:
	CAudio() :m_pAudioCB(NULL) {}
	virtual ~CAudio() {}

	void SetCallBack(IAudioCB* cb) { m_pAudioCB = cb; }

	virtual void Play(double time) = 0;
	virtual void Stop() = 0;
	virtual unsigned char* GetBuffer() = 0;

	virtual void SetVolume(double volume){}
	virtual double GetDuration() const{ return 0; }
	virtual double GetCurrentTime() const{ return 0; }

	virtual bool IsPlaying() { return false; }
};

extern CAudio* OpenAudioFile(LPCTSTR szFilePath);
