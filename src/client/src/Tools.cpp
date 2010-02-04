#include "stdafx.h"

#include "Tools.h"

#include <algorithm>
#include <vector>

using namespace Tools;
using namespace std;


//-----------------
// Helper functions
//-----------------

// Based on Raymond Chen's implementation.
// http://blogs.msdn.com/oldnewthing/archive/2004/01/30/65013.aspx
LPCWSTR GetStringResource(HINSTANCE instance, UINT id)
{
	// Convert the string ID into a bundle number
	LPWSTR resourceName = MAKEINTRESOURCE(id / 16 + 1);
	HRSRC hrsrc = FindResource(instance, resourceName, RT_STRING);
	if (!hrsrc)
		throw std::exception("Resource not found.");
	HGLOBAL hglob = LoadResource(instance, hrsrc);
	if (!hglob)
		throw std::exception("Resource could not be loaded.");
	LPCWSTR resource = reinterpret_cast<LPCWSTR>(LockResource(hglob));
	if (!resource)
		throw std::exception("Resource could not be locked.");
	// okay now walk the string table
	for (UINT i = 0; i < (id & 0xF); i++)
		resource += 1 + (UINT)*resource;
	return resource;
}

//---------------------
// Tools implementation
//---------------------

std::wstring Tools::ConvertToUnicode(std::string str)
{
	std::vector<wchar_t> result
		( MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), NULL, 0) + 1
		);
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), &result[0], result.size());
	return &result[0];
}

wstring Tools::LoadStringResource(HINSTANCE instance, UINT id)
{
	LPCWSTR resource = GetStringResource(instance, id);
	vector<wchar_t> str(1 + *resource);
	CopyMemory(&str[0], resource + 1, *resource * sizeof(wchar_t));
	return &str[0];
}
