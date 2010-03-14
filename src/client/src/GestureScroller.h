#pragma once

#include "htmlayout.h"

class GestureScroller : public htmlayout::event_handler
{
public:

	GestureScroller();

private:

	virtual BOOL handle_mouse(HELEMENT he, MOUSE_PARAMS & params);
};
