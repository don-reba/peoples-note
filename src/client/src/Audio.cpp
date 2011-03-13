#include "stdafx.h"
#include "Audio.h"

#include <mmsystem.h>

Audio::Audio()
	: waveIn  (NULL)
	, waveOut (NULL)
{
	waveHeader.lpData = NULL;
}

Audio::~Audio()
{
	if (waveHeader.lpData)
		::waveOutUnprepareHeader(waveOut, &waveHeader, sizeof(waveHeader));
	if (waveOut)
		::waveOutClose(waveOut);
	if (waveIn)
		::waveInClose(waveIn);
}

void Audio::Play(const Blob & data)
{
	MMRESULT result;

	WAVEFORMATEX format;
	Blob         wav;
	ReadWav(data, format, wav);

	HWAVEOUT waveOut;
	result = ::waveOutOpen
		( &waveOut    // device handle
		, WAVE_MAPPER // let API choose best device
		, &format     // data format
		, reinterpret_cast<DWORD>(&Audio::waveOutProc) // callback
		, NULL        // callback parameter
		, CALLBACK_FUNCTION | WAVE_ALLOWSYNC
		);
	switch (result)
	{
		case MMSYSERR_NOERROR:
			break;
		case MMSYSERR_INVALHANDLE:
			// Specified device handle is invalid.
			return;
		case MMSYSERR_BADDEVICEID:
			// Specified device identifier is out of range.
			return;
		case MMSYSERR_NODRIVER:
			// No device driver is present.
			return;
		case MMSYSERR_NOMEM:
			// Unable to allocate or lock memory.
			return;
		case WAVERR_BADFORMAT:
			// Attempted to open with an unsupported waveform-audio format.
			return;
		case WAVERR_SYNC:
			// Device is synchronous but waveOutOpen was called without using the WAVE_ALLOWSYNC flag.
			return;
		default:
			// Unknown error.
			return;
	}

	WAVEHDR waveHeader = { 0 };
	waveHeader.lpData         = reinterpret_cast<LPSTR>(&wav[0]);
	waveHeader.dwBufferLength = wav.size();
	result = ::waveOutPrepareHeader(waveOut, &waveHeader, sizeof(waveHeader));
	switch (result)
	{
		case MMSYSERR_NOERROR:
			// Success.
			break;
		case MMSYSERR_INVALHANDLE:
			// Specified device handle is invalid.
			return;
		case MMSYSERR_INVALPARAM:
			// The buffer's base address is not aligned with the sample size.
			return;
		case MMSYSERR_NODRIVER:
			// No device driver is present.
			return;
		case MMSYSERR_NOMEM:
			// Unable to allocate or lock memory.
			return;
		default:
			// Unknown error.
			return;
	}

	result = ::waveOutWrite(waveOut, &waveHeader, sizeof(waveHeader));
	switch (result)
	{
		case MMSYSERR_NOERROR:
			// Success.
			break;
		case MMSYSERR_INVALHANDLE:
			// Specified device handle is invalid.
			return;
		case MMSYSERR_NODRIVER:
			// No device driver is present.
			return;
		case MMSYSERR_NOMEM:
			// Unable to allocate or lock memory.
			return;
		case WAVERR_UNPREPARED:
			// Data block pointed to by the pwh parameter has not been prepared.
			return;
		case WAVERR_BADFORMAT:
			// Unsupported wave format.
			return;
		case WAVERR_STILLPLAYING:
			// The waveform output device is still playing some other data.
			return;
		case WAVERR_SYNC:
			// The waveform output device is synchronous.
			return;
		default:
			// Unknown error.
			return;
	}
	Sleep(18000);
}
#include <mmreg.h>
void Audio::Record(Blob & data)
{
	//1.waveInOpen
	//2.waveInPrepareHeader
	//3.waveInAddBuffer
	//4.waveInStart
	//5.waveInUnprepareHeader
	//6.waveInClose

	MMRESULT result;

	WAVEFORMATEX format = { };
	GetBestRecordingFormat(format);
	format.nAvgBytesPerSec = 4 * format.nSamplesPerSec;
	format.nBlockAlign     = 4;
	format.cbSize          = sizeof(format);
	// wFormatTag      1
	// nChannels       2
	// nSamplesPerSec  44100
	// nAvgBytesPerSec 176400
	// nBlockAlign     4
	// wBitsPerSample  16
	// cbSize          16

	result = ::waveInOpen
		( &waveIn     // device handle
		, 0 // let the API choose the device
		, &format     // data format
		, reinterpret_cast<DWORD>(&Audio::waveInProc)
		, NULL
		, CALLBACK_FUNCTION
		);
	switch (result)
	{
		case MMSYSERR_NOERROR:
			// Success.
			break;
		case MMSYSERR_ALLOCATED:
			// Specified resource is already allocated.
			return;
		case MMSYSERR_BADDEVICEID:
			// Specified device identifier is out of range.
			return;
		case MMSYSERR_NODRIVER:
			// No device driver is present.
			return;
		case MMSYSERR_NOMEM:
			// Unable to allocate or lock memory.
			return;
		case WAVERR_BADFORMAT:
			// Attempted to open with an unsupported waveform-audio format.
			return;
		default:
			// Unknown error.
			return;
	}
}

