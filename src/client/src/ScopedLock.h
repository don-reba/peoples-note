#pragma once

class ScopedLock
{
private:

	CRITICAL_SECTION & criticalSection;

public:

	ScopedLock(CRITICAL_SECTION & criticalSection);

	~ScopedLock();
};
