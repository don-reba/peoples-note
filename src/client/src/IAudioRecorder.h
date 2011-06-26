#pragma once

#include "Blob.h"

class IAudioRecorder
{
public:

	virtual void Record(Blob & data) = 0;

	virtual void Stop() = 0;
};
