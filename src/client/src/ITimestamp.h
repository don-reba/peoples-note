#pragma once

class ITimestamp
{
public:

	virtual ~ITimestamp() {}

	virtual std::wstring GetFormattedDateTime() const = 0;

	virtual __int64 GetTime() const = 0;
};
