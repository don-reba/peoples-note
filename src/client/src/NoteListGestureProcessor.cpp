#include "stdafx.h"
#include "NoteListGestureProcessor.h"

#include "IAnimator.h"

using namespace boost;

NoteListGestureProcessor::NoteListGestureProcessor
	( IAnimator & animator
	)
	: acceleration (-0.001)
	, animator     (animator)
	, sensitivity  (2.0)
{}

void NoteListGestureProcessor::ConnectDelayedMouseDown(slot_type OnDelayedMouseDown)
{
	SignalDelayedMouseDown.connect(OnDelayedMouseDown);
}

void NoteListGestureProcessor::ConnectGestureStart(slot_type OnGestureStart)
{
	SignalGestureStart.connect(OnGestureStart);
}

void NoteListGestureProcessor::ConnectGestureStep(slot_type OnGestureStep)
{
	SignalGestureStep.connect(OnGestureStep);
}

WndMsg * NoteListGestureProcessor::GetMouseDownMessage()
{
	return &*lButtonDown;
}

int NoteListGestureProcessor::GetScrollDistance()
{
	return scrollDistance;
}

void NoteListGestureProcessor::OnMouseDown(Msg<WM_LBUTTONDOWN> & msg)
{
	SignalGestureStart();

	if (state == StateAnimating)
		animator.Unsubscribe(IAnimator::AnimationNoteListScroll);

	state = StateDragging;
	startTime = ::GetTickCount();

	lButtonDown  = make_shared<WndMsg>(WM_LBUTTONDOWN, msg.lprm_, msg.wprm_);
	clickPosition = msg.Position();
}

void NoteListGestureProcessor::OnMouseMove(Msg<WM_MOUSEMOVE> & msg)
{
	if (state == StateDragging)
	{
		scrollDistance = clickPosition.y - msg.Position().y;
		SignalGestureStep();
	}
}

void NoteListGestureProcessor::OnMouseUp(Msg<WM_LBUTTONUP> & msg)
{
	if (state == StateDragging)
	{
		HDC hdc(::GetDC(NULL));
		int dpi(::GetDeviceCaps(hdc, LOGPIXELSX)); // px/in
		::ReleaseDC(NULL, hdc);

		double threshold(sensitivity * 0.03937 * dpi); // 0.03937 in/mm

		int dx = msg.Position().x - clickPosition.x;
		int dy = msg.Position().y - clickPosition.y;
		if (threshold * threshold < dx * dx + dy * dy)
		{
			SignalGestureStart();

			state     = StateAnimating;
			dragSpeed = dy / static_cast<double>(::GetTickCount() - startTime);
			animator.Subscribe
				( IAnimator::AnimationNoteListScroll
				, bind(&NoteListGestureProcessor::OnFrameStep, this, _1)
				);
		}
		else
		{
			state = StateIdle;
			SignalDelayedMouseDown();
			lButtonDown.reset();
		}
	}
}

void NoteListGestureProcessor::OnFrameStep(DWORD time)
{
	const int    t   (time);
	const int    sgn ((dragSpeed >= 0.0) ? 1 : -1);
	const double s   (fabs(dragSpeed));
	const double a   (acceleration);
	if (s + a * t > 0.001)
	{
		scrollDistance = -sgn * static_cast<int>(t * (s + 0.5 * a * t));
		SignalGestureStep();
	}
	else
	{
		state = StateIdle;
		animator.Unsubscribe(IAnimator::AnimationNoteListScroll);
	}
}
