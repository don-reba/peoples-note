#include "stdafx.h"
#include "Timer.h"


Timer::Timer()
{
	::QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&s));
}

void Timer::Stop()
{
	::QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&f));
}

float Timer::GetElapsedSeconds() const
{
	__int64 frequency;
	::QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&frequency));
	return static_cast<float>(f - s) / frequency;
}
