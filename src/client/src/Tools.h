#pragma once

#include "Blob.h"
#include "HtmlResource.h"

namespace Tools
{
	std::string ConvertToAnsi(const std::wstring & str);

	const unsigned char * ConvertToUtf8
		( const std::wstring         & str
		, std::vector<unsigned char> & result
		);

	std::wstring ConvertToUnicode(const std::string & str);

	std::wstring ConvertToUnicode(const unsigned char * str);

	void DecodeBase64(const wchar_t * text, Blob & data);

#ifdef _DEBUG
	std::wstring GetMessageName(int id);
#endif // _DEBUG

	std::string HashWithMD5(const Blob & data);

	HtmlResource LoadHtmlResource(LPCWSTR id);

	std::wstring LoadStringResource(int id);

	void ReplaceAll
		(       std::wstring & str
		, const std::wstring & target
		, const std::wstring & replacement
		);

	bool StartsWith(const wchar_t * text, const wchar_t * prefix);

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