void Audio::Stop()
{
	if (waveOut)
		::waveOutReset(waveOut);
}

//------------------
// utility functions
//------------------

void Audio::GetBestRecordingFormat(WAVEFORMATEX & format)
{
	UINT deviceCount(::waveInGetNumDevs());

	// find best device and format
	// assume better formats have higher ids
	// (this is so at the moment)
	UINT bestDeviceId (0);
	UINT bestFormat   (0);
	for (UINT deviceId(0); deviceId != deviceCount; ++deviceId)
	{
		WAVEINCAPS caps;
		::waveInGetDevCaps(deviceId, &caps, sizeof(caps));
		if (caps.dwFormats < bestFormat)
			continue;
		bestDeviceId = deviceId;
		bestFormat   = caps.dwFormats;
	}
	// 44.1   kHz, Stereo, 16-bit 
	if ((bestFormat & WAVE_FORMAT_4S16) == WAVE_FORMAT_4S16)
	{
		format.wFormatTag     = WAVE_FORMAT_4S16;
		format.nSamplesPerSec = 44100;
		format.nChannels      = 2;
		format.wBitsPerSample = 16;
		return;
	}
	// 44.1   kHz, Mono,   16-bit
	if ((bestFormat & WAVE_FORMAT_4M16) == WAVE_FORMAT_4M16)
	{
		format.wFormatTag     = WAVE_FORMAT_4M16;
		format.nSamplesPerSec = 44100;
		format.nChannels      = 1;
		format.wBitsPerSample = 16;
		return;
	}
	// 44.1   kHz, Stereo, 8-bit
	if ((bestFormat & WAVE_FORMAT_4S08) == WAVE_FORMAT_4S08)
	{
		format.wFormatTag     = WAVE_FORMAT_4S08;
		format.nSamplesPerSec = 44100;
		format.nChannels      = 2;
		format.wBitsPerSample = 8;
		return;
	}
	// 44.1   kHz, Mono,   8-bit
	if ((bestFormat & WAVE_FORMAT_4M08) == WAVE_FORMAT_4M08)
	{
		format.wFormatTag     = WAVE_FORMAT_4M08;
		format.nSamplesPerSec = 44100;
		format.nChannels      = 1;
		format.wBitsPerSample = 8;
		return;
	}
	// 22.05   kHz, Stereo, 16-bit 
	if ((bestFormat & WAVE_FORMAT_2S16) == WAVE_FORMAT_2S16)
	{
		format.wFormatTag     = WAVE_FORMAT_2S16;
		format.nSamplesPerSec = 22050;
		format.nChannels      = 2;
		format.wBitsPerSample = 16;
		return;
	}
	// 22.05   kHz, Mono,   16-bit
	if ((bestFormat & WAVE_FORMAT_2M16) == WAVE_FORMAT_2M16)
	{
		format.wFormatTag     = WAVE_FORMAT_2M16;
		format.nSamplesPerSec = 22050;
		format.nChannels      = 1;
		format.wBitsPerSample = 16;
		return;
	}
	// 22.05   kHz, Stereo, 8-bit
	if ((bestFormat & WAVE_FORMAT_2S08) == WAVE_FORMAT_2S08)
	{
		format.wFormatTag     = WAVE_FORMAT_2S08;
		format.nSamplesPerSec = 22050;
		format.nChannels      = 2;
		format.wBitsPerSample = 8;
		return;
	}
	// 22.05   kHz, Mono,   8-bit
	if ((bestFormat & WAVE_FORMAT_2M08) == WAVE_FORMAT_2M08)
	{
		format.wFormatTag     = WAVE_FORMAT_2M08;
		format.nSamplesPerSec = 22050;
		format.nChannels      = 1;
		format.wBitsPerSample = 8;
		return;
	}
	// 11.025   kHz, Stereo, 16-bit 
	if ((bestFormat & WAVE_FORMAT_1S16) == WAVE_FORMAT_1S16)
	{
		format.wFormatTag     = WAVE_FORMAT_1S16;
		format.nSamplesPerSec = 44100;
		format.nChannels      = 2;
		format.wBitsPerSample = 16;
		return;
	}
	// 11.025   kHz, Mono,   16-bit
	if ((bestFormat & WAVE_FORMAT_1M16) == WAVE_FORMAT_1M16)
	{
		format.wFormatTag     = WAVE_FORMAT_1M16;
		format.nSamplesPerSec = 44100;
		format.nChannels      = 1;
		format.wBitsPerSample = 16;
		return;
	}
	// 11.025   kHz, Stereo, 8-bit
	if ((bestFormat & WAVE_FORMAT_1S08) == WAVE_FORMAT_1S08)
	{
		format.wFormatTag     = WAVE_FORMAT_1S08;
		format.nSamplesPerSec = 44100;
		format.nChannels      = 2;
		format.wBitsPerSample = 8;
		return;
	}
	// 11.025   kHz, Mono,   8-bit
	if ((bestFormat & WAVE_FORMAT_1M08) == WAVE_FORMAT_1M08)
	{
		format.wFormatTag     = WAVE_FORMAT_1M08;
		format.nSamplesPerSec = 44100;
		format.nChannels      = 1;
		format.wBitsPerSample = 8;
		return;
	}
	throw std::exception("No recording device.");
}

