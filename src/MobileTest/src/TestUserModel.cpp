#include "stdafx.h"

#include "DataStore.h"
#include "MockCredentialsModel.h"
#include "Note.h"
#include "Notebook.h"
#include "Test.h"
#include "UserModel.h"

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
	UserModel userModel;

	DataStoreFixture()
		: userModel (store, storeFolder)
	{
		::DeleteFile(storeFile);
		userModel.SetCredentials(MockCredentialsModel(storeName, L""));
		userModel.Load();
	}
};

bool FileExists(const wchar_t * path)
{
	return ::GetFileAttributes(path) != 0xFFFFFFFF;
}

//-----------
// test suite
//-----------

FIXTURE_TEST_CASE(TestUserModelAddNote, DataStoreFixture)
{
	Note note        (Guid(), L"note-0", Timestamp(0));
	wstring body     (L"<html>note body</html>");
	wstring bodyText (L"");
	const Notebook & notebook = userModel.GetLastUsedNotebook();
	userModel.AddNote(note, body, bodyText, notebook);

	wstring loaded;
	userModel.GetNoteBody(note.GetGuid(), loaded);
	TEST_CHECK_EQUAL(loaded, body);
}

FIXTURE_TEST_CASE(TestUserModelNoteForeignKey, DataStoreFixture)
{
	Notebook notebook(Guid(), L"test-notebook");
	userModel.AddNotebook(notebook);

	wstring empty;
	userModel.AddNote
		( Note(Guid(), L"note-0", Timestamp(0))
		, empty
		, empty
		, notebook
		);

	Notebook fakeNotebook(Guid(), L"fake-notebook");

	TEST_CHECK_EXCEPTION
		( userModel.AddNote
			( Note(Guid(), L"note-1", Timestamp(1))
			, empty
			, empty
			, fakeNotebook
			)
		, std::exception
		, MESSAGE_EQUALS("foreign key constraint failed")
		);
}

FIXTURE_TEST_CASE(TestUserModelAddNotebook, DataStoreFixture)
{
	Notebook notebook(Guid(), L"test-notebook");
	userModel.AddNotebook(notebook);

	TEST_CHECK_EQUAL(userModel.GetNotebookCount(), 2);

	TEST_CHECK_EXCEPTION
		( userModel.AddNotebook(notebook)
		, std::exception
		, MESSAGE_EQUALS("column guid is not unique")
		);

	TEST_CHECK_EQUAL(userModel.GetNotebookCount(), 2);
}

FIXTURE_TEST_CASE(TestUserModelDefaultNotebook, DataStoreFixture)
{
	Notebook notebook(Guid(), L"test-notebook");
	userModel.AddNotebook(notebook);
	userModel.MakeNotebookDefault(notebook);

	TEST_CHECK_EQUAL(userModel.GetDefaultNotebook().GetName(), L"test-notebook");
}

FIXTURE_TEST_CASE(TestUserModelImageResource0, DataStoreFixture)
{
	Note note(Guid(), L"note-0", Timestamp(0));

	Blob blob;
	blob.push_back(2);
	blob.push_back(3);
	blob.push_back(5);
	blob.push_back(7);

	TEST_CHECK_EXCEPTION
		( userModel.AddImageResource("hash", blob, note.GetGuid())
		, std::exception
		, MESSAGE_EQUALS("foreign key constraint failed")
		);
}

FIXTURE_TEST_CASE(TestUserModelImageResource1, DataStoreFixture)
{
	std::string hash ("hash");
	Note        note (Guid(), L"note-0", Timestamp(0));

	Blob blob;
	blob.push_back(2);
	blob.push_back(3);
	blob.push_back(5);
	blob.push_back(7);

	userModel.AddNote(note, L"", L"", userModel.GetLastUsedNotebook());
	userModel.AddImageResource(hash, blob, note.GetGuid());

	Blob loaded;
	userModel.GetImageResource(hash, loaded);
	TEST_CHECK_EQUAL(blob.size(), loaded.size());
	for (int i = 0; i != blob.size(); ++i)
		TEST_CHECK_EQUAL(blob.at(i), loaded.at(i));
}

FIXTURE_TEST_CASE(TestUserModelLastUsedNotebook, DataStoreFixture)
{
	vector<Notebook> notebooks;
	notebooks.push_back(Notebook(Guid(), L"notebook0"));
	notebooks.push_back(Notebook(Guid(), L"notebook1"));
	notebooks.push_back(Notebook(Guid(), L"notebook2"));
	foreach (Notebook & notebook, notebooks)
		userModel.AddNotebook(notebook);
	userModel.MakeNotebookLastUsed(notebooks.at(1));

	TEST_CHECK_EQUAL(userModel.GetLastUsedNotebook().GetName(), L"notebook1");
}

