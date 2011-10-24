#pragma once

#include "Blob.h"
#include "HtmlResource.h"

namespace Tools
{
	//----------
	// templates
	//----------

	template<typename T, typename I>
	bool StartsWith(const T & sequence, I begin, I end)
	{
		if (sequence.size() < static_cast<size_t>(end - begin))
			return false;
		T::const_iterator i = sequence.begin();
		while (begin != end)
		{
			if (*i != *begin)
				return false;
			++i; ++begin;
		}
		return true;
	}

	template<typename T>
	bool StartsWith(const T & sequence, const T & fragment)
	{
		return StartsWith(sequence, fragment.begin(), fragment.end());
	}

	template<typename T, typename I>
	bool EndsWith(const T & sequence, I begin, I end)
	{
		if (sequence.size() < static_cast<size_t>(end - begin))
			return false;
		T::const_iterator i = sequence.end() - (end - begin);
		while (begin != end)
		{
			if (*i != *begin)
				return false;
			++i; ++begin;
		}
		return true;
	}

	template<typename T>
	bool EndsWith(const T & sequence, const T & fragment)
	{
		return EndsWith(sequence, fragment.begin(), fragment.end());
	}

	template<class T, std::size_t N>
	std::size_t GetArraySize(const T(&)[N]) throw() 
	{
		return N;
	}

	//-------------
	// declarations
	//-------------
 
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

	bool EndsWith(const std::wstring & sequence, const wchar_t * fragment);

	std::wstring EscapeHtml(const std::wstring & str);

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

	bool StartsWith(const std::wstring & sequence, const wchar_t * fragment);

	void UnixTimeToFileTime
		( time_t         unixTime
		, OUT FILETIME & fileTime
		);

	void UnixTimeToLocalSystemTime
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
