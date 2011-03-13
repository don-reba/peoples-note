#pragma once

#include "Blob.h"

class IAudio
{
public:

	virtual void Play(const Blob & data) = 0;

	virtual void Record(Blob & data) = 0;

	virtual void Stop() = 0;
};
