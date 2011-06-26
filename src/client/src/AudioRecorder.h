#pragma once
#include "IAudioRecorder.h"

typedef struct tWAVEFORMATEX WAVEFORMATEX;

class AudioRecorder : public IAudioRecorder
{
private:

	HWAVEIN waveIn;

public:

	AudioRecorder();
	~AudioRecorder();

public:

	virtual void Record(Blob & data);

	virtual void Stop();

// utility functions

private:

	static void GetBestRecordingFormat(WAVEFORMATEX & format);

	static void CALLBACK waveInProc(
		HWAVEIN hwi,
		UINT    uMsg,
		DWORD   dwInstance,
		DWORD   dwParam1,
		DWORD   dwParam2
		);
};
