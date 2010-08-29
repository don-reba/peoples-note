#pragma once

#include <windows.h>

class Timestamp
{
private:
	
	time_t unixTime;

public:

	Timestamp();

	Timestamp(time_t unixTime);

	std::wstring GetFormattedDateTime() const;

	__int64 GetTime() const;

	static Timestamp GetCurrentTime();
};
