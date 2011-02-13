#pragma once

#include "IAnimator.h"

#include <list>

class Animator : public IAnimator
{
// events

	MacroEvent(AnimationCompleted)

// types

private:

	struct Record
	{
		AnimationId id;
		Animation   animation;
		DWORD       startTime;
		int         frameCount;
	};

// data

private:

	std::list<Record> records;
	AnimationId       lastId;
	double            lastFps;

// interface

public:

	Animator();

// IAnimator implementation

public:

	virtual int GetLastAnimationId();

	virtual double GetLastAnimationFps();

	virtual bool IsRunning();

	virtual void StepFrame();

	virtual void Subscribe(AnimationId id, Animation OnFrameStep);

	virtual void Unsubscribe(AnimationId id);
};
