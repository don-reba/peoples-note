#include "stdafx.h"
#include "MessagePump.h"

MessagePump::MessagePump()
	: thread (::GetCurrentThreadId())
{
}

void MessagePump::WakeUp()
{
	::PostThreadMessage(thread, WM_NULL, 0, NULL);
}
