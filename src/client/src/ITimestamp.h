#pragma once

class ITimestamp
{
public:

	virtual std::wstring GetFormattedDateTime() const = 0;
};
