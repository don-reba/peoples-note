#pragma once
#include "IAudio.h"

class IFile;

class Audio : public IAudio
{
private:

	IFile & file;

public:

	Audio(IFile & file);

public:

	virtual void Play();

	virtual void Record();

	virtual void Stop();
};
