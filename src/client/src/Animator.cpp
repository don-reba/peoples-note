#include "stdafx.h"
#include "Animator.h"

//-------------------------
// IAnimator implementation
//-------------------------

void Animator::StepFrame()
{
	SignalFrameStep(::GetTickCount());
}

bool Animator::IsRunning()
{
	return SignalFrameStep.num_slots() > 0;
}

IAnimator::Connection Animator::Subscribe(Signal::slot_type OnFrameStep)
{
	return SignalFrameStep.connect(OnFrameStep);
}

DWORD Animator::GetMilliseconds()
{
	return ::GetTickCount();
}
