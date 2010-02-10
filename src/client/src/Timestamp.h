#pragma once

#include "ITimestamp.h"

#include <windows.h>

class Timestamp : public ITimestamp
{
private:
	
	SYSTEMTIME time;

public:

	virtual std::wstring GetFormattedDateTime() const;
};
