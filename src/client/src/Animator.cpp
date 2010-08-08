#include "stdafx.h"
#include "Animator.h"

using namespace boost;
using namespace std;

//----------
// interface
//----------

Animator::Animator()
	: lastFps (0)
	, lastId  (AnimationNone)
{
}

//-------------------------
// IAnimator implementation
//-------------------------

void Animator::ConnectAnimationCompleted(slot_type OnAnimationCompleted)
{
	SignalAnimationCompleted.connect(OnAnimationCompleted);
}

int Animator::GetLastAnimationId()
{
	return lastId;
}

double Animator::GetLastAnimationFps()
{
	return lastFps;
}

bool Animator::IsRunning()
{
	return !records.empty();
}

void Animator::StepFrame()
{
	DWORD time(::GetTickCount());

	list<Record>::iterator  i(records.begin());
	while (i != records.end())
	{
		Record & record(*i);
		++i;
		++record.frameCount;
		record.animation(time - record.startTime);
	}
}

void Animator::Subscribe(AnimationId id, Animation OnFrameStep)
{
	Record record;
	record.id         = id;
	record.animation  = OnFrameStep;
	record.startTime  = ::GetTickCount();
	record.frameCount = 0;

	records.push_back(record);
}

void Animator::Unsubscribe(AnimationId connectionId)
{
	typedef list<Record>::iterator Iterator;

	DWORD time(::GetTickCount());

	for (Iterator i(records.begin()); i != records.end(); ++i)
	{
		if (i->id != connectionId)
			continue;
		lastId  = connectionId;
		lastFps = 1000.0 * i->frameCount / (time - i->startTime);
		records.erase(i);
		SignalAnimationCompleted();
		return;
	}
}
