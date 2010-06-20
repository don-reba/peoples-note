#pragma once

#include "IAnimator.h"

#include <list>

class Animator : public IAnimator
{
// types

private:

	struct Record
	{
		int       id;
		Animation animation;
		DWORD     startTime;
	};

// data

private:

	std::list<Record> records;
	int               lastId;

// interface

public:

	Animator();

// IAnimator implementation

public:

	virtual void StepFrame();

	virtual bool IsRunning();

	virtual Connection Subscribe(Animation OnFrameStep);

	virtual void Unsubscribe(int connectionId);
};
