#pragma once
#include "IAudio.h"

class MockAudio : public IAudio
{
public:

	virtual void Play();

	virtual void Record();

	virtual void Stop();
};
