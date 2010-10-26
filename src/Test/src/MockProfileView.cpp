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

void MockProfileView::ConnectMoveToCard(slot_type OnMoveToCard)
{
	SignalMoveToCard.connect(OnMoveToCard);
}

void MockProfileView::ConnectMoveToDevice(slot_type OnMoveToDevice)
{
	SignalMoveToDevice.connect(OnMoveToDevice);
}

void MockProfileView::Hide()
{
	isShown = false;
}

void MockProfileView::SetDbPath(const wchar_t * path)
{
	dbPath = path;
}

void MockProfileView::SetDbSize(const wchar_t * size)
{
	dbSize = size;
}

void MockProfileView::SetMoveButtonState(MoveButtonState state)
{
	moveButtonState = state;
}

void MockProfileView::SetMoveErrorMessage(const wchar_t * message)
{
	moveErrorMessage = message;
}

void MockProfileView::SetUsername(const wchar_t * username)
{
	this->username = username;
}

void MockProfileView::Show()
{
	isShown = true;
}
