#pragma once

#include "DataStore.h"

bool FileExists(const wchar_t * path)
{
	return GetFileAttributes(path) != 0xFFFFFFFF;
}

void TestDataStoreLoad(Test & test)
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
