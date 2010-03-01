#include "stdafx.h"

#include "DataStore.h"
#include "MockNotebook.h"
#include "Test.h"

using namespace boost;
using namespace std;

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

	TEST_CHECK_EQUAL(store.GetVersion(),       0);
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

	try
	{
		store.AddNotebook(notebook);
	}
	catch (const std::exception & e)
	{
		TEST_CHECK_EQUAL(string(e.what()), "column name is not unique");
	}

	TEST_CHECK_EQUAL(store.GetNotebookCount(), 2);
}

AUTO_TEST_CASE(TestDataStoreDefaultNotebook)
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

	TEST_CHECK_EQUAL(store.GetDefaultNotebook().GetName(), L"Notes");

	store.MakeNotebookDefault(notebook);

	TEST_CHECK_EQUAL(store.GetDefaultNotebook().GetName(), L"test-notebook");
}

AUTO_TEST_CASE(TestDataStoreLastUsedNotebook)
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

	TEST_CHECK_EQUAL(store.GetLastUsedNotebook().GetName(), L"Notes");
}

AUTO_TEST_CASE(TestDataStoreNotebooks)
{
	const wchar_t * name   = L"test";
	const wchar_t * folder = L"Program Files\\MobileTest";
	const wchar_t * file   = L"Program Files\\MobileTest\\test.db";

	DataStore store(folder);
	::DeleteFile(file);
	store.LoadOrCreate(name);

	MockNotebook notebook;
	notebook.name = L"Test";
	store.AddNotebook(notebook);

	ptr_vector<INotebook> & notebooks = store.GetNotebooks();
	TEST_CHECK_EQUAL(notebooks.size(), 2);
	TEST_CHECK_EQUAL(notebooks.at(0).GetName(), L"Notes");
	TEST_CHECK_EQUAL(notebooks.at(1).GetName(), L"Test");
}
