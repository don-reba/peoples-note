#pragma once

class ISqlBlob;

class IAudioPlayer
{
public:

	virtual void Play(ISqlBlob & blob) = 0;

	virtual void Stop() = 0;
};
