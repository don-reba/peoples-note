#include "stdafx.h"
#include "AudioRecorder.h"

#include <mmreg.h>
#include <mmsystem.h>

AudioRecorder::AudioRecorder()
	: waveIn (NULL)
{
}

AudioRecorder::~AudioRecorder()
{
	if (waveIn)
		::waveInClose(waveIn);
}

void AudioRecorder::Record(Blob & data)
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
		, reinterpret_cast<DWORD>(&AudioRecorder::waveInProc)
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

void AudioRecorder::Stop()
{
}

//------------------
// utility functions
//------------------


void AudioRecorder::GetBestRecordingFormat(WAVEFORMATEX & format)
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


void CALLBACK AudioRecorder::waveInProc(
	HWAVEIN hwi,
	UINT    uMsg,
	DWORD   dwInstance,
	DWORD   dwParam1,
	DWORD   dwParam2
	)
{
}
