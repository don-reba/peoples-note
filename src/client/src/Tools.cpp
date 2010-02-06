#include "stdafx.h"

#include "Tools.h"

#include <algorithm>
#include <windows.h>

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
		throw std::exception("Resource not found.");

	HGLOBAL hglob = ::LoadResource(instance, hrsrc);
	if (!hglob)
		throw std::exception("Resource could not be loaded.");

	LPCWSTR resource = reinterpret_cast<LPCWSTR>(::LockResource(hglob));
	if (!resource)
		throw std::exception("Resource could not be locked.");

	// walk the string table
	for (int i = 0; i < (id & 0xF); i++)
		resource += 1 + (UINT)*resource;

	return resource;
}

//---------------------
// Tools implementation
//---------------------

std::string Tools::ConvertToAnsi(std::wstring str)
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

std::wstring Tools::ConvertToUnicode(std::string str)
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

HtmlResource Tools::LoadHtmlResource(int id)
{
	const LPCWSTR RT_HTML = MAKEINTRESOURCE(23);
	HINSTANCE instance = GetModuleHandle(NULL);

	HRSRC hrsrc = ::FindResource(instance, MAKEINTRESOURCE(id), RT_HTML);
	if(!hrsrc)
		throw std::exception("Resource not found.");

	HGLOBAL hglob = ::LoadResource(instance, hrsrc);
	if(!hglob)
		throw std::exception("Resource could not be loaded.");

	HtmlResource resource;

	resource.data = static_cast<PBYTE>(::LockResource(hglob));
	if (!resource.data)
		throw std::exception("Resource could not be locked.");

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
