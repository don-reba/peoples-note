#pragma once

#include "Blob.h"
#include "HtmlResource.h"

namespace Tools
{
	template<class T, std::size_t N>
	std::size_t GetArraySize(const T(&)[N]) throw() 
	{
		return N;
	}
 
	std::string ConvertToAnsi(const std::wstring & str);

	const unsigned char * ConvertToUtf8
		( const std::wstring         & str
		, std::vector<unsigned char> & result
		);

	std::wstring ConvertToUnicode(const std::string & str);

	void ConvertToUnicode
		( const unsigned char * str
		, std::wstring        & result
		);

	void DecodeBase64(const wchar_t * text, Blob & data);

#ifdef _DEBUG
	std::wstring GetMessageName(int id);
#endif // _DEBUG

	std::string HashWithMD5(const Blob & data);

	std::wstring HashPassword(const std::wstring & password);

	HtmlResource LoadHtmlResource(LPCWSTR id, bool highRes);

	std::wstring LoadStringResource(int id);

	std::wstring MakeSizeString(__int64 size);

	void ReadBinaryFile
		( const wchar_t * fileName
		, Blob          & data
		);

	void ReadUtf8File
		( const wchar_t * fileName
		, std::wstring  & contents
		);

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
