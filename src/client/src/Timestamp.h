#pragma once

#include "ITimestamp.h"

#include <windows.h>

class Timestamp : public ITimestamp
{
private:
	
	time_t unixTime;

public:

	Timestamp();

	Timestamp(time_t unixTime);

	virtual std::wstring GetFormattedDateTime() const;
};
