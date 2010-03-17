#pragma once

#include "IAnimator.h"

class Animator : public IAnimator
{
private:

	Signal SignalFrameStep;

// IAnimator implementation

public:

	virtual void StepFrame();

	virtual bool IsRunning();

	virtual Connection Subscribe(Signal::slot_type OnFrameStep);

	virtual DWORD GetMilliseconds();
};
