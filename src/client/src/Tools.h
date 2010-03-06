#pragma once

#include "HtmlResource.h"

#include <windows.h>

namespace Tools
{
	std::string ConvertToAnsi(const std::wstring str);

	std::vector<unsigned char> ConvertToUtf8(const std::wstring str);

	std::wstring ConvertToUnicode(const std::string str);

	std::wstring ConvertToUnicode(const unsigned char * str);

	HtmlResource LoadHtmlResource(int id);

	std::wstring LoadStringResource(int id);

	void UnixTimeToFileTime
		( time_t         unixTime
		, OUT FILETIME & fileTime
		);

	void UnixTimeToSystemTime
		( time_t           unixTime
		, OUT SYSTEMTIME & systemTime
		);

	time_t FileTimeToUnixTime
		( const FILETIME & fileTime
		);

	time_t SystemTimeToUnixTime
		( const SYSTEMTIME & systemTime
		);
}
