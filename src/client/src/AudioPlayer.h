#pragma once
#include "IAudioPlayer.h"

#include "WaveOut.h"

typedef struct tWAVEFORMATEX WAVEFORMATEX;

class AudioPlayer : public IAudioPlayer
{
private:

	// This critical section synchronizes three threads:
	// - main (UI) thread
	// - audio loading thread
	// - audio callback thread
	CRITICAL_SECTION lock;

	HANDLE thread;

	boost::shared_ptr<ISqlBlob> blob;

	bool isPlaying;
	bool isStopRequested;

	WAVEHDR * waveBlocks;
	long waveCurrentBlock;
	long waveFreeBlockCount;

	static const int blockCount = 20;
	static const int blockSize  = 4096;
	static const int bufferSize = 2048;

	static const int wavHeaderSize = 44;

public:

	AudioPlayer();
	~AudioPlayer();

public:

	virtual void Play(boost::shared_ptr<ISqlBlob> & blob);

	virtual void Stop();

// utility functions

private:

	static WAVEHDR * AllocateBlocks(int size, int count);

	static void DeallocateBlocks(WAVEHDR * blocks);

	void FlushWav(WaveOut & waveOut);

	void Play();

	static DWORD WINAPI Play(LPVOID param);

	static void ReadWavHeader
		( ISqlBlob     & blob
		, WAVEFORMATEX & format
		, int          & dataSize
		);

	static void ReadWavHeader
		( const BYTE   * data
		, WAVEFORMATEX & format
		, int          & dataSize
		);

	void UnprepareHeaders(WaveOut & waveOut);

	static void (CALLBACK WaveOutProc)
		( HANDLE waveOut
		, UINT   message
		, DWORD  instanceParameter
		, DWORD  parameter1
		, DWORD  parameter2
		);

	void WaveOutProc
		( HANDLE waveOut
		, UINT   message
		, DWORD  parameter1
		, DWORD  parameter2
		);

	void WriteWav(WaveOut & waveOut, const BYTE * data, int size);
};
