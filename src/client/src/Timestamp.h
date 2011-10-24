#pragma once

#include <windows.h>

class Timestamp
{
private:
	
	time_t unixTime;

public:

	Timestamp();

	Timestamp(time_t unixTime);

	bool IsValid() const;

	std::wstring GetFormattedDateTime() const;

	__int64 GetTime() const;

	bool operator < (const Timestamp & rhs) const;

	static Timestamp GetCurrentTime();
};
