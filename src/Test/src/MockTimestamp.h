#pragma once

#include "ITimestamp.h"

class MockTimestamp : public ITimestamp
{
public:

	std::wstring formattedDateTime;
	__int64      time;

public:

	MockTimestamp();

	MockTimestamp(const ITimestamp & timestamp);

	MockTimestamp(std::wstring formattedDateTime, __int64 time);

	virtual std::wstring GetFormattedDateTime() const;

	virtual __int64 GetTime() const;
};
