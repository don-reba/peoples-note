#pragma once

#include "IAnimator.h"

#include <list>

class Animator : public IAnimator
{
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

	signal SignalAnimationCompleted;

// interface

public:

	Animator();

// IAnimator implementation

public:

	virtual void ConnectAnimationCompleted(slot_type OnAnimationCompleted);

	virtual int GetLastAnimationId();

	virtual double GetLastAnimationFps();

	virtual bool IsRunning();

	virtual void StepFrame();

	virtual void Subscribe(AnimationId id, Animation OnFrameStep);

	virtual void Unsubscribe(AnimationId id);
};
