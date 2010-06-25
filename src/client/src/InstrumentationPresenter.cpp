#include "stdafx.h"
#include "InstrumentationPresenter.h"

#include "IAnimator.h"
#include "INoteListView.h"

#include <sstream>

using namespace boost;
using namespace std;

InstrumentationPresenter::InstrumentationPresenter
	( IAnimator     & animator
	, INoteListView & noteListView
	)
	: animator     (animator)
	, noteListView (noteListView)
{
	animator.ConnectAnimationCompleted(bind
		( &InstrumentationPresenter::OnAnimationCompleted
		, this
		));
}

void InstrumentationPresenter::OnAnimationCompleted()
{
	wostringstream stream;	
	switch (animator.GetLastAnimationId())
	{
	case IAnimator::AnimationNone:
		return;
	case IAnimator::AnimationNoteListScroll:
		stream << L"NoteListScroll";
	}
	stream << L": " << animator.GetLastAnimationFps() << L" fps";
	noteListView.SetStatusText(stream.str());
}
