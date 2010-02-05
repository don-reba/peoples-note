#pragma once

#include "ITimestamp.h"

class MockTimestamp : public ITimestamp
{
public:

	std::wstring formattedDateTime;

public:

	virtual std::wstring GetFormattedDateTime() const;
};
