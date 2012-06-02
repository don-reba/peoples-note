#pragma once
#include "IAudioPlayer.h"

class MockAudioPlayer : public IAudioPlayer
{
public:

	bool isPlaying;
	boost::shared_ptr<ISqlBlob> data;

public:

	virtual void Play(boost::shared_ptr<ISqlBlob> & data);

	virtual void Stop();
};
