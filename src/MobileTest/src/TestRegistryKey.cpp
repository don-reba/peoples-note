#include "stdafx.h"

#include "RegistryKey.h"
#include "Test.h"

using namespace std;

void DeleteRegistryKey(const wchar_t * path)
{
	::RegDeleteKey(HKEY_CURRENT_USER, path);
}

AUTO_TEST_CASE(TestRegistryKey)
{
	const wchar_t * key   (L"Software\\test-key");
	const wchar_t * value (L"test-value");
	const wchar_t * data  (L"test-data");
	const wchar_t * fail  (L"test-fail");
	{
		DeleteRegistryKey(key);
		RegistryKey key(key);
		key.SetString(value, data);
	}
	{
		RegistryKey key    (key);
		wstring     result (key.GetString(value, fail));
		TEST_CHECK_EQUAL(result, data);
	}
}