#include "stdafx.h"
#include "WinException.h"

#include "Tools.h"

#include <vector>

using namespace std;
using namespace Tools;

WinException::WinException(DWORD error, const wchar_t * source)
{
		DWORD flags
			( FORMAT_MESSAGE_FROM_SYSTEM
			| FORMAT_MESSAGE_FROM_HMODULE
			| FORMAT_MESSAGE_IGNORE_INSERTS
			);
		vector<wchar_t> buffer(256);
		::FormatMessage
			( flags                   // dwFlags
			, GetModuleHandle(source) // lpSource
			, error                   // dwMessageId
			, 0                       // dwLanguageId
			, &buffer[0]              // pBuffer
			, buffer.size()           // nSize
			, NULL                    // Arguments
			);
		message = ConvertToAnsi(&buffer[0]);
}

const char * WinException::what() const
{
	return message.c_str();
}
