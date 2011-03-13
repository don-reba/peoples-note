#pragma once
#include "IAudio.h"

typedef struct tWAVEFORMATEX WAVEFORMATEX;

class Audio : public IAudio
{
private:

	WAVEHDR  waveHeader;
	HWAVEIN  waveIn;
	HWAVEOUT waveOut;

public:

	Audio();
	~Audio();

public:

	virtual void Play(const Blob & data);

	virtual void Record(Blob & data);

	virtual void Stop();

// utility functions

private:

	static void GetBestRecordingFormat(WAVEFORMATEX & format);

	static void Read
		( const Blob & data
		, size_t       begin
		, size_t       end
		, void       * destination
		);

	static void ReadWav
		( const Blob   & blob
		, WAVEFORMATEX & format
		, Blob         & data
		);

	static void CALLBACK waveInProc(
		HWAVEIN hwi,
		UINT    uMsg,
		DWORD   dwInstance,
		DWORD   dwParam1,
		DWORD   dwParam2
		);

	static void CALLBACK waveOutProc
		( HWAVEOUT hwo
		, UINT     uMsg
		, DWORD    dwInstance
		, DWORD    dwParam1
		, DWORD    dwParam2
		);

};
