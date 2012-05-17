#pragma once

class ISqlBlob;

class IAudioPlayer
{
public:

	virtual void Play(boost::shared_ptr<ISqlBlob> & blob) = 0;

	virtual void Stop() = 0;
};
