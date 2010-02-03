#include "stdafx.h"

#include "Tools.h"

#include <algorithm>

using namespace Tools;
using namespace std;

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

wstring Tools::LoadStringResource(HINSTANCE instance, UINT id)
{
	LPCWSTR resource = GetStringResource(instance, id);
	wstring str;
	str.reserve(1 + (resource ? *resource : 0));
	copy
		( resource + 1
		, resource + 1 + str.length()
		, std::back_insert_iterator<wstring>(str)
		);
	return str;
}