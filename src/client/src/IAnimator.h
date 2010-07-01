#pragma once

#include "ISignalProvider.h"

#include <boost/function.hpp>

class IAnimator : public ISignalProvider
{
public:

	enum AnimationId
	{
		AnimationNone = -1,
		AnimationNoteListScroll,
		AnimationTest0,
		AnimationTest1,
	};

	typedef boost::function<void(DWORD)> Animation;

public:

	virtual void ConnectAnimationCompleted(slot_type OnAnimationCompleted) = 0;

	virtual int GetLastAnimationId() = 0;

	virtual double GetLastAnimationFps() = 0;

	virtual bool IsRunning() = 0;

	virtual void StepFrame() = 0;

	virtual void Subscribe(AnimationId id, Animation OnFrameStep) = 0;

	virtual void Unsubscribe(AnimationId id) = 0;
};
