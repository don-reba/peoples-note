#include "stdafx.h"
#include "TException.h"

#include <vector>

using namespace std;
using namespace Thrift;

TException::TException()
	: ansi    ( "Default TException.")
	, message (L"Default TException.")
{
}

TException::TException(const wchar_t * message)
	: ansi    (UnicodeToAnsi(message))
	, message (message)
{
}

TException::~TException()
{
}

const wchar_t * TException::GetMessage() const
{
	return message.c_str();
}

const char * TException::what() const throw()
{
	return ansi.c_str();
}

string TException::UnicodeToAnsi(const wchar_t * str)
{
	UINT  codePage = CP_ACP;
	DWORD flags    = 0;
	int strLen     = wcslen(str);
	int resultSize = WideCharToMultiByte
		( codePage // CodePage
		, flags    // dwFlags
		, str      // lpWideCharStr
		, strLen   // cchWideChar
		, NULL     // lpMultiByteStr
		, 0        // cbMultiByte
		, NULL     // lpDefaultChar
		, NULL     // lpUsedDefaultChar
		);
	vector<char> result(resultSize + 1);
	WideCharToMultiByte
		( codePage   // CodePage
		, flags      // dwFlags
		, str        // lpWideCharStr
		, strLen     // cchWideChar
		, &result[0] // lpMultiByteStr
		, resultSize // cbMultiByte
		, NULL       // lpDefaultChar
		, NULL       // lpUsedDefaultChar
		);
	return &result[0];
}
