#include "stdafx.h"
#include "WaveOut.h"

WaveOut::WaveOut
	( UINT           deviceId
	, WAVEFORMATEX * format
	, DRVCALLBACK  * callback
	, void         * callbackParameter
	, DWORD          flags
	)
{
	Open
		( deviceId
		, format
		, reinterpret_cast<DWORD>(callback)
		, reinterpret_cast<DWORD>(callbackParameter)
		, flags | CALLBACK_FUNCTION
		);
}

WaveOut::~WaveOut()
{
	while (::waveOutClose(handle) == WAVERR_STILLPLAYING)
		::Sleep(10);
}

void WaveOut::PrepareHeader(WAVEHDR * header)
{
	switch (::waveOutPrepareHeader(handle, header, sizeof(WAVEHDR)))
	{
		case MMSYSERR_NOERROR:
			return; // success
		case MMSYSERR_INVALHANDLE:
			throw std::exception("Specified device handle is invalid.");
		case MMSYSERR_INVALPARAM:
			throw std::exception("The buffer's base address is not aligned with the sample size.");
		case MMSYSERR_NODRIVER:
			throw std::exception("No device driver is present.");
		case MMSYSERR_NOMEM:
			throw std::exception("Unable to allocate or lock memory.");
		default:
			throw std::exception("waveOutPrepareHeader failed for an unknown reason.");
	}
}

void WaveOut::Write(WAVEHDR * header)
{
	switch (::waveOutWrite(handle, header, sizeof(WAVEHDR)))
	{
		case MMSYSERR_NOERROR:
			return; // success
		case MMSYSERR_INVALHANDLE:
			throw std::exception("Specified device handle is invalid.");
		case MMSYSERR_NODRIVER:
			throw std::exception("No device driver is present.");
		case MMSYSERR_NOMEM:
			throw std::exception("Unable to allocate or lock memory.");
		case WAVERR_UNPREPARED:
			throw std::exception("Data block pointed to by the pwh parameter has not been prepared.");
		case WAVERR_BADFORMAT:
			throw std::exception("Unsupported wave format.");
		case WAVERR_STILLPLAYING:
			throw std::exception("The waveform output device is still playing some other data.");
		case WAVERR_SYNC:
			throw std::exception("The waveform output device is synchronous.");
		default:
			throw std::exception("waveOutWrite failed for an unknown reason.");
	}
}

bool WaveOut::UnprepareHeader(WAVEHDR * header)
{
	switch (::waveOutUnprepareHeader(handle, header, sizeof(WAVEHDR)))
	{
	case MMSYSERR_NOERROR:
		return true;
	case MMSYSERR_INVALHANDLE:
		throw std::exception("Specified device handle is invalid.");
	case MMSYSERR_INVALPARAM:
		throw std::exception("Invalid parameter passed.");
	case MMSYSERR_NODRIVER:
		throw std::exception("No device driver is present.");
	case MMSYSERR_NOMEM:
		throw std::exception("Unable to allocate or lock memory.");
	case WAVERR_STILLPLAYING:
		return false;
	default:
		throw std::exception("waveOutUnprepareHeader failed for an unknown reason.");
	}
}

void WaveOut::Open
	( UINT           deviceId
	, WAVEFORMATEX * format
	, DWORD          callback
	, DWORD          callbackParamater
	, DWORD          flags
	)
{
	switch (::waveOutOpen(&handle, deviceId, format, callback, callbackParamater, flags))
	{
		case MMSYSERR_NOERROR:
			return; // success
		case MMSYSERR_INVALHANDLE:
			throw std::exception("Specified device handle is invalid.");
		case MMSYSERR_BADDEVICEID:
			throw std::exception("Specified device identifier is out of range.");
		case MMSYSERR_NODRIVER:
			throw std::exception("No device driver is present.");
		case MMSYSERR_NOMEM:
			throw std::exception("Unable to allocate or lock memory.");
		case WAVERR_BADFORMAT:
			throw std::exception("Attempted to open with an unsupported waveform-audio format.");
		case WAVERR_SYNC:
			throw std::exception("Device is synchronous but waveOutOpen was called without using the WAVE_ALLOWSYNC flag.");
		default:
			throw std::exception("waveOutOpen failed for an unknown reason.");
	}
}
