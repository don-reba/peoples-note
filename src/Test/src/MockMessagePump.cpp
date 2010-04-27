#include "stdafx.h"
#include "MockMessagePump.h"

MockMessagePump::MockMessagePump()
	: wokeUp (false)
{
}

void MockMessagePump::WakeUp()
{
	wokeUp = true;
}