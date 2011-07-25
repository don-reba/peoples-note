#pragma once
#include "IAudioPlayer.h"

class MockAudioPlayer : public IAudioPlayer
{
public:

	virtual void Play(LPCWSTR path);

	virtual void Stop();
};
