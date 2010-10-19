#include "stdafx.h"
#include "RegistryKey.h"

using namespace std;

RegistryKey::RegistryKey(const wstring & path)
	: key (NULL)
{
	DWORD disposition(0);
	LONG result = ::RegCreateKeyEx
		( HKEY_CURRENT_USER       // hKey
		, path.c_str()            // lpSubKey
		, 0                       // Reserved
		, NULL                    // lpClass
		, REG_OPTION_NON_VOLATILE // dwOptions
		, 0                       // samDesired
		, NULL                    // lpSecurityAttributes
		, &key                    // phkResult
		, &disposition            // lpdwDisposition
		);
	// this is not a critical component, so handle failure gracefully
	if (result != ERROR_SUCCESS)
		key = NULL;
}

RegistryKey::~RegistryKey()
{
	if (key != NULL)
		::RegCloseKey(key);
}

wstring RegistryKey::GetString
	( const wchar_t * value
	, const wchar_t * defaultData
	) const
{
	if (key == NULL)
		return defaultData;

	DWORD type(0);
	DWORD size(0);
	LONG result = ::RegQueryValueEx
		( key   // hKey
		, value // lpValueName
		, 0     // lpReserved
		, &type // lpType
		, NULL  // lpData
		, &size // lpcbData
		);
	if (ERROR_SUCCESS != result)
		return defaultData;
	if (type != REG_SZ)
		return defaultData;
	if (size <= 0)
		return defaultData;

	vector<BYTE> data(size);
	result = ::RegQueryValueEx
		( key      // hKey
		, value    // lpValueName
		, 0        // lpReserved
		, &type    // lpType
		, &data[0] // lpData
		, &size    // lpcbData
		);
	if (ERROR_SUCCESS != result)
		return defaultData;

	return reinterpret_cast<const wchar_t *>(&data[0]);
}

void RegistryKey::SetString
	( const wchar_t * value
	, const wchar_t * data
	)
{
	if (NULL == key)
		return;
	const BYTE * byteData (reinterpret_cast<const BYTE *>(data));
	DWORD        dataSize (wcslen(data) * 2 + 2);
	::RegSetValueEx
		( key      // hKey
		, value    // lpValuename
		, 0        // Reserved
		, REG_SZ   // dwType
		, byteData // lpData
		, dataSize // cbData
		);
}
