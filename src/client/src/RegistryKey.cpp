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
	( const wstring & value
	, const wstring & defaultData
	) const
{
	if (key == NULL)
		return defaultData;

	DWORD type(0);
	DWORD size(0);
	LONG result = ::RegQueryValueEx
		( key           // hKey
		, value.c_str() // lpValueName
		, 0             // lpReserved
		, &type         // lpType
		, NULL          // lpData
		, &size         // lpcbData
		);
	if (ERROR_SUCCESS != result)
		return defaultData;
	if (type != REG_SZ)
		return defaultData;
	if (size <= 0)
		return defaultData;

	vector<BYTE> data(size);
	result = ::RegQueryValueEx
		( key           // hKey
		, value.c_str() // lpValueName
		, 0             // lpReserved
		, &type         // lpType
		, &data[0]      // lpData
		, &size         // lpcbData
		);
	if (ERROR_SUCCESS != result)
		return defaultData;

	return reinterpret_cast<const wchar_t *>(&data[0]);
}

void RegistryKey::SetString
	( const wstring & value
	, const wstring & data
	)
{
	if (NULL == key)
		return;
	const BYTE * byteData (reinterpret_cast<const BYTE *>(data.c_str()));
	DWORD        dataSize (data.size() * 2 + 2);
	::RegSetValueEx
		( key           // hKey
		, value.c_str() // lpValuename
		, 0             // Reserved
		, REG_SZ        // dwType
		, byteData      // lpData
		, dataSize      // cbData
		);
}