// TODO: get a separate stream class
void Audio::Read
	( const Blob & data
	, size_t       begin
	, size_t       end
	, void       * destination
	)
{
	// safety off
	#pragma warning(push)
	#pragma warning(disable:4996)

	std::copy
		( data.begin() + begin
		, data.begin() + end
		, reinterpret_cast<unsigned char *>(destination)
		);

	// safety on
	#pragma warning(pop)
}

void Audio::ReadWav
	( const Blob   & data
	, WAVEFORMATEX & format
	, Blob         & wav
	)
{
	try
	{
		DWORD magicNumber;
		Read(data, 0, 4, &magicNumber);
		if (magicNumber != 0x46464952) // "RIFF"
			return;

		DWORD formatName;
		Read(data, 8, 12, &formatName);
		if (formatName != 0x45564157) // "WAVE"
			return;

		DWORD formatHeader;
		Read(data, 12, 16, &formatHeader);
		if (formatHeader != 0x20746D66) // " fmt"
			return;

		Read(data, 16, 20, &format.cbSize);
		if (format.cbSize != 16)
			return;

		Read(data, 20, 22, &format.wFormatTag);
		Read(data, 22, 24, &format.nChannels);
		Read(data, 24, 28, &format.nSamplesPerSec);
		Read(data, 28, 32, &format.nAvgBytesPerSec);
		Read(data, 32, 34, &format.nBlockAlign);
		Read(data, 34, 36, &format.wBitsPerSample);

		DWORD dataHeader;
		Read(data, 36, 40, &dataHeader);
		if (dataHeader != 0x61746164) // "data"
			return;

		DWORD dataSize;
		Read(data, 40, 44, &dataSize);
		if (dataSize <= 0)
			return;

		wav.resize(dataSize);
		Read(data, 44, 44 + dataSize, &wav[0]);
	}
	catch (...)
	{
	}
}

void CALLBACK Audio::waveInProc(
	HWAVEIN hwi,
	UINT    uMsg,
	DWORD   dwInstance,
	DWORD   dwParam1,
	DWORD   dwParam2
	)
{
}

void CALLBACK Audio::waveOutProc
	( HWAVEOUT hwo
	, UINT     uMsg
	, DWORD    dwInstance
	, DWORD    dwParam1
	, DWORD    dwParam2
	)
{
}
