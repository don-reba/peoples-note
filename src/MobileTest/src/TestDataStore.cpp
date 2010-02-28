#include "stdafx.h"

#include "DataStore.h"
#include "Test.h"

bool FileExists(const wchar_t * path)
{
	return GetFileAttributes(path) != 0xFFFFFFFF;
}

AUTO_TEST_CASE(TestDataStoreLoad)
{
	const wchar_t * name   = L"test";
	const wchar_t * folder = L"Program Files\\MobileTest";
	const wchar_t * file   = L"Program Files\\MobileTest\\test.db";

	DataStore store(folder);
	::DeleteFile(file);
	TEST_CHECK(!FileExists(file));
	store.LoadOrCreate(name);
	TEST_CHECK(FileExists(file));
}
