#pragma once
#include "IAudioPlayer.h"

#include "WaveOut.h"

#include <fstream>

typedef struct tWAVEFORMATEX WAVEFORMATEX;

class AudioPlayer : public IAudioPlayer
{
private:

	CRITICAL_SECTION criticalSection;

	WAVEHDR * waveBlocks;
	int waveCurrentBlock;
	int waveFreeBlockCount;

	static const int blockCount = 20;
	static const int blockSize  = 4096;
	static const int bufferSize = 2048;

public:

	AudioPlayer();
	~AudioPlayer();

public:

	virtual void Play(LPCWSTR path);

	virtual void Stop();

// utility functions

private:

	static WAVEHDR * AllocateBlocks(int size, int count);

	static void DeallocateBlocks(WAVEHDR * blocks);

	static void ReadWavHeader
		( std::istream & stream
		, WAVEFORMATEX & format
		, int          & dataSize
		);

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

	void FlushWav(WaveOut & waveOut);

	void WriteWav(WaveOut & waveOut, char * data, int size);
};
