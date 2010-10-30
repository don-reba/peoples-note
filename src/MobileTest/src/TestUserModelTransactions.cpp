#include "stdafx.h"
#include "DataStore.h"
#include "MockFlashCard.h"
#include "Notebook.h"
#include "Test.h"
#include "Transaction.h"
#include "UserModel.h"

const wchar_t * const storeName   = L"test";
const wchar_t * const storeFolder = L"Program Files\\MobileTest";
const wchar_t * const storeFile   = L"Program Files\\MobileTest\\test.db";

DWORD WINAPI UserModelTransactionsThread(LPVOID)
{
	try
	{
		MockFlashCard flashCard;

		DataStore store;
		UserModel model(store, storeFolder, flashCard);

		model.LoadOrCreate(storeName);

		Transaction transaction(model);
		model.AddNotebook(Notebook());
	}
	catch (...)
	{
		return 1;
	}

	return 0;
}

AUTO_TEST_CASE(TestUserModelTransactions)
{
	MockFlashCard flashCard;

	DataStore store;
	UserModel model(store, storeFolder, flashCard);

	::DeleteFile(storeFile);
	model.LoadOrCreate(storeName);

	TEST_CHECK_EQUAL(model.GetNotebookCount(), 1);

	HANDLE thread(NULL);
	DWORD  threadId(-1);
	{
		Transaction transaction(model);

		model.AddNotebook(Notebook());
		
		Transaction nestedTransaction(model);

		thread = ::CreateThread(NULL, 0, &UserModelTransactionsThread, NULL, 0, &threadId);
		TEST_CHECK(thread != NULL);
		if (!thread)
			return;
		::Sleep(100);

		TEST_CHECK_EQUAL(model.GetNotebookCount(), 2);
	}

	DWORD waitResult(::WaitForSingleObject(thread, 1000));
	if (waitResult == WAIT_OBJECT_0)
		waitResult = threadId;
	TEST_CHECK_EQUAL(waitResult, threadId);

	DWORD exitCode(-1);
	::GetExitCodeThread(thread, &exitCode);
	TEST_CHECK_EQUAL(exitCode, 0);
	::CloseHandle(thread);

	TEST_CHECK_EQUAL(model.GetNotebookCount(), 3);
}
