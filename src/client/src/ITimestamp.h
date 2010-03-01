#pragma once

class ITimestamp
{
public:

	virtual ~ITimestamp() {}

	virtual std::wstring GetFormattedDateTime() const = 0;
};
