#include "stdafx.h"

#include "Tools.h"

#include <algorithm>

using namespace Tools;
using namespace std;


//-----------------
// Helper functions
//-----------------

// Based on Raymond Chen's implementation.
// http://blogs.msdn.com/oldnewthing/archive/2004/01/30/65013.aspx
LPCWSTR GetStringResource(HINSTANCE instance, WORD id)
{
	// Convert the string ID into a bundle number
	LPWSTR resourceName = MAKEINTRESOURCE(id / 16 + 1);

	HRSRC hrsrc = ::FindResource(instance, resourceName, RT_STRING);
	if (!hrsrc)
		throw exception("Resource not found.");

	HGLOBAL hglob = ::LoadResource(instance, hrsrc);
	if (!hglob)
		throw exception("Resource could not be loaded.");

	LPCWSTR resource = reinterpret_cast<LPCWSTR>(::LockResource(hglob));
	if (!resource)
		throw exception("Resource could not be locked.");

	// walk the string table
	for (int i = 0; i < (id & 0xF); i++)
		resource += 1 + (UINT)*resource;

	return resource;
}

bool IsBase64Whitespace(wchar_t c)
{
	switch (c)
	{
	case L' ': case L'\n': case L'\r': case L'\t':
		return true;
	}
	return false;
}

#ifndef RT_HTML
#define RT_HTML MAKEINTRESOURCE(23)
#endif // RT_HTML

//---------------------
// Tools implementation
//---------------------

string Tools::ConvertToAnsi(const wstring str)
{
	UINT  codePage = CP_ACP;
	DWORD flags    = 0;
	int resultSize = WideCharToMultiByte
		( codePage     // CodePage
		, flags        // dwFlags
		, str.c_str()  // lpWideCharStr
		, str.length() // cchWideChar
		, NULL         // lpMultiByteStr
		, 0            // cbMultiByte
		, NULL         // lpDefaultChar
		, NULL         // lpUsedDefaultChar
		);
	vector<char> result(resultSize + 1);
	WideCharToMultiByte
		( codePage     // CodePage
		, flags        // dwFlags
		, str.c_str()  // lpWideCharStr
		, str.length() // cchWideChar
		, &result[0]   // lpMultiByteStr
		, resultSize   // cbMultiByte
		, NULL         // lpDefaultChar
		, NULL         // lpUsedDefaultChar
		);
	return &result[0];
}

const unsigned char * Tools::ConvertToUtf8
	( const wstring         & str
	, vector<unsigned char> & result
	)
{
	UINT  codePage = CP_UTF8;
	DWORD flags    = 0;
	int resultSize = WideCharToMultiByte
		( codePage     // CodePage
		, flags        // dwFlags
		, str.c_str()  // lpWideCharStr
		, str.length() // cchWideChar
		, NULL         // lpMultiByteStr
		, 0            // cbMultiByte
		, NULL         // lpDefaultChar
		, NULL         // lpUsedDefaultChar
		);
	result.resize(0);
	result.resize(resultSize + 1);
	LPSTR resultStr = reinterpret_cast<char*>(&result[0]);
	WideCharToMultiByte
		( codePage     // CodePage
		, flags        // dwFlags
		, str.c_str()  // lpWideCharStr
		, str.length() // cchWideChar
		, resultStr    // lpMultiByteStr
		, resultSize   // cbMultiByte
		, NULL         // lpDefaultChar
		, NULL         // lpUsedDefaultChar
		);
	result.resize(result.size() - 1); // cut terminating zero
	return result.empty()
		? reinterpret_cast<unsigned char *>("")
		: &result[0]
		;
}

wstring Tools::ConvertToUnicode(const string str)
{
	UINT  codePage = CP_ACP;
	DWORD flags    = 0;
	int resultSize = MultiByteToWideChar
		( codePage     // CodePage
		, flags        // dwFlags
		, str.c_str()  // lpMultiByteStr
		, str.length() // cbMultiByte
		, NULL         // lpWideCharStr
		, 0            // cchWideChar
		);
	vector<wchar_t> result(resultSize + 1);
	MultiByteToWideChar
		( codePage     // CodePage
		, flags        // dwFlags
		, str.c_str()  // lpMultiByteStr
		, str.length() // cbMultiByte
		, &result[0]   // lpWideCharStr
		, resultSize   // cchWideChar
		);
	return &result[0];
}

