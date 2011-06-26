// Code partly based on:
// http://www.planet-source-code.com/vb/scripts/ShowCode.asp?txtCodeId=4422&lngWId=3

#include "stdafx.h"
#include "AudioPlayer.h"

AudioPlayer::AudioPlayer()
{
	::InitializeCriticalSection(&criticalSection);

	waveBlocks = AllocateBlocks(blockSize, blockCount);
}

AudioPlayer::~AudioPlayer()
{
	::DeleteCriticalSection(&criticalSection);
	DeallocateBlocks(waveBlocks);
}

void AudioPlayer::Play(LPCWSTR path)
{
	std::ifstream file(path, std::ios::binary);

	WAVEFORMATEX format;
	int          dataSize;
	ReadWavHeader(file, format, dataSize);

	WaveOut waveOut(WAVE_MAPPER, &format, &AudioPlayer::WaveOutProc, this, WAVE_ALLOWSYNC);

	std::vector<char> buffer(bufferSize);

	waveCurrentBlock   = 0;
	waveFreeBlockCount = blockCount;
	while(dataSize > 0 && !file.eof())
	{
		file.read(&buffer[0], min(dataSize, bufferSize));
		WriteWav(waveOut, &buffer[0], file.gcount());
		dataSize -= file.gcount();
	}
	FlushWav(waveOut);

	while(waveFreeBlockCount < blockCount)
		Sleep(10);

	for(int i(0); i < waveFreeBlockCount; i++) 
	{
		if(waveBlocks[i].dwFlags & WHDR_PREPARED)
			waveOut.UnprepareHeader(&waveBlocks[i]);
	}
}

void AudioPlayer::Stop()
{
//	if (waveOut)
//		::waveOutReset(waveOut);
}

//------------------
// utility functions
//------------------

// Based on: http://www.planet-source-code.com/vb/scripts/ShowCode.asp?txtCodeId=4422&lngWId=3
WAVEHDR * AudioPlayer::AllocateBlocks(int size, int count)
{
	const int bufferSize = count * (size + sizeof(WAVEHDR));
	char * buffer(new char[bufferSize]);
	::SecureZeroMemory(buffer, bufferSize);

	WAVEHDR * blocks(reinterpret_cast<WAVEHDR*>(buffer));
	buffer += sizeof(WAVEHDR) * count;
	for(int i(0); i != count; ++i)
	{
		blocks[i].dwBufferLength = size;
		blocks[i].lpData         = buffer;
		buffer += size;
	}
	return blocks;
}

// Based on: http://www.planet-source-code.com/vb/scripts/ShowCode.asp?txtCodeId=4422&lngWId=3
void AudioPlayer::DeallocateBlocks(WAVEHDR * blocks)
{
	delete [] blocks;
}

void AudioPlayer::ReadWavHeader
	( std::istream & stream
	, WAVEFORMATEX & format
	, int          & dataSize
	)
{
	DWORD magicNumber;
	stream.read(reinterpret_cast<char*>(&magicNumber), 4);
	if (magicNumber != 0x46464952) // "RIFF"
		throw std::exception("Wrong file format.");

	stream.read(reinterpret_cast<char*>(&magicNumber), 4);

	DWORD formatName;
	stream.read(reinterpret_cast<char*>(&formatName), 4);
	if (formatName != 0x45564157) // "WAVE"
		throw std::exception("Wrong file format.");

	DWORD formatHeader;
	stream.read(reinterpret_cast<char*>(&formatHeader), 4);
	if (formatHeader != 0x20746D66) // " fmt"
		throw std::exception("Wrong file format.");

	stream.read(reinterpret_cast<char*>(&format.cbSize), 4);
	if (format.cbSize != 16)
		throw std::exception("Wrong file format.");

	stream.read(reinterpret_cast<char*>(&format.wFormatTag),      2);
	stream.read(reinterpret_cast<char*>(&format.nChannels),       2);
	stream.read(reinterpret_cast<char*>(&format.nSamplesPerSec),  4);
	stream.read(reinterpret_cast<char*>(&format.nAvgBytesPerSec), 4);
	stream.read(reinterpret_cast<char*>(&format.nBlockAlign),     2);
	stream.read(reinterpret_cast<char*>(&format.wBitsPerSample),  2);

	DWORD dataHeader;
	stream.read(reinterpret_cast<char*>(&dataHeader), 4);
	if (dataHeader != 0x61746164) // "data"
		throw std::exception("Wrong file format.");

	stream.read(reinterpret_cast<char*>(&dataSize), 4);
	if (dataSize < 0)
		throw std::exception("Wrong file format.");
}

void AudioPlayer::WaveOutProc
	( HANDLE waveOut
	, UINT   message
	, DWORD  instanceParameter
	, DWORD  parameter1
	, DWORD  parameter2
	)
{
	reinterpret_cast<AudioPlayer*>(instanceParameter)->
		WaveOutProc(waveOut, message, parameter1, parameter2);
}

void AudioPlayer::WaveOutProc
	( HANDLE waveOut
	, UINT   message
	, DWORD  parameter1
	, DWORD  parameter2
	)
{
	if(message != WOM_DONE)
		return;

	EnterCriticalSection(&criticalSection);
	waveFreeBlockCount++;
	LeaveCriticalSection(&criticalSection);
}

void AudioPlayer::FlushWav(WaveOut & waveOut)
{
	WAVEHDR * current(&waveBlocks[waveCurrentBlock]);
	if (current->dwUser > 0)
	{
		current->dwBufferLength = current->dwUser;
		current->dwUser = 0;

		waveOut.PrepareHeader(current);
		waveOut.Write(current);
	}
}

void AudioPlayer::WriteWav(WaveOut & waveOut, char * data, int size)
{
	while(size > 0)
	{
		WAVEHDR * current(&waveBlocks[waveCurrentBlock]);

		if(current->dwFlags & WHDR_PREPARED)
			waveOut.UnprepareHeader(current);

		const int remainder(blockSize - current->dwUser);
		if (size < remainder)
		{
			::memcpy(current->lpData + current->dwUser, data, size);
			current->dwUser += size;
			break;
		}
		::memcpy(current->lpData + current->dwUser, data, remainder);
		size -= remainder;
		data += remainder;
		current->dwBufferLength = blockSize;

		waveOut.PrepareHeader(current);
		waveOut.Write(current);

		::EnterCriticalSection(&criticalSection);
		waveFreeBlockCount--;
		::LeaveCriticalSection(&criticalSection);

		while(waveFreeBlockCount == 0)
			Sleep(10);

		waveCurrentBlock = (waveCurrentBlock + 1) % blockCount;
		current->dwUser = 0;
	}
}
