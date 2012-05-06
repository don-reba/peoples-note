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

	std::vector<BYTE> buffer(bufferSize);

	waveCurrentBlock   = 0;
	waveFreeBlockCount = blockCount;
	while(dataSize > 0 && !file.eof())
	{
		file.read(reinterpret_cast<char*>(&buffer[0]), min(dataSize, bufferSize));
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

void AudioPlayer::Play(const Blob & data)
{
	WAVEFORMATEX format;
	int          dataSize;
	ReadWavHeader(&data[0], format, dataSize);

	WaveOut waveOut(WAVE_MAPPER, &format, &AudioPlayer::WaveOutProc, this, WAVE_ALLOWSYNC);

	std::vector<char> buffer(bufferSize);

	waveCurrentBlock   = 0;
	waveFreeBlockCount = blockCount;
	for (int i(0), size(data.size()); i < size; i += bufferSize)
	{
		size_t count(min(size - i, bufferSize));
		WriteWav(waveOut, &data[i], count);
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
	BYTE buffer[wavHeaderSize];
	stream.read(reinterpret_cast<char*>(buffer), wavHeaderSize);
	ReadWavHeader(buffer, format, dataSize);
}

void AudioPlayer::ReadWavHeader
	( const BYTE   * data
	, WAVEFORMATEX & format
	, int          & dataSize
	)
{
	DWORD magicNumber(*reinterpret_cast<const DWORD *>(data + 0));
	if (magicNumber != 0x46464952) // "RIFF"
		throw std::exception("Wrong file format.");

	DWORD formatName(*reinterpret_cast<const DWORD *>(data + 8));
	if (formatName != 0x45564157) // "WAVE"
		throw std::exception("Wrong file format.");

	DWORD formatHeader(*reinterpret_cast<const DWORD *>(data + 12));
	if (formatHeader != 0x20746D66) // " fmt"
		throw std::exception("Wrong file format.");

	format.cbSize = static_cast<WORD>(*reinterpret_cast<const DWORD *>(data + 16));
	if (format.cbSize != 16)
		throw std::exception("Wrong file format.");


	format.wFormatTag      = *reinterpret_cast<const  WORD *>(data + 20);
	format.nChannels       = *reinterpret_cast<const  WORD *>(data + 22);
	format.nSamplesPerSec  = *reinterpret_cast<const DWORD *>(data + 24);
	format.nAvgBytesPerSec = *reinterpret_cast<const DWORD *>(data + 28);
	format.nBlockAlign     = *reinterpret_cast<const  WORD *>(data + 32);
	format.wBitsPerSample  = *reinterpret_cast<const  WORD *>(data + 34);

	DWORD dataHeader(*reinterpret_cast<const DWORD *>(data + 36));
	if (dataHeader != 0x61746164) // "data"
		throw std::exception("Wrong file format.");

	dataSize = *reinterpret_cast<const DWORD *>(data + 40);
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

void AudioPlayer::WriteWav(WaveOut & waveOut, const BYTE * data, int size)
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
