#include "stdafx.h"
#include "MockAboutView.h"

MockAboutView::MockAboutView()
	: isShown (false)
{
}

void MockAboutView::ConnectClose(slot_type OnClose)
{
	SignalClose.connect(OnClose);
}

void MockAboutView::Hide()
{
	isShown = false;
}

void MockAboutView::Show()
{
	isShown = true;
}
