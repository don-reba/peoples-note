#include "stdafx.h"

#include "DataStore.h"
#include "MockNote.h"
#include "MockNotebook.h"
#include "Test.h"

using namespace boost;
using namespace std;

bool FileExists(const wchar_t * path)
{
	return ::GetFileAttributes(path) != 0xFFFFFFFF;
}

AUTO_TEST_CASE(TestDataStoreAddNote)
{
	const wchar_t * name   = L"test";
	const wchar_t * folder = L"Program Files\\MobileTest";
	const wchar_t * file   = L"Program Files\\MobileTest\\test.db";

	DataStore store(folder);
	::DeleteFile(file);
	store.LoadOrCreate(name);

	MockNotebook notebook(L"test-notebook");
	store.AddNotebook(notebook);

	store.AddNote
		( MockNote(Guid(), L"note-0", MockTimestamp())
		, notebook
		);

	MockNotebook fakeNotebook(L"fake-notebook");

	try
	{
		store.AddNote
			( MockNote(Guid(), L"note-1", MockTimestamp())
			, fakeNotebook
			);
	}
	catch (const std::exception & e)
	{
		TEST_CHECK_EQUAL(string(e.what()), "foreign key constraint failed");
	}
}

AUTO_TEST_CASE(TestDataStoreAddNotebook)
{
	const wchar_t * name   = L"test";
	const wchar_t * folder = L"Program Files\\MobileTest";
	const wchar_t * file   = L"Program Files\\MobileTest\\test.db";

	DataStore store(folder);
	::DeleteFile(file);
	store.LoadOrCreate(name);

	MockNotebook notebook(L"test-notebook");
	store.AddNotebook(notebook);

	TEST_CHECK_EQUAL(store.GetNotebookCount(), 1);

	try
	{
		store.AddNotebook(notebook);
	}
	catch (const std::exception & e)
	{
		TEST_CHECK_EQUAL(string(e.what()), "column guid is not unique");
	}

	TEST_CHECK_EQUAL(store.GetNotebookCount(), 1);
}

AUTO_TEST_CASE(TestDataStoreDefaultNotebook)
{
	const wchar_t * name   = L"test";
	const wchar_t * folder = L"Program Files\\MobileTest";
	const wchar_t * file   = L"Program Files\\MobileTest\\test.db";

	DataStore store(folder);
	::DeleteFile(file);
	store.LoadOrCreate(name);

	MockNotebook notebook(L"test-notebook");
	store.AddNotebook(notebook);
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

	vector<MockNotebook> notebooks;
	notebooks.push_back(MockNotebook(L"notebook0"));
	notebooks.push_back(MockNotebook(L"notebook1"));
	notebooks.push_back(MockNotebook(L"notebook2"));
	foreach (MockNotebook & notebook, notebooks)
		store.AddNotebook(notebook);
	store.MakeNotebookLastUsed(notebooks.at(1));

	TEST_CHECK_EQUAL(store.GetLastUsedNotebook().GetName(), L"notebook1");
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
	TEST_CHECK_EQUAL(store.GetNotebookCount(), 0);
}

AUTO_TEST_CASE(TestDataStoreNotebooks)
{
	const wchar_t * name   = L"test";
	const wchar_t * folder = L"Program Files\\MobileTest";
	const wchar_t * file   = L"Program Files\\MobileTest\\test.db";

	DataStore store(folder);
	::DeleteFile(file);
	store.LoadOrCreate(name);

	vector<MockNotebook> mockNotebooks;
	mockNotebooks.push_back(MockNotebook(L"notebook1"));
	mockNotebooks.push_back(MockNotebook(L"notebook0"));
	mockNotebooks.push_back(MockNotebook(L"notebook2"));
	foreach (MockNotebook & notebook, mockNotebooks)
		store.AddNotebook(notebook);

	ptr_vector<INotebook> & notebooks = store.GetNotebooks();
	TEST_CHECK_EQUAL(notebooks.size(), 3);
	TEST_CHECK_EQUAL(notebooks.at(0).GetName(), L"notebook0");
	TEST_CHECK_EQUAL(notebooks.at(1).GetName(), L"notebook1");
	TEST_CHECK_EQUAL(notebooks.at(2).GetName(), L"notebook2");
}
