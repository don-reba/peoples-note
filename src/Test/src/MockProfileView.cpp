#include "stdafx.h"
#include "MockProfileView.h"

using namespace std;

MockProfileView::MockProfileView()
	: isMoveButtonEnabled (true)
	, isShown             (false)
{
}

void MockProfileView::DisableMoveButton()
{
	isMoveButtonEnabled = false;
}

void MockProfileView::EnableMoveButton()
{
	isMoveButtonEnabled = true;
}

void MockProfileView::Hide()
{
	isShown = false;
}

bool MockProfileView::IsShown()
{
	return isShown;
}

void MockProfileView::SetDbPath(const wstring & path)
{
	dbPath = path;
}

void MockProfileView::SetDbSize(const wstring & size)
{
	dbSize = size;
}

void MockProfileView::SetMoveButtonText(const wstring & text)
{
	moveButtonText = text;
}

void MockProfileView::SetMoveErrorMessage(const wstring & message)
{
	moveErrorMessage = message;
}

void MockProfileView::SetUsername(const wstring & username)
{
	this->username = username;
}

void MockProfileView::Show()
{
	isShown = true;
}
