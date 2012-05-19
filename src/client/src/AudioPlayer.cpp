// Code partly based on:
// http://www.planet-source-code.com/vb/scripts/ShowCode.asp?txtCodeId=4422&lngWId=3

#include "stdafx.h"
#include "AudioPlayer.h"

#include "ISqlBlob.h"

using namespace boost;

AudioPlayer::AudioPlayer()
	: isPlaying (false)
	, thread    (NULL)
{
	waveBlocks = AllocateBlocks(blockSize, blockCount);
}

AudioPlayer::~AudioPlayer()
{
	if (thread != NULL)
		::CloseHandle(thread);
	DeallocateBlocks(waveBlocks);
}

void AudioPlayer::Play(shared_ptr<ISqlBlob> & blob)
{
	Stop();
	this->blob = blob;
	thread = ::CreateThread
		( NULL               // lpsa
		, 0                  // cbStack
		, &AudioPlayer::Play // lpStartAddr
		, this               // lpvThreadParam
		, 0                  // fdwCreate
		, NULL               // lpIDThread
		);
}

void AudioPlayer::Stop()
{
	isStopRequested = true;
	::WaitForSingleObject(thread, INFINITE);
	isStopRequested = false;
	this->blob.reset();
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

void AudioPlayer::Play()
{
	WAVEFORMATEX format;
	int          dataSize;
	ReadWavHeader(*blob, format, dataSize);

	WaveOut waveOut(WAVE_MAPPER, &format, &AudioPlayer::WaveOutProc, this, WAVE_ALLOWSYNC);

	Blob buffer(bufferSize);

	try
	{
		waveCurrentBlock   = 0;
		waveFreeBlockCount = blockCount;
		int offset(wavHeaderSize);
		while(dataSize > 0)
		{
			if (isStopRequested)
			{
				waveOut.Reset();
				UnprepareHeaders(waveOut);
				return;
			}

			blob->Read(offset, min(dataSize, bufferSize), buffer);
			if (buffer.empty())
				break;
			WriteWav(waveOut, &buffer[0], buffer.size());
			offset   += buffer.size();
			dataSize -= buffer.size();
		}
	}
	catch (const std::exception &)
	{
		// try to do with what we have
	}
	FlushWav(waveOut);

	while(waveFreeBlockCount < blockCount)
		Sleep(10);

	UnprepareHeaders(waveOut);
}

DWORD WINAPI AudioPlayer::Play(LPVOID param)
{
	reinterpret_cast<AudioPlayer*>(param)->Play();
	return 0;
}

void AudioPlayer::ReadWavHeader
	( ISqlBlob     & blob
	, WAVEFORMATEX & format
	, int          & dataSize
	)
{
	Blob buffer(wavHeaderSize);
	blob.Read(0, wavHeaderSize, buffer);
	ReadWavHeader(&buffer[0], format, dataSize);
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

void AudioPlayer::UnprepareHeaders(WaveOut & waveOut)
{
	for(int i(0); i < waveFreeBlockCount; i++) 
	{
		if(waveBlocks[i].dwFlags & WHDR_PREPARED)
			waveOut.UnprepareHeader(&waveBlocks[i]);
	}
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

	InterlockedIncrement(&waveFreeBlockCount);
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
	// copy data into blocks in a circular buffer
	while(size > 0)
	{
		WAVEHDR * current(&waveBlocks[waveCurrentBlock]);

		if(current->dwFlags & WHDR_PREPARED)
			waveOut.UnprepareHeader(current);

		const int remainder(blockSize - current->dwUser);

		// write a partial block
		if (size < remainder)
		{
			::memcpy(current->lpData + current->dwUser, data, size);
			current->dwUser += size;
			break;
		}

		// write a complete block
		::memcpy(current->lpData + current->dwUser, data, remainder);
		size -= remainder;
		data += remainder;
		current->dwBufferLength = blockSize;

		// enqueue the block for playback
		waveOut.PrepareHeader(current);
		waveOut.Write(current);

		InterlockedDecrement(&waveFreeBlockCount);

		// move onto the next block, as soon as one becomes available
		while(waveFreeBlockCount == 0)
			Sleep(10);
		waveCurrentBlock = (waveCurrentBlock + 1) % blockCount;
		current->dwUser = 0;
	}
}
