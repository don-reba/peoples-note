#pragma once

#include "Blob.h"

class IAudioPlayer
{
public:

	virtual void Play(LPCWSTR path) = 0;

	virtual void Stop() = 0;
};
