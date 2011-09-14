#include "stdafx.h"
#include "WaitCursor.h"

WaitCursor::WaitCursor()
	: cursor(::SetCursor(::LoadCursor(NULL, IDC_WAIT)))
{
}

WaitCursor::~WaitCursor()
{
	::SetCursor(cursor);
}