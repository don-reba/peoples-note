#include "stdafx.h"
#include "MockProfileView.h"

MockProfileView::MockProfileView()
	: isShown (false)
{
}

void MockProfileView::ConnectClose(slot_type OnClose)
{
	SignalClose.connect(OnClose);
}

void MockProfileView::Hide()
{
	isShown = false;
}

void MockProfileView::Show()
{
	isShown = true;
}