AUTO_TEST_CASE(TestUserModelLoadOrCreate)
{
	{
		DataStore store;
		UserModel userModel(store, storeFolder);
		userModel.SetCredentials(MockCredentialsModel(storeName, L""));

		::DeleteFile(storeFile);
		TEST_CHECK(!FileExists(storeFile));
		userModel.Load();
		TEST_CHECK(FileExists(storeFile));

		TEST_CHECK_EQUAL(userModel.GetVersion(),       0);
		TEST_CHECK_EQUAL(userModel.GetUser(),          storeName);
		TEST_CHECK_EQUAL(userModel.GetNotebookCount(), 1);

		userModel.AddNotebook(Notebook(Guid(), L""));
	}
	{
		DataStore store;
		UserModel userModel(store, storeFolder);
		userModel.SetCredentials(MockCredentialsModel(storeName, L""));

		userModel.Load();

		TEST_CHECK_EQUAL(userModel.GetVersion(),       0);
		TEST_CHECK_EQUAL(userModel.GetUser(),          storeName);
		TEST_CHECK_EQUAL(userModel.GetNotebookCount(), 2);
	}
}

FIXTURE_TEST_CASE(TestUserModelNotebooks, DataStoreFixture)
{
	userModel.AddNotebook(Notebook(Guid(), L"notebook1"));
	userModel.AddNotebook(Notebook(Guid(), L"notebook0"));
	userModel.AddNotebook(Notebook(Guid(), L"notebook2"));

	const NotebookList & notebooks = userModel.GetNotebooks();
	TEST_CHECK_EQUAL(notebooks.size(), 4);
	TEST_CHECK_EQUAL(notebooks.at(0).GetName(), L"Notes");
	TEST_CHECK_EQUAL(notebooks.at(1).GetName(), L"notebook0");
	TEST_CHECK_EQUAL(notebooks.at(2).GetName(), L"notebook1");
	TEST_CHECK_EQUAL(notebooks.at(3).GetName(), L"notebook2");
}

FIXTURE_TEST_CASE(TestUserModelNotesByNotebook, DataStoreFixture)
{
	vector<Notebook> notebooks;
	notebooks.push_back(Notebook(Guid(), L"notebook-0"));
	notebooks.push_back(Notebook(Guid(), L"notebook-1"));
	foreach (const Notebook & notebook, notebooks)
		userModel.AddNotebook(notebook);

	wstring empty;

	userModel.AddNote
		( Note(Guid(), L"note-0", Timestamp(0))
		, empty
		, empty
		, notebooks.at(0)
		);
	userModel.AddNote
		( Note(Guid(), L"note-1", Timestamp(1))
		, empty
		, empty
		, notebooks.at(1)
		);
	userModel.AddNote
		( Note(Guid(), L"note-2", Timestamp(2))
		, empty
		, empty
		, notebooks.at(0)
		);

	const NoteList & notes = userModel.GetNotesByNotebook(notebooks.at(0));
	TEST_CHECK_EQUAL(notes.size(), 2);
	TEST_CHECK_EQUAL(notes.at(0).GetTitle(), L"note-0");
	TEST_CHECK_EQUAL(notes.at(1).GetTitle(), L"note-2");
}

FIXTURE_TEST_CASE(TestUserModelNotesBySearch, DataStoreFixture)
{
	Notebook notebook(Guid(), L"notebook");
	userModel.AddNotebook(notebook);

	wstring empty;

	userModel.AddNote
		( Note(Guid(), L"useful software", Timestamp(0))
		, empty
		, empty
		, notebook
		);
	userModel.AddNote
		( Note(Guid(), L"software use", Timestamp(1))
		, empty
		, empty
		, notebook
		);

	const NoteList & notes0 = userModel.GetNotesBySearch(L"software");
	TEST_CHECK_EQUAL(notes0.size(), 2);
	TEST_CHECK_EQUAL(notes0.at(0).GetTitle(), L"useful software");
	TEST_CHECK_EQUAL(notes0.at(1).GetTitle(), L"software use");

	const NoteList & notes1 = userModel.GetNotesBySearch(L"use");
	TEST_CHECK_EQUAL(notes1.size(), 1);
	TEST_CHECK_EQUAL(notes1.at(0).GetTitle(), L"software use");
}
