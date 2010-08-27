// Library for capturing profiling information with
// the /callcap compiler flag.

#ifdef DEBUG

#include <windows.h>

class Store
{ 
private:

	static const int bufferSize = 1024;

	const HANDLE file;
	unsigned __int32 * const buffer;

	int bufferPos;

public:

	Store()
		: buffer    (new unsigned __int32[bufferSize])
		, bufferPos (0)
		, file      (CreateLogFile())
	{
		if (!IsValidHandle(file))
		{
			DWORD error (::GetLastError());
			DWORD flags (FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS);
			const int bufferSize(100);
			wchar_t * buffer = new wchar_t[bufferSize];
			::FormatMessage
				( flags      // dwFlags
				, NULL       // lpSource
				, error      // dwMessageId
				, 0          // dwLanguageId
				, buffer     // pBuffer
				, bufferSize // nSize
				, NULL       // Arguments
				);
			DEBUGMSG(true, (L"%s", buffer));
			delete [] buffer;
		}
	}

	~Store()
	{
		Flush();
		::CloseHandle(file);
		delete [] buffer;
	}

	void Write(void * p, bool enter)
	{
		if (!IsValidHandle(file))
			return;

		// WARN: Not 64-bit or large address space-compatible.
		unsigned __int32 value(reinterpret_cast<unsigned __int32>(p));
		if (!enter)
			value |= 0x80000000;
		buffer[bufferPos++] = value;

		if (bufferPos < bufferSize)
			return;

		Flush();
		bufferPos = 0;
	}

private:

	void Flush()
	{
		::SetFilePointer(file, 0, NULL, FILE_BEGIN);
		::WriteFile(file, &buffer[0], bufferPos * 4, NULL, NULL);
	}

	static HANDLE CreateLogFile()
	{
		const wchar_t * const folderPath (L"\\My Documents\\People's Note");
		const wchar_t * const filePath  (L"\\My Documents\\People's Note\\calls.bin");
		::CreateDirectory(folderPath, NULL);
		return ::CreateFile
			( filePath              // lpFileName
			, GENERIC_WRITE         // dwDesiredAccess
			, FILE_SHARE_READ       // dwShareMode
			, NULL                  // lpSecurityAttributes
			, CREATE_ALWAYS         // dwCreationDisposition
			, FILE_ATTRIBUTE_NORMAL // dwFlagsAndAttributes
			, NULL                  // hTemplateFile
			);
	}

	static bool IsValidHandle(HANDLE handle)
	{
		return handle != NULL && handle != INVALID_HANDLE_VALUE;
	}
};

void Write(void * p, bool enter)
{
	static Store store;
	store.Write(p, enter);
}

extern "C"
{
	void _CAP_Enter_Function(void *p)
	{
		Write(p, true);	
	}

	void _CAP_Exit_Function(void *p) 
	{
		Write(p, false);
	}
}

#endif // DEBUG
