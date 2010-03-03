#include "stdafx.h"

#include "DataStore.h"
#include "MockNote.h"
#include "MockNotebook.h"
#include "Test.h"

using namespace boost;
using namespace std;

//----------------------
// auxillary definitions
//----------------------

const wchar_t * const storeName   = L"test";
const wchar_t * const storeFolder = L"Program Files\\MobileTest";
const wchar_t * const storeFile   = L"Program Files\\MobileTest\\test.db";

struct DataStoreFixture
{

	DataStore store;

	DataStoreFixture()
		: store (storeFolder)
	{
		::DeleteFile(storeFile);
		store.LoadOrCreate(storeName);
	}
};

bool FileExists(const wchar_t * path)
{
	return ::GetFileAttributes(path) != 0xFFFFFFFF;
}

//-----------
// test suite
//-----------

FIXTURE_TEST_CASE(TestDataStoreAddNote, DataStoreFixture)
{
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

FIXTURE_TEST_CASE(TestDataStoreAddNotebook, DataStoreFixture)
{
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

FIXTURE_TEST_CASE(TestDataStoreDefaultNotebook, DataStoreFixture)
{
	MockNotebook notebook(L"test-notebook");
	store.AddNotebook(notebook);
	store.MakeNotebookDefault(notebook);

	TEST_CHECK_EQUAL(store.GetDefaultNotebook().GetName(), L"test-notebook");
}

FIXTURE_TEST_CASE(TestDataStoreLastUsedNotebook, DataStoreFixture)
{
	vector<MockNotebook> notebooks;
	notebooks.push_back(MockNotebook(L"notebook0"));
	notebooks.push_back(MockNotebook(L"notebook1"));
	notebooks.push_back(MockNotebook(L"notebook2"));
	foreach (MockNotebook & notebook, notebooks)
		store.AddNotebook(notebook);
	store.MakeNotebookLastUsed(notebooks.at(1));

	TEST_CHECK_EQUAL(store.GetLastUsedNotebook().GetName(), L"notebook1");
}

AUTO_TEST_CASE(TestDataStoreLoadOrCreate)
{
	{
		DataStore store(storeFolder);

		::DeleteFile(storeFile);
		TEST_CHECK(!FileExists(storeFile));
		store.LoadOrCreate(storeName);
		TEST_CHECK(FileExists(storeFile));

		TEST_CHECK_EQUAL(store.GetVersion(),       0);
		TEST_CHECK_EQUAL(store.GetUser(),          storeName);
		TEST_CHECK_EQUAL(store.GetNotebookCount(), 0);

		store.AddNotebook(MockNotebook());
	}
	{
		DataStore store(storeFolder);
		store.LoadOrCreate(storeName);

		TEST_CHECK_EQUAL(store.GetVersion(),       0);
		TEST_CHECK_EQUAL(store.GetUser(),          storeName);
		TEST_CHECK_EQUAL(store.GetNotebookCount(), 1);
	}
}

FIXTURE_TEST_CASE(TestDataStoreNotebooks, DataStoreFixture)
{
	store.AddNotebook(MockNotebook(L"notebook1"));
	store.AddNotebook(MockNotebook(L"notebook0"));
	store.AddNotebook(MockNotebook(L"notebook2"));

	ptr_vector<INotebook> & notebooks = store.GetNotebooks();
	TEST_CHECK_EQUAL(notebooks.size(), 3);
	TEST_CHECK_EQUAL(notebooks.at(0).GetName(), L"notebook0");
	TEST_CHECK_EQUAL(notebooks.at(1).GetName(), L"notebook1");
	TEST_CHECK_EQUAL(notebooks.at(2).GetName(), L"notebook2");
}

FIXTURE_TEST_CASE(TestDataStoreNotesByNotebook, DataStoreFixture)
{
	vector<MockNotebook> notebooks;
	notebooks.push_back(MockNotebook(L"notebook-0"));
	notebooks.push_back(MockNotebook(L"notebook-1"));
	foreach (const INotebook & notebook, notebooks)
		store.AddNotebook(notebook);

	store.AddNote
		( MockNote(Guid(), L"note-0", MockTimestamp(L"", 0))
		, notebooks.at(0)
		);
	store.AddNote
		( MockNote(Guid(), L"note-1", MockTimestamp(L"", 1))
		, notebooks.at(1)
		);
	store.AddNote
		( MockNote(Guid(), L"note-2", MockTimestamp(L"", 2))
		, notebooks.at(0)
		);

	ptr_vector<INote> & notes = store.GetNotesByNotebook(notebooks.at(0));
	TEST_CHECK_EQUAL(notes.size(), 2);
	TEST_CHECK_EQUAL(notes.at(0).GetTitle(), L"note-0");
	TEST_CHECK_EQUAL(notes.at(1).GetTitle(), L"note-2");
}

FIXTURE_TEST_CASE(TestDataStoreNotesBySearch, DataStoreFixture)
{
	MockNotebook notebook(L"notebook");
	store.AddNotebook(notebook);

	store.AddNote
		( MockNote(Guid(), L"useful software", MockTimestamp(L"", 0))
		, notebook
		);
	store.AddNote
		( MockNote(Guid(), L"software use", MockTimestamp(L"", 1))
		, notebook
		);

	ptr_vector<INote> & notes0 = store.GetNotesBySearch(L"software");
	TEST_CHECK_EQUAL(notes0.size(), 2);
	TEST_CHECK_EQUAL(notes0.at(0).GetTitle(), L"useful software");
	TEST_CHECK_EQUAL(notes0.at(1).GetTitle(), L"software use");

	ptr_vector<INote> & notes1 = store.GetNotesBySearch(L"use");
	TEST_CHECK_EQUAL(notes1.size(), 1);
	TEST_CHECK_EQUAL(notes1.at(0).GetTitle(), L"software use");
}
