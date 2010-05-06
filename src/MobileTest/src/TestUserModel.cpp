#include "stdafx.h"
#include "DataStore.h"
#include "Note.h"
#include "Notebook.h"
#include "Test.h"
#include "UserModel.h"

#include <boost/ref.hpp>
#include <fstream>

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
		userModel.LoadOrCreate(storeName);
	}

	~DataStoreFixture()
	{
		::DeleteFile(storeFile);
	}
};

bool FileExists(const wchar_t * path)
{
	return ::GetFileAttributes(path) != 0xFFFFFFFF;
}

struct SignalCheck
{
	bool signalled;
	SignalCheck() : signalled(false) {}
	void operator () () { signalled = true; }
};

Note MakeNote(const wchar_t * name)
{
	Note note;
	note.name = name;
	return note;
}

Note MakeNote(const wchar_t * name, int timestamp)
{
	Note note;
	note.name = name;
	note.creationDate = timestamp;
	return note;
}

Note MakeNote(const Guid & guid, const wchar_t * name = L"")
{
	Note note;
	note.guid = guid;
	note.name = name;
	return note;
}

Note MakeNote
	( const Guid    & guid
	, const wchar_t * name
	, Timestamp       creationDate
	, int             usn
	, bool            isDirty
	)
{
	Note note;
	note.guid         = guid;
	note.name         = name;
	note.creationDate = creationDate;
	note.usn          = usn;
	note.isDirty      = isDirty;
	return note;
}

//-----------
// test suite
//-----------

FIXTURE_TEST_CASE(TestUserModelAddNote, DataStoreFixture)
{
	Guid guid0;
	Guid guid1;
	{
		Note note        (MakeNote(guid0, L"note-0", Timestamp(2), 3, true));
		wstring body     (L"<html>note body 0</html>");
		wstring bodyText (L"note body 0");
		Notebook notebook;
		userModel.GetLastUsedNotebook(notebook);
		userModel.AddNote(note, body, bodyText, notebook);
	}
	{
		Note note        (MakeNote(guid1, L"note-1", Timestamp(5), 7, false));
		wstring body     (L"<html>note body 1</html>");
		wstring bodyText (L"note body 1");
		Notebook notebook;
		userModel.GetLastUsedNotebook(notebook);
		userModel.AddNote(note, body, bodyText, notebook);
	}
	{
		Note result = userModel.GetNote(guid0);
		TEST_CHECK_EQUAL(result.guid,                   guid0);
		TEST_CHECK_EQUAL(result.creationDate.GetTime(), 2L);
		TEST_CHECK_EQUAL(result.usn,                    3);
		TEST_CHECK_EQUAL(result.name,                   L"note-0");
		TEST_CHECK_EQUAL(result.isDirty,                true);

		wstring loaded;
		userModel.GetNoteBody(guid0, loaded);
		TEST_CHECK_EQUAL(loaded, L"<html>note body 0</html>");
	}
	{
		Note result = userModel.GetNote(guid1);
		TEST_CHECK_EQUAL(result.guid,                   guid1);
		TEST_CHECK_EQUAL(result.creationDate.GetTime(), 5L);
		TEST_CHECK_EQUAL(result.usn,                    7);
		TEST_CHECK_EQUAL(result.name,                   L"note-1");
		TEST_CHECK_EQUAL(result.isDirty,                false);

		wstring loaded;
		userModel.GetNoteBody(guid1, loaded);
		TEST_CHECK_EQUAL(loaded, L"<html>note body 1</html>");
	}
}

AUTO_TEST_CASE(TestUserModelExists)
{
	DataStore store;
	UserModel userModel(store, storeFolder);

	::DeleteFile(storeFile);
	TEST_CHECK(!userModel.Exists(storeName));

	{ ofstream(storeFile) << 0; }
	TEST_CHECK(userModel.Exists(storeName));
}

