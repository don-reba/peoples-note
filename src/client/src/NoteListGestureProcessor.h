#pragma once

#include "ISignalProvider.h"
#include "window.h"

class IAnimator;

class NoteListGestureProcessor : public ISignalProvider
{
private:

	enum State
	{
		StateIdle,
		StateAnimating,
		StateDragging,
	};

	typedef boost::shared_ptr<WndMsg> WndMsgPtr;

private:

	const double acceleration; // px/ms
	const double sensitivity;  // mm

	IAnimator & animator;

	WndMsgPtr lButtonDown;
	POINT     clickPosition;
	int       scrollDistance;
	double    dragSpeed;
	int       startTime;
	State     state;

	signal SignalDelayedMouseDown;
	signal SignalGestureStart;
	signal SignalGestureStep;

public:

	NoteListGestureProcessor(IAnimator & animator);

	void ConnectDelayedMouseDown (slot_type OnDelayedMouseDown);
	void ConnectGestureStart     (slot_type OnGestureStart);
	void ConnectGestureStep      (slot_type OnGestureStep);

	WndMsg * GetMouseDownMessage();
	int      GetScrollDistance();

	void OnMouseDown (Msg<WM_LBUTTONDOWN> & msg);
	void OnMouseMove (Msg<WM_MOUSEMOVE>   & msg);
	void OnMouseUp   (Msg<WM_LBUTTONUP>   & msg);

private:

	void OnFrameStep(DWORD time);
};
