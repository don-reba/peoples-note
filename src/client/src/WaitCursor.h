#pragma once

class WaitCursor
{
private:

	HCURSOR cursor;

public:

	WaitCursor();

	~WaitCursor();
};

#define MacroWaitCursor \
	WaitCursor waitCursor