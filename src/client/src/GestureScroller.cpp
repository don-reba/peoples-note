#include "stdafx.h"
#include "GestureScroller.h"

using namespace htmlayout;

GestureScroller::GestureScroller()
	: event_handler(HANDLE_MOUSE)
{
}

BOOL GestureScroller::handle_mouse(HELEMENT he, MOUSE_PARAMS & params)
{
	return FALSE;
}
