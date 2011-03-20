#pragma once
#include "IAudio.h"

class MockAudio : public IAudio
{
public:

	virtual void Play(const Blob & data);

	virtual void Record(Blob & data);

	virtual void Stop();
};
