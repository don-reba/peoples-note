#include "stdafx.h"

#include "DataStore.h"
#include "MockNotebook.h"
#include "Test.h"

bool FileExists(const wchar_t * path)
{
	return ::GetFileAttributes(path) != 0xFFFFFFFF;
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

	TEST_CHECK_EQUAL(store.GetVersion(),       1);
	TEST_CHECK_EQUAL(store.GetUser(),          name);
	TEST_CHECK_EQUAL(store.GetNotebookCount(), 1);
}

AUTO_TEST_CASE(TestDataStoreAddNotebook)
{
	const wchar_t * name   = L"test";
	const wchar_t * folder = L"Program Files\\MobileTest";
	const wchar_t * file   = L"Program Files\\MobileTest\\test.db";

	DataStore store(folder);
	::DeleteFile(file);
	store.LoadOrCreate(name);

	MockNotebook notebook;
	notebook.name = L"test-notebook";
	store.AddNotebook(notebook);

	TEST_CHECK_EQUAL(store.GetNotebookCount(), 2);

	store.AddNotebook(notebook);

	TEST_CHECK_EQUAL(store.GetNotebookCount(), 2);
}
