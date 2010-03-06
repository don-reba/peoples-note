#include "stdafx.h"
#include "MockTimestamp.h"

using namespace std;

MockTimestamp::MockTimestamp()
{
}

MockTimestamp::MockTimestamp(const ITimestamp & timestamp)
{
	formattedDateTime = L"";
	time              = timestamp.GetTime();
}

MockTimestamp::MockTimestamp(wstring formattedDateTime, __int64 time)
	: formattedDateTime (formattedDateTime)
	, time              (time)
{
}

wstring MockTimestamp::GetFormattedDateTime() const
{
	return formattedDateTime;
}

__int64 MockTimestamp::GetTime() const
{
	return time;
}