FIXTURE_TEST_CASE(TestUserModelNoteForeignKey, DataStoreFixture)
{
	Notebook notebook;
	notebook.name = L"test-notebook";
	userModel.AddNotebook(notebook);

	wstring empty;
	userModel.AddNote
		( MakeNote(L"note-0")
		, empty
		, empty
		, notebook
		);

	Notebook fakeNotebook;
	fakeNotebook.name = L"fake-notebook";

	TEST_CHECK_EXCEPTION
		( userModel.AddNote
			( MakeNote(L"note-1")
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
	Notebook notebook;
	notebook.name = L"test-notebook";
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
	Notebook notebook;
	notebook.name = L"test-notebook";
	userModel.AddNotebook(notebook);
	userModel.MakeNotebookDefault(notebook);

	Notebook defaultNotebook;
	userModel.GetDefaultNotebook(defaultNotebook);
	TEST_CHECK_EQUAL(defaultNotebook.name, L"test-notebook");
}

FIXTURE_TEST_CASE(TestUserModelImageResource0, DataStoreFixture)
{
	Note note(MakeNote(L"note-0"));

	Resource resource;
	resource.Data.push_back(2);
	resource.Data.push_back(3);
	resource.Data.push_back(5);
	resource.Data.push_back(7);
	resource.Hash = "hash";
	resource.Note = note.guid;

	TEST_CHECK_EXCEPTION
		( userModel.AddResource(resource)
		, std::exception
		, MESSAGE_EQUALS("foreign key constraint failed")
		);
}

FIXTURE_TEST_CASE(TestUserModelImageResource1, DataStoreFixture)
{
	std::string hash ("hash");
	Note        note (MakeNote(L"note-0"));

	Resource resource;
	resource.Data.push_back(2);
	resource.Data.push_back(3);
	resource.Data.push_back(5);
	resource.Data.push_back(7);
	resource.Hash = hash;
	resource.Note = note.guid;

	Notebook notebook;
	userModel.GetLastUsedNotebook(notebook);
	userModel.AddNote(note, L"", L"", notebook);
	userModel.AddResource(resource);

	Blob loaded;
	userModel.GetResource(hash, loaded);
	TEST_CHECK_EQUAL(resource.Data.size(), loaded.size());
	for (int i = 0; i != resource.Data.size(); ++i)
		TEST_CHECK_EQUAL(resource.Data.at(i), loaded.at(i));
}

FIXTURE_TEST_CASE(TestUserModelLastUsedNotebook, DataStoreFixture)
{
	Notebook notebook0;
	notebook0.name = L"notebook0";
	Notebook notebook1;
	notebook1.name = L"notebook1";
	Notebook notebook2;
	notebook2.name = L"notebook2";
	userModel.AddNotebook(notebook0);
	userModel.AddNotebook(notebook1);
	userModel.AddNotebook(notebook2);
	userModel.MakeNotebookLastUsed(notebook1);

	Notebook lastUsedNotebook;
	userModel.GetLastUsedNotebook(lastUsedNotebook);
	TEST_CHECK_EQUAL(lastUsedNotebook.name, L"notebook1");
}

AUTO_TEST_CASE(TestUserModelLoad)
{
	DataStore store;
	UserModel userModel(store, storeFolder);

	SignalCheck check;
	userModel.ConnectLoaded(boost::ref(check));

	::DeleteFile(storeFile);
	TEST_CHECK_EXCEPTION
		( userModel.Load(storeName)
		, std::exception
		, MESSAGE_EQUALS("Database could not be loaded.")
		);
	TEST_CHECK(!check.signalled);

	{
		DataStore store;
		UserModel userModel(store, storeFolder);
		userModel.LoadOrCreate(storeName);
	}

	userModel.Load(storeName);
	TEST_CHECK(check.signalled);

	Credentials credentials;
	userModel.GetCredentials(credentials);
	TEST_CHECK_EQUAL(credentials.GetUsername(), storeName);
}

AUTO_TEST_CASE(TestUserModelLoadAs)
{
	DataStore store;
	UserModel userModel(store, storeFolder);

	SignalCheck check;
	userModel.ConnectLoaded(boost::ref(check));

	const wchar_t * dstName = L"test2";
	const wchar_t * dstFile = L"Program Files\\MobileTest\\test2.db";

	::DeleteFile(storeFile);
	TEST_CHECK_EXCEPTION
		( userModel.LoadAs(storeName, dstName)
		, std::exception
		, MESSAGE_EQUALS("Database could not be renamed.")
		);
	TEST_CHECK(!check.signalled);

	{
		DataStore store;
		UserModel model(store, storeFolder);
		model.LoadOrCreate(storeName);
	}

	{ ofstream(dstFile) << 0; }
	TEST_CHECK_EXCEPTION
		( userModel.LoadAs(storeName, dstName)
		, std::exception
		, MESSAGE_EQUALS("Database could not be renamed.")
		);
	TEST_CHECK(!check.signalled);

	::DeleteFile(dstFile);
	userModel.LoadAs(storeName, dstName);
	TEST_CHECK(check.signalled);

	Credentials credentials;
	userModel.GetCredentials(credentials);
	TEST_CHECK_EQUAL(credentials.GetUsername(), dstName);
}

AUTO_TEST_CASE(TestUserModelLoadOrCreate)
{
	{
		DataStore store;
		UserModel userModel(store, storeFolder);

		SignalCheck check;
		userModel.ConnectLoaded(boost::ref(check));

		::DeleteFile(storeFile);
		TEST_CHECK(!FileExists(storeFile));
		userModel.LoadOrCreate(storeName);
		TEST_CHECK(FileExists(storeFile));
		TEST_CHECK(check.signalled);

		Credentials credentials;
		userModel.GetCredentials(credentials);
		TEST_CHECK_EQUAL(userModel.GetVersion(),       0);
		TEST_CHECK_EQUAL(credentials.GetUsername(),    storeName);
		TEST_CHECK_EQUAL(userModel.GetNotebookCount(), 1);

		userModel.SetCredentials(storeName, L"test-pwd");

		userModel.AddNotebook(Notebook());
	}
	{
		DataStore store;
		UserModel userModel(store, storeFolder);

		SignalCheck check;
		userModel.ConnectLoaded(boost::ref(check));

		userModel.LoadOrCreate(storeName);
		TEST_CHECK(check.signalled);

		Credentials credentials;
		userModel.GetCredentials(credentials);
		TEST_CHECK_EQUAL(userModel.GetVersion(),       0);
		TEST_CHECK_EQUAL(credentials.GetUsername(),    storeName);
		TEST_CHECK_EQUAL(credentials.GetPassword(),    L"test-pwd");
		TEST_CHECK_EQUAL(userModel.GetNotebookCount(), 2);
	}
}

FIXTURE_TEST_CASE(TestUserModelNotebooks, DataStoreFixture)
{
	Notebook notebook0;
	notebook0.name = L"notebook0";
	Notebook notebook1;
	notebook1.name = L"notebook1";
	Notebook notebook2;
	notebook2.name = L"notebook2";
	userModel.AddNotebook(notebook1);
	userModel.AddNotebook(notebook0);
	userModel.AddNotebook(notebook2);

	NotebookList notebooks;
	userModel.GetNotebooks(notebooks);
	TEST_CHECK_EQUAL(notebooks.size(), 4);
	TEST_CHECK_EQUAL(notebooks.at(0).name, L"Notes");
	TEST_CHECK_EQUAL(notebooks.at(1).name, L"notebook0");
	TEST_CHECK_EQUAL(notebooks.at(2).name, L"notebook1");
	TEST_CHECK_EQUAL(notebooks.at(3).name, L"notebook2");
}

FIXTURE_TEST_CASE(TestUserModelNotesByNotebook, DataStoreFixture)
{
	vector<Notebook> notebooks;
	notebooks.push_back(Notebook());
	notebooks.back().name = L"notebook-0";
	notebooks.push_back(Notebook());
	notebooks.back().name = L"notebook-1";
	foreach (const Notebook & notebook, notebooks)
		userModel.AddNotebook(notebook);

	wstring empty;

	userModel.AddNote
		( MakeNote(L"note-0")
		, empty
		, empty
		, notebooks.at(0)
		);
	userModel.AddNote
		( MakeNote(L"note-1")
		, empty
		, empty
		, notebooks.at(1)
		);
	userModel.AddNote
		( MakeNote(L"note-2")
		, empty
		, empty
		, notebooks.at(0)
		);

	NoteList notes;
	userModel.GetNotesByNotebook(notebooks.at(0), notes);
	TEST_CHECK_EQUAL(notes.size(), 2);
	TEST_CHECK_EQUAL(notes.at(0).name, L"note-0");
	TEST_CHECK_EQUAL(notes.at(1).name, L"note-2");
}

FIXTURE_TEST_CASE(TestUserModelNotesBySearch, DataStoreFixture)
{
	Notebook notebook;
	notebook.name = L"notebook";
	userModel.AddNotebook(notebook);

	wstring empty;

	userModel.AddNote
		( MakeNote(L"useful software", 0)
		, empty
		, empty
		, notebook
		);
	userModel.AddNote
		( MakeNote(L"software use", 1)
		, empty
		, empty
		, notebook
		);

	NoteList notes0;
	userModel.GetNotesBySearch(L"software", notes0);
	TEST_CHECK_EQUAL(notes0.size(), 2);
	TEST_CHECK_EQUAL(notes0.at(0).name, L"useful software");
	TEST_CHECK_EQUAL(notes0.at(1).name, L"software use");

	NoteList notes1;
	userModel.GetNotesBySearch(L"use", notes1);
	TEST_CHECK_EQUAL(notes1.size(), 1);
	TEST_CHECK_EQUAL(notes1.at(0).name, L"software use");
}

FIXTURE_TEST_CASE(TestUserModelThumbnail, DataStoreFixture)
{
	Notebook notebook;
	userModel.GetDefaultNotebook(notebook);

	wstring empty;

	Guid guid;

	userModel.AddNote
		( MakeNote(guid, L"note")
		, empty
		, empty
		, notebook
		);
	
	Thumbnail in;
	in.Data.push_back(2);
	in.Data.push_back(3);
	in.Data.push_back(5);
	in.Width  = 7;
	in.Height = 11;
	userModel.SetNoteThumbnail(guid, in);

	Thumbnail out;
	userModel.GetNoteThumbnail(guid, out);
	TEST_CHECK_EQUAL(out.Data.size(), 3);
	TEST_CHECK_EQUAL(out.Data.at(0), 2);
	TEST_CHECK_EQUAL(out.Data.at(1), 3);
	TEST_CHECK_EQUAL(out.Data.at(2), 5);
	TEST_CHECK_EQUAL(out.Width,  7);
	TEST_CHECK_EQUAL(out.Height, 11);
}

FIXTURE_TEST_CASE(TestUserModelUnload, DataStoreFixture)
{
	TEST_CHECK_EQUAL(::DeleteFile(storeFile), FALSE);
	userModel.Unload();
	TEST_CHECK_EQUAL(::DeleteFile(storeFile), TRUE);
}
