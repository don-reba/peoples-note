#pragma once

class IAnimator
{
public:

	typedef boost::signals2::signal<void(DWORD)> Signal;
	typedef boost::signals2::connection Connection;

public:

	virtual void StepFrame() = 0;

	virtual bool IsRunning() = 0;

	virtual Connection Subscribe(Signal::slot_type OnFrameStep) = 0;

	virtual DWORD GetMilliseconds() = 0;
};
