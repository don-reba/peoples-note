#include "stdafx.h"
#include "ScopedLock.h"
#include "Test.h"

struct Context
{
	 int              marker;
	 CRITICAL_SECTION section;
};

DWORD WINAPI ScopedLockThread(void * param)
{
	Context * context(reinterpret_cast<Context*>(param));

	ScopedLock lock(context->section);

	context->marker = 1;

	return 0;
}

AUTO_TEST_CASE(TestScopedLock)
{
	Context context;

	context.marker = 0;
	::InitializeCriticalSection(&context.section);

	HANDLE otherThread(::CreateThread(NULL, 0, &ScopedLockThread, &context, CREATE_SUSPENDED, NULL));

	{
		ScopedLock lock(context.section);
		::ResumeThread(otherThread);
		::Sleep(10);
		TEST_CHECK_EQUAL(context.marker, 0);
	}

	::Sleep(10);
	TEST_CHECK_EQUAL(context.marker, 1);
}