#include "stdafx.h"
#include "Animator.h"

using namespace boost;
using namespace std;

//----------
// interface
//----------

Animator::Animator()
	: lastId (0)
{
}

//-------------------------
// IAnimator implementation
//-------------------------

void Animator::StepFrame()
{
	DWORD time(::GetTickCount());

	list<Record>::iterator  i(records.begin());
	while (i != records.end())
	{
		Record & record(*i);
		++i;
		record.animation(time - record.startTime);
	}
}

bool Animator::IsRunning()
{
	return !records.empty();
}

IAnimator::Connection Animator::Subscribe(Animation OnFrameStep)
{
	Record record;
	record.id        = lastId;
	record.animation = OnFrameStep;
	record.startTime = ::GetTickCount();

	Connection connection(this, lastId);

	++lastId;

	records.push_back(record);
	return connection;
}

void Animator::Unsubscribe(int connectionId)
{
	typedef list<Record>::iterator Iterator;

	for (Iterator i(records.begin()); i != records.end(); ++i)
	{
		if (i->id != connectionId)
			continue;
		records.erase(i);
		return;
	}
}
