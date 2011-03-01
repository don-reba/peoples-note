#include "stdafx.h"
#include "Audio.h"

#include <mmsystem.h>

Audio::Audio()
{
}

void Audio::Play(Blob & blob)
{
	//1.waveOutOpen
	//2.waveOutPrepareHeader
	//3.waveOutWrite
	//4.waveOutUnprepareHeader
	//5.waveOutClose

	try
	{
		WAVEFORMATEX format;

		DWORD magicNumber;
		Read(blob, 0, 4, &magicNumber);
		if (magicNumber != 0x46464952) // RIFF
			return;

		DWORD formatName;
		Read(blob, 8, 12, &formatName);
		if (formatName != 0x45564157) // WAVE
			return;

		DWORD formatHeader;
		Read(blob, 12, 16, &formatHeader);
		if (formatHeader != 0x20746D66) // fmt
			return;

		Read(blob, 16, 20, &format.cbSize);
		if (format.cbSize != 16)
			return;

		Read(blob, 20, 22, &format.wFormatTag);
		Read(blob, 22, 24, &format.nChannels);
		Read(blob, 24, 28, &format.nSamplesPerSec);
		Read(blob, 28, 32, &format.nAvgBytesPerSec);
		Read(blob, 32, 34, &format.nBlockAlign);
		Read(blob, 34, 36, &format.wBitsPerSample);

		DWORD dataHeader;
		Read(blob, 36, 40, &dataHeader); // data
		if (dataHeader != 0x61746164)
			return;
	}
	catch (...)
	{
	}
	
	// check magic number

	//HWAVEOUT waveOut;
	//waveOutOpen(&waveOut, WAVE_MAPPER, 
}

void Audio::Record(Blob & blob)
{
}

void Audio::Stop()
{
}

// TODO: get a separate stream class
void Audio::Read
	( Blob   & blob
	, size_t   begin
	, size_t   end
	, void   * destination
	)
{
#pragma warning(push)
#pragma warning(disable:4996)

	std::copy
		( blob.begin() + begin
		, blob.begin() + end
		, reinterpret_cast<unsigned char *>(destination)
		);

#pragma warning(pop)
}
