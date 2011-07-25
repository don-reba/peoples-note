#pragma once
#include "IAudioRecorder.h"

class MockAudioRecorder : public IAudioRecorder
{
public:

	virtual void Record(Blob & data);

	virtual void Stop();
};
