#pragma once

#include "Blob.h"

class IAudio
{
public:

	virtual void Play(Blob & blob) = 0;

	virtual void Record(Blob & blob) = 0;

	virtual void Stop() = 0;
};
