#include "stdafx.h"
#include "RegistryKey.h"

using namespace std;

wstring RegistryKey::GetString
	( wstring value
	, wstring defautlData
	) const
{
	// TODO: implement
	if (value == L"username")
		return L"test-usr";
	if (value == L"password")
		return L"test-pwd";
	return L"test";
}
