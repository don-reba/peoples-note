#include "stdafx.h"
#include "File.h"

using namespace std;

bool File::Read(const wstring & path, Blob & data)
{
	HANDLE file = ::CreateFile
		( path.c_str()          // lpFileName
		, GENERIC_READ          // dwDesiredAccess
		, FILE_SHARE_READ       // dwShareMode
		, NULL                  // lpSecurityAttributes
		, OPEN_EXISTING         // dwCreationDisposition
		, FILE_ATTRIBUTE_NORMAL // dwFlagsAndAttributes
		, NULL                  // hTemplateFile
		);
	if (INVALID_HANDLE_VALUE == file)
		return false;

	DWORD size(::GetFileSize(file, NULL));
	if (INVALID_FILE_SIZE == size)
	{
		::CloseHandle(file);
		return false;
	}

	data.resize(size);
	if (0 == size)
	{
		::CloseHandle(file);
		return true;
	}

	DWORD bytesRead(0);
	BOOL result(::ReadFile(file, &data[0], size, &bytesRead, NULL));

	::CloseHandle(file);

	if (!result || bytesRead != size)
	{
		data.clear();
		return false;
	}

	return true;
}

bool File::Write(const std::wstring & path, const Blob & data)
{
	// TODO
	return false;
}
