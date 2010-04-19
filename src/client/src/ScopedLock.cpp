#include "stdafx.h"
#include "ScopedLock.h"

ScopedLock::ScopedLock(CRITICAL_SECTION & criticalSection)
	: criticalSection (criticalSection)
{
	::EnterCriticalSection(&criticalSection);
}

ScopedLock::~ScopedLock()
{
	::LeaveCriticalSection(&criticalSection);
}