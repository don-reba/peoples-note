#include "stdafx.h"
#include "ImagingException.h"

#include "Tools.h"

using namespace std;
using namespace Tools;

ImagingException::ImagingException(HRESULT result)
	: error  (::GetLastError())
	, result (result)
{
	DWORD   flags  (FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_FROM_HMODULE|FORMAT_MESSAGE_IGNORE_INSERTS);
	HMODULE source (GetModuleHandle(L"imaging.dll"));
	vector<wchar_t> buffer(100);
	::FormatMessage
		( flags         // dwFlags
		, source        // lpSource
		, error         // dwMessageId
		, 0             // dwLanguageId
		, &buffer[0]    // pBuffer
		, buffer.size() // nSize
		, NULL          // Arguments
		);
	message = ConvertToAnsi(&buffer[0]);
}

const char * ImagingException::what() const
{
	return message.c_str();
}