std::wstring Tools::ConvertToUnicode(const unsigned char * str)
{
	LPCSTR cStr = reinterpret_cast<LPCSTR>(str);
	int length = strlen(cStr);

	UINT  codePage = CP_UTF8;
	DWORD flags    = 0;
	int resultSize = MultiByteToWideChar
		( codePage // CodePage
		, flags    // dwFlags
		, cStr     // lpMultiByteStr
		, length   // cbMultiByte
		, NULL     // lpWideCharStr
		, 0        // cchWideChar
		);
	vector<wchar_t> result(resultSize + 1);
	MultiByteToWideChar
		( codePage   // CodePage
		, flags      // dwFlags
		, cStr       // lpMultiByteStr
		, length     // cbMultiByte
		, &result[0] // lpWideCharStr
		, resultSize // cchWideChar
		);
	return &result[0];
}

void Tools::DecodeBase64(const wchar_t * text, Blob & data)
{
	if (!text)
		throw std::exception("Invalid input.");

	// validate and measure input
	int size(0);
	for (const wchar_t * t(text); *t;)
	{
		if (IsBase64Whitespace(*t))
			++t;
		else
		{
			for (int i(0); i != 4; ++i)
			{
				if (!*t || *t < 43 || *t > 43 + 90)
					throw std::exception("Invalid input.");
				++size;
				++t;
			}
		}
	}

	// calculate output size
	if (size == 0)
		return;
	if (size % 4 != 0)
		throw std::exception("Invalid input.");
	size = size / 4 * 3;

	// parse input
	const BYTE lookup[] =
		{ 62, 0 , 0 , 0 , 63, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 0
		, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 1 , 2 , 3 , 4 , 5 , 6 , 7 , 8 , 9
		, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25
		, 0 , 0 , 0 , 0 , 0 , 0 , 26, 27, 28, 29, 30, 31, 32, 33, 34, 35
		, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51
		};
	data.clear();
	data.reserve(size);
	while (*text)
	{
		if (IsBase64Whitespace(*text))
			++text;
		else
		{
			BYTE n0 = lookup[*text - 43]; ++text;
			BYTE n1 = lookup[*text - 43]; ++text;
			BYTE n2 = lookup[*text - 43]; ++text;
			BYTE n3 = lookup[*text - 43]; ++text;
			data.push_back((n0 << 2) | (n1 >> 4));
			data.push_back((n1 << 4) | (n2 >> 2));
			data.push_back((n2 << 6) | (n3 >> 0));
		}
	}

	// remove padding
	if (text[-1] == L'=')
		--size;
	if (text[-2] == L'=')
		--size;
	data.resize(size);
}

HtmlResource Tools::LoadHtmlResource(LPCWSTR id)
{
	HINSTANCE instance = GetModuleHandle(NULL);

	HRSRC hrsrc = ::FindResource(instance, id, RT_HTML);
	if(!hrsrc)
		throw exception("Resource not found.");

	HGLOBAL hglob = ::LoadResource(instance, hrsrc);
	if(!hglob)
		throw exception("Resource could not be loaded.");

	HtmlResource resource;

	resource.data = static_cast<PBYTE>(::LockResource(hglob));
	if (!resource.data)
		throw exception("Resource could not be locked.");

	resource.size = ::SizeofResource(instance, hrsrc);
	if (0 == resource.size)
		throw exception("Resource size could not be found");

	return resource;
}

wstring Tools::LoadStringResource(int id)
{
	HINSTANCE instance = GetModuleHandle(NULL);
	LPCWSTR resource = GetStringResource(instance, id);
	vector<wchar_t> str(1 + *resource);
	CopyMemory(&str[0], resource + 1, *resource * sizeof(wchar_t));
	return &str[0];
}

void Tools::UnixTimeToFileTime
	(     time_t     unixTime
	, OUT FILETIME & fileTime
	)
{
	// Note that LONGLONG is a 64-bit value
	LONGLONG temp;
	temp = Int32x32To64(unixTime, 10000000) + 116444736000000000;
	fileTime.dwLowDateTime  = static_cast<DWORD>(temp);
	fileTime.dwHighDateTime = temp >> 32;
}

void Tools::UnixTimeToSystemTime
	(     time_t       unixTime
	, OUT SYSTEMTIME & systemTime
	)
{
	FILETIME fileTime;
	UnixTimeToFileTime(unixTime, fileTime);
	::FileTimeToSystemTime(&fileTime, &systemTime);
}

time_t Tools::FileTimeToUnixTime
	( const FILETIME & fileTime
	)
{
	LONGLONG temp;
	temp = fileTime.dwHighDateTime;
	temp = (temp << 32) | fileTime.dwLowDateTime;
	return static_cast<time_t>(temp / 10000000 - 11644473600);
}

time_t Tools::SystemTimeToUnixTime
	( const SYSTEMTIME & systemTime
	)
{
	FILETIME fileTime;
	::SystemTimeToFileTime(&systemTime, &fileTime);
	return FileTimeToUnixTime(fileTime);
}
