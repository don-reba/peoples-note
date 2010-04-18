#include "stdafx.h"
#include "DataStore.h"
#include "Notebook.h"
#include "Test.h"
#include "Transaction.h"
#include "UserModel.h"

const wchar_t * const storeName   = L"test";
const wchar_t * const storeFolder = L"Program Files\\MobileTest";
const wchar_t * const storeFile   = L"Program Files\\MobileTest\\test.db";

DWORD WINAPI OtherThread(LPVOID)
{
	try
	{
		DataStore store;
		UserModel model(store, storeFolder);

		model.LoadOrCreate(storeName);

		Transaction transaction(model);
		model.AddNotebook(Notebook(Guid(), L"notebook-1"));
	}
	catch (...)
	{
		// if we get here, the test won't pass
	}

	return 0;
}

AUTO_TEST_CASE(TestUserModelTransactions)
{
	DataStore store;
	UserModel model(store, storeFolder);

	::DeleteFile(storeFile);
	model.LoadOrCreate(storeName);

	TEST_CHECK_EQUAL(model.GetNotebookCount(), 1);

	{
		Transaction transaction(model);

		model.AddNotebook(Notebook(Guid(), L"notebook-0"));

		HANDLE otherThread(::CreateThread(NULL, 0, &OtherThread, NULL, 0, NULL));
		TEST_CHECK(otherThread != 0);
		::Sleep(100);
		TEST_CHECK_EQUAL(model.GetNotebookCount(), 2);
	}

	::Sleep(100);
	TEST_CHECK_EQUAL(model.GetNotebookCount(), 3);
}
