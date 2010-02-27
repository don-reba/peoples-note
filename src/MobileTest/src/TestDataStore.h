#pragma once

#include "DataStore.h"

void TestDataStoreLoad(Test & test)
{
	DataStore store(L"Program Files\\MobileTest");

	::DeleteFile(L"Program Files\\MobileTest\\test.db");
	
	TEST_CHECK_EQUAL
		( GetFileAttributes(L"Program Files\\MobileTest\\test.db")
		, 0xFFFFFFFF
		);

	store.LoadOrCreate(L"test");

	TEST_CHECK_NOT_EQUAL
		( ::GetFileAttributes(L"Program Files\\MobileTest\\test.db")
		, 0xFFFFFFFF
		);
}
