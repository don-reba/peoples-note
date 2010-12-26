#include "stdafx.h"
#include "NoteViewGestureProcessor.h"

#include "IAnimator.h"

using namespace boost;

NoteViewGestureProcessor::NoteViewGestureProcessor
	( IAnimator & animator
	)
	: acceleration (-0.001)
	, animator     (animator)
	, sensitivity  (2.0)
{}

void NoteViewGestureProcessor::ConnectDelayedMouseDown(slot_type OnDelayedMouseDown)
{
	SignalDelayedMouseDown.connect(OnDelayedMouseDown);
}

void NoteViewGestureProcessor::ConnectGestureStart(slot_type OnGestureStart)
{
	SignalGestureStart.connect(OnGestureStart);
}

void NoteViewGestureProcessor::ConnectGestureStep(slot_type OnGestureStep)
{
	SignalGestureStep.connect(OnGestureStep);
}

WndMsg * NoteViewGestureProcessor::GetMouseDownMessage()
{
	return &*lButtonDown;
}

SIZE NoteViewGestureProcessor::GetScrollDistance()
{
	return scrollDistance;
}

void NoteViewGestureProcessor::OnMouseDown(Msg<WM_LBUTTONDOWN> & msg)
{
	SignalGestureStart();

	if (state == StateAnimating)
		animator.Unsubscribe(IAnimator::AnimationNoteListScroll);

	state = StateDragging;
	startTime = ::GetTickCount();

	lButtonDown  = make_shared<WndMsg>(WM_LBUTTONDOWN, msg.lprm_, msg.wprm_);
	clickPosition = msg.Position();
}

void NoteViewGestureProcessor::OnMouseMove(Msg<WM_MOUSEMOVE> & msg)
{
	if (state == StateDragging)
	{
		scrollDistance.cx = clickPosition.x - msg.Position().x;
		scrollDistance.cy = clickPosition.y - msg.Position().y;
		SignalGestureStep();
	}
}

void NoteViewGestureProcessor::OnMouseUp(Msg<WM_LBUTTONUP> & msg)
{
	if (state == StateDragging)
	{
		HDC hdc(::GetDC(NULL));
		int dpi(::GetDeviceCaps(hdc, LOGPIXELSX)); // px/in
		::ReleaseDC(NULL, hdc);

		double threshold(sensitivity * 0.03937 * dpi); // 0.03937 in/mm

		int dx(msg.Position().x - clickPosition.x);
		int dy(msg.Position().y - clickPosition.y);
		if (threshold * threshold < dx * dx + dy * dy)
		{
			SignalGestureStart();

			double dt(::GetTickCount() - startTime);

			state      = StateAnimating;
			dragSpeedX = dx / dt;
			dragSpeedY = dy / dt;
			animator.Subscribe
				( IAnimator::AnimationNoteListScroll
				, bind(&NoteViewGestureProcessor::OnFrameStep, this, _1)
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

void NoteViewGestureProcessor::OnFrameStep(DWORD time)
{
	double sx(dragSpeedX);
	double sy(dragSpeedY);

	const double s(sqrt(sx * sx + sy * sy));
	sx /= s;
	sy /= s;

	const int    t(time);
	const double a(acceleration);
	if (s + a * t > 0.001)
	{
		double d(t * (s + 0.5 * a * t));
		scrollDistance.cx = static_cast<int>(-sx * d);
		scrollDistance.cy = static_cast<int>(-sy * d);
		SignalGestureStep();
	}
	else
	{
		state = StateIdle;
		animator.Unsubscribe(IAnimator::AnimationNoteListScroll);
	}
}
