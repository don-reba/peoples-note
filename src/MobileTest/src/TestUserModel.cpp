#include "stdafx.h"

#include "DataStore.h"
#include "MockFlashCard.h"
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
	MockFlashCard flashCard;

	DataStore store;
	UserModel userModel;

	DataStoreFixture()
		: userModel (store, storeFolder, flashCard)
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
	note.modificationDate = timestamp;
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
	, Timestamp       modificationDate
	, int             usn
	, bool            isDirty
	)
{
	Note note;
	note.guid             = guid;
	note.name             = name;
	note.modificationDate = modificationDate;
	note.usn              = usn;
	note.isDirty          = isDirty;
	return note;
}

#define TestNoteEquality(note1, note2)                                                        \
        TEST_CHECK_EQUAL(note1.guid,                       note2.guid);                       \
        TEST_CHECK_EQUAL(note1.usn,                        note2.usn);                        \
        TEST_CHECK_EQUAL(note1.name,                       note2.name);                       \
        TEST_CHECK_EQUAL(note1.isDirty,                    note2.isDirty);                    \
        TEST_CHECK_EQUAL(note1.creationDate.GetTime(),     note2.creationDate.GetTime());     \
        TEST_CHECK_EQUAL(note1.modificationDate.GetTime(), note2.modificationDate.GetTime()); \
        TEST_CHECK_EQUAL(note1.subjectDate.GetTime(),      note2.subjectDate.GetTime());      \
        TEST_CHECK_EQUAL(note1.Location.Altitude,          note2.Location.Altitude);          \
        TEST_CHECK_EQUAL(note1.Location.Latitude,          note2.Location.Latitude);          \
        TEST_CHECK_EQUAL(note1.Location.Longitude,         note2.Location.Longitude);         \
        TEST_CHECK_EQUAL(note1.Author,                     note2.Author);                     \
        TEST_CHECK_EQUAL(note1.Source,                     note2.Source);                     \
        TEST_CHECK_EQUAL(note1.SourceUrl,                  note2.SourceUrl);                  \
        TEST_CHECK_EQUAL(note1.SourceApplication,          note2.SourceApplication);          \

//-----------
// test suite
//-----------

FIXTURE_TEST_CASE(UserModelAddNote, DataStoreFixture)
{
	Note note0;
	note0.usn                = 3;
	note0.isDirty            = true;
	note0.name               = L"note-0";
	note0.creationDate       = Timestamp(2);
	note0.modificationDate   = Timestamp(3);
	note0.subjectDate        = Timestamp(4);
	note0.Location.IsValid   = true;
	note0.Location.Altitude  = -0.5;
	note0.Location.Latitude  = 0.5;
	note0.Location.Longitude = 1.5;
	note0.Author             = L"Motoko";
	note0.Source             = L"Sea of information";
	note0.SourceApplication  = L"Project 2501";
	note0.SourceUrl          = L"http://peoplesnote.org";
	wstring body0(L"<html>note body 0</html>");

	Note note1;
	note1.usn              = 8;
	note1.isDirty          = true;
	note1.name             = L"note-1";
	note1.creationDate     = Timestamp(5);
	note1.modificationDate = Timestamp(6);
	note1.subjectDate      = Timestamp(7);
	wstring body1(L"<html>note body 1</html>");

	Notebook notebook;
	userModel.GetLastUsedNotebook(notebook);
	userModel.AddNote(note0, body0, L"", notebook);
	userModel.AddNote(note1, body1, L"", notebook);

	Tag tag0;
	tag0.name    = L"test-tag-0";
	tag0.usn     = 2;
	tag0.isDirty = true;

	Tag tag1;
	tag1.name    = L"test-tag-1";
	tag1.usn     = 3;
	tag1.isDirty = false;

	Tag tag2;
	tag2.name    = L"test-tag-2";

	userModel.AddTag(tag2);
	userModel.AddTag(tag0);
	userModel.AddTag(tag1);

	userModel.AddTagToNote(L"test-tag-2", note0);
	userModel.AddTagToNote(L"test-tag-1", note0);

	{
		Note result;
		userModel.GetNote(note0.guid, result);
		TestNoteEquality(result, note0);

		wstring loaded;
		userModel.GetNoteBody(note0.guid, loaded);
		TEST_CHECK_EQUAL(loaded, L"<html>note body 0</html>");
	}
	{
		Note result;
		userModel.GetNote(note1.guid, result);
		TEST_CHECK_EQUAL(result.usn,                        8);
		TEST_CHECK_EQUAL(result.isDirty,                    true);
		TEST_CHECK_EQUAL(result.name,                       L"note-1");
		TEST_CHECK_EQUAL(result.creationDate.GetTime(),     5L);
		TEST_CHECK_EQUAL(result.modificationDate.GetTime(), 6L);
		TEST_CHECK_EQUAL(result.subjectDate.GetTime(),      7L);

		wstring loaded;
		userModel.GetNoteBody(note1.guid, loaded);
		TEST_CHECK_EQUAL(loaded, L"<html>note body 1</html>");
	}
	{
		TagList tags;
		userModel.GetTags(tags);
		TEST_CHECK_EQUAL(tags.size(), 3);
		TEST_CHECK_EQUAL(tags.at(0).guid,       tag0.guid);
		TEST_CHECK_EQUAL(tags.at(0).name,       L"test-tag-0");
		TEST_CHECK_EQUAL(tags.at(0).usn,        2);
		TEST_CHECK_EQUAL(tags.at(0).isDirty,    true);
		TEST_CHECK_EQUAL(tags.at(0).parentGuid, tag0.parentGuid);
		TEST_CHECK_EQUAL(tags.at(1).guid,       tag1.guid);
		TEST_CHECK_EQUAL(tags.at(1).name,       L"test-tag-1");
		TEST_CHECK_EQUAL(tags.at(1).usn,        3);
		TEST_CHECK_EQUAL(tags.at(1).isDirty,    false);
		TEST_CHECK_EQUAL(tags.at(2).guid,       tag2.guid);
	}
	{
		TagList tags;
		userModel.GetNoteTags(note0, tags);
		TEST_CHECK_EQUAL(tags.size(), 2);
		TEST_CHECK_EQUAL(tags.at(0).guid,       tag1.guid);
		TEST_CHECK_EQUAL(tags.at(0).name,       L"test-tag-1");
		TEST_CHECK_EQUAL(tags.at(0).usn,        3);
		TEST_CHECK_EQUAL(tags.at(0).isDirty,    false);
		TEST_CHECK_EQUAL(tags.at(0).parentGuid, tag1.parentGuid);
		TEST_CHECK_EQUAL(tags.at(1).guid,       tag2.guid);
		TEST_CHECK_EQUAL(tags.at(1).name,       L"test-tag-2");

		tags.clear();
		userModel.GetNoteTags(note1, tags);
		TEST_CHECK(tags.empty());
	}
}

FIXTURE_TEST_CASE(UserModelAddNotebook, DataStoreFixture)
{
	Notebook notebook;
	notebook.name = L"test-notebook";
	notebook.CreationDate     = Timestamp(1);
	notebook.ModificationDate = Timestamp(2);
	userModel.AddNotebook(notebook);

	TEST_CHECK_EQUAL(userModel.GetNotebookCount(), 2);

	userModel.AddNotebook(notebook);
	TEST_CHECK_EQUAL(userModel.GetNotebookCount(), 2);
}

FIXTURE_TEST_CASE(UserModelCascade, DataStoreFixture)
{
	Notebook notebook;
	Note note0;
	Note note1;
	Resource resource0;
	Resource resource1;

	resource0.Note = note0.guid;
	resource1.Note = note0.guid;
	resource0.Hash = "0";
	resource1.Hash = "1";

	userModel.AddNotebook(notebook);
	userModel.AddNote(note0, L"", L"", notebook);
	userModel.AddNote(note1, L"", L"", notebook);
	userModel.AddResource(resource0);
	userModel.AddResource(resource1);

	TEST_CHECK_EQUAL(userModel.GetNotebookCount(), 2);
	TEST_CHECK_EQUAL(userModel.GetNoteCount(),     2);
	TEST_CHECK_EQUAL(userModel.GetResourceCount(), 2);

	userModel.ExpungeNote(note0.guid);

	TEST_CHECK_EQUAL(userModel.GetNotebookCount(), 2);
	TEST_CHECK_EQUAL(userModel.GetNoteCount(),     1);
	TEST_CHECK_EQUAL(userModel.GetResourceCount(), 0);

	userModel.ExpungeNotebook(notebook.guid);

	TEST_CHECK_EQUAL(userModel.GetNotebookCount(), 1);
	TEST_CHECK_EQUAL(userModel.GetNoteCount(),     0);
	TEST_CHECK_EQUAL(userModel.GetResourceCount(), 0);
}

FIXTURE_TEST_CASE(UserModelDeleteNote, DataStoreFixture)
{
	Notebook notebook;
	userModel.GetDefaultNotebook(notebook);

	Note note1;
	Note note2;
	userModel.AddNote(note1, L"", L"", notebook);
	userModel.AddNote(note2, L"", L"", notebook);

	userModel.DeleteNote(note2.guid);

	NoteList notes;
	userModel.GetNotesByNotebook(notebook, notes);
	TEST_CHECK_EQUAL(notes.size(), 1);
	TEST_CHECK_EQUAL(notes.at(0).guid, note1.guid);

	GuidList deletedNotes;
	userModel.GetDeletedNotes(deletedNotes);
	TEST_CHECK_EQUAL(deletedNotes.size(), 1);
	TEST_CHECK_EQUAL(deletedNotes.at(0), note2.guid);
}

FIXTURE_TEST_CASE(UserModelExpungeNote, DataStoreFixture)
{
	Notebook notebook;
	userModel.GetDefaultNotebook(notebook);

	Note note;
	userModel.AddNote(note, L"", L"", notebook);
	userModel.ExpungeNote(note.guid);
	
	NoteList notes;
	userModel.GetNotesByNotebook(notebook, notes);
	TEST_CHECK(notes.empty());
}

FIXTURE_TEST_CASE(UserModelExpungeNotebook, DataStoreFixture)
{
	Notebook defaultNotebook;
	userModel.GetDefaultNotebook(defaultNotebook);

	userModel.ExpungeNotebook(defaultNotebook.guid);
	TEST_CHECK_EQUAL(userModel.GetNotebookCount(), 1);

	Notebook newNotebook;
	userModel.AddNotebook(newNotebook);
	userModel.MakeNotebookLastUsed(newNotebook.guid);
	TEST_CHECK_EQUAL(userModel.GetNotebookCount(), 2);

	Notebook lastUsedNotebook;
	userModel.GetLastUsedNotebook(lastUsedNotebook);
	TEST_CHECK_EQUAL(lastUsedNotebook.guid, newNotebook.guid);

	userModel.ExpungeNotebook(lastUsedNotebook.guid);
	TEST_CHECK_EQUAL(userModel.GetNotebookCount(), 1);
}

FIXTURE_TEST_CASE(UserModelExpungedTag, DataStoreFixture)
{
	Tag tag;
	userModel.AddTag(tag);
	userModel.ExpungeTag(tag.guid);
	
	TagList tags;
	userModel.GetTags(tags);
	TEST_CHECK(tags.empty());
}

AUTO_TEST_CASE(UserModelExists)
{
	MockFlashCard flashCard;

	DataStore store;
	UserModel userModel(store, storeFolder, flashCard);

	::DeleteFile(storeFile);
	TEST_CHECK(!userModel.Exists(storeName));

	{ ofstream(storeFile) << 0; }
	TEST_CHECK(userModel.Exists(storeName));
}

FIXTURE_TEST_CASE(UserModelNoteForeignKey, DataStoreFixture)
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
		, MESSAGE_EQUALS("constraint failed")
		);
}

FIXTURE_TEST_CASE(UserModelDefaultNotebook, DataStoreFixture)
{
	Notebook notebook;
	notebook.name = L"test-notebook";
	userModel.AddNotebook(notebook);
	userModel.MakeNotebookDefault(notebook.guid);

	Notebook defaultNotebook;
	userModel.GetDefaultNotebook(defaultNotebook);
	TEST_CHECK_EQUAL(defaultNotebook.name, L"test-notebook");
}

FIXTURE_TEST_CASE(UserModelGetDirtyNoteCount, DataStoreFixture)
{
	Notebook notebook;
	userModel.GetDefaultNotebook(notebook);

	Note notes[3];
	notes[0].name    = L"note-0";
	notes[0].isDirty = true;
	notes[1].name    = L"note-1";
	notes[1].isDirty = false;
	notes[2].name    = L"note-2";
	notes[2].isDirty = true;

	for (int i(0); i != 3; ++i)
		userModel.AddNote(notes[i], L"", L"", notebook);

	int dirtyNoteCount(userModel.GetDirtyNoteCount(notebook));
	TEST_CHECK_EQUAL(dirtyNoteCount, 2);
}

FIXTURE_TEST_CASE(UserModelGetNotebook, DataStoreFixture)
{
	Notebook notebook;
	notebook.name = L"test-notebook";
	userModel.AddNotebook(notebook);

	Notebook result;
	userModel.GetNotebook(notebook.guid, result);

	TEST_CHECK_EQUAL(result.name, L"test-notebook");
}

FIXTURE_TEST_CASE(UserModelReplacement, DataStoreFixture)
{
	Notebook notebook0;
	Notebook notebook1;
	Note note0;
	Note note1;
	Tag tag0;
	Tag tag1;
	Resource resource0;
	Resource resource1;
	Resource resource2;
	Resource resource;

	notebook1.guid = notebook0.guid;
	notebook0.name = L"notebook-0";
	notebook1.name = L"notebook-1";
	notebook0.CreationDate     = Timestamp(1);
	notebook1.CreationDate     = Timestamp(2);
	notebook0.ModificationDate = Timestamp(3);
	notebook1.ModificationDate = Timestamp(4);
	note1.guid     = note0.guid;
	note0.name     = L"note-0";
	note1.name     = L"note-1";
	tag1.guid      = tag0.guid;
	tag0.name      = L"tag-0";
	tag1.name      = L"tag-1";
	resource1.Guid = resource0.Guid;
	resource0.Hash = "0";
	resource1.Hash = "1";
	resource2.Hash = "1";
	resource0.Note = note1.guid;
	resource1.Note = note1.guid;
	resource2.Note = note1.guid;

	resource0.Data.push_back(2);
	resource1.Data.push_back(3);
	resource2.Data.push_back(5);

	userModel.AddNotebook(notebook0);
	userModel.AddNotebook(notebook1);

	NotebookList notebooks;
	userModel.GetNotebooks(notebooks);
	TEST_CHECK_EQUAL(notebooks.size(), 2);
	TEST_CHECK_EQUAL(notebooks.at(1).name, L"notebook-1");
	TEST_CHECK_EQUAL(notebooks.at(1).CreationDate.GetTime(),     2L);
	TEST_CHECK_EQUAL(notebooks.at(1).ModificationDate.GetTime(), 4L);

	userModel.AddNote(note0, L"", L"", notebook1);
	userModel.AddNote(note1, L"", L"", notebook1);

	NoteList notes;
	userModel.GetNotesByNotebook(notebook1, notes);
	TEST_CHECK_EQUAL(notes.size(), 1);
	TEST_CHECK_EQUAL(notes.at(0).name, L"note-1");

	userModel.AddTag(tag0);
	userModel.AddTag(tag1);

	TagList tags;
	userModel.GetTags(tags);
	TEST_CHECK_EQUAL(tags.size(), 1);
	TEST_CHECK_EQUAL(tags.at(0).name, L"tag-1");

	userModel.AddResource(resource0);
	userModel.AddResource(resource1);

	TEST_CHECK_EQUAL(userModel.GetResourceCount(), 1);

	userModel.GetResource(resource0.Guid, resource);
	TEST_CHECK_EQUAL(resource.Hash, "1");
	TEST_CHECK_EQUAL(resource.Data.at(0), 3);

	userModel.AddResource(resource2);

	TEST_CHECK_EQUAL(userModel.GetResourceCount(), 1);

	userModel.GetResource(resource2.Guid, resource);
	TEST_CHECK_EQUAL(resource.Hash, "1");
	TEST_CHECK_EQUAL(resource.Data.at(0), 5);
}

FIXTURE_TEST_CASE(UserModelResource0, DataStoreFixture)
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
		, MESSAGE_EQUALS("constraint failed")
		);
}

FIXTURE_TEST_CASE(UserModelResource1, DataStoreFixture)
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
	userModel.AddResource(resource);

	Blob loaded;
	userModel.GetResource(hash, loaded);
	TEST_CHECK_EQUAL(resource.Data.size(), loaded.size());
	for (int i = 0; i != resource.Data.size(); ++i)
		TEST_CHECK_EQUAL(resource.Data.at(i), loaded.at(i));
}

FIXTURE_TEST_CASE(UserModelLastUsedNotebook, DataStoreFixture)
{
	Notebook notebook0;
	notebook0.name = L"notebook0";
	Notebook notebook1;
	notebook1.name    = L"notebook1";
	notebook1.isDirty = true;
	notebook1.usn     = 2;
	Notebook notebook2;
	notebook2.name = L"notebook2";
	userModel.AddNotebook(notebook0);
	userModel.AddNotebook(notebook1);
	userModel.AddNotebook(notebook2);
	userModel.MakeNotebookLastUsed(notebook1.guid);

	Notebook lastUsedNotebook;
	lastUsedNotebook.isDirty = false;
	userModel.GetLastUsedNotebook(lastUsedNotebook);
	TEST_CHECK_EQUAL(lastUsedNotebook.guid,    notebook1.guid);
	TEST_CHECK_EQUAL(lastUsedNotebook.name,    L"notebook1");
	TEST_CHECK_EQUAL(lastUsedNotebook.isDirty, true);
	TEST_CHECK_EQUAL(lastUsedNotebook.usn,     2);
}

AUTO_TEST_CASE(UserModelLoad)
{
	MockFlashCard flashCard;

	DataStore store;
	UserModel userModel(store, storeFolder, flashCard);

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
		MockFlashCard flashCard;

		DataStore store;
		UserModel userModel(store, storeFolder, flashCard);

		userModel.LoadOrCreate(storeName);
	}

	userModel.Load(storeName);
	TEST_CHECK(check.signalled);

	Credentials credentials;
	userModel.GetCredentials(credentials);
	TEST_CHECK_EQUAL(credentials.GetUsername(), storeName);
}

AUTO_TEST_CASE(UserModelLoadAs)
{
	MockFlashCard flashCard;

	DataStore store;
	UserModel userModel(store, storeFolder, flashCard);

	SignalCheck check;
	userModel.ConnectLoaded(boost::ref(check));

	const wchar_t * dstName = L"test2";
	const wchar_t * dstFile = L"Program Files\\MobileTest\\test2.db";

	::DeleteFile(storeFile);
	TEST_CHECK_EXCEPTION
		( userModel.LoadAs(storeName, dstName)
		, std::exception
		, MESSAGE_EQUALS("Database could not be found.")
		);
	TEST_CHECK(!check.signalled);

	{
		MockFlashCard flashCard;

		DataStore store;
		UserModel userModel(store, storeFolder, flashCard);

		userModel.LoadOrCreate(storeName);
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

AUTO_TEST_CASE(UserModelLoadOrCreate)
{
	{
		MockFlashCard flashCard;

		DataStore store;
		UserModel userModel(store, storeFolder, flashCard);

		SignalCheck check;
		userModel.ConnectLoaded(boost::ref(check));

		::DeleteFile(storeFile);
		TEST_CHECK(!FileExists(storeFile));
		userModel.LoadOrCreate(storeName);
		TEST_CHECK(FileExists(storeFile));
		TEST_CHECK(check.signalled);

		Credentials credentials;
		userModel.GetCredentials(credentials);
		TEST_CHECK_EQUAL(userModel.GetVersion(),       5);
		TEST_CHECK_EQUAL(credentials.GetUsername(),    storeName);
		TEST_CHECK_EQUAL(userModel.GetNotebookCount(), 1);

		userModel.SetCredentials(storeName, L"test-pwd");

		userModel.AddNotebook(Notebook());
	}
	{
		MockFlashCard flashCard;

		DataStore store;
		UserModel userModel(store, storeFolder, flashCard);
		SignalCheck check;
		userModel.ConnectLoaded(boost::ref(check));

		userModel.LoadOrCreate(storeName);
		TEST_CHECK(check.signalled);

		Credentials credentials;
		userModel.GetCredentials(credentials);
		TEST_CHECK_EQUAL(userModel.GetVersion(),       5);
		TEST_CHECK_EQUAL(credentials.GetUsername(),    storeName);
		TEST_CHECK_EQUAL(credentials.GetPassword(),    L"test-pwd");
		TEST_CHECK_EQUAL(userModel.GetNotebookCount(), 2);
	}
}

FIXTURE_TEST_CASE(UserModelNotebooks, DataStoreFixture)
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

FIXTURE_TEST_CASE(UserModelNotesByNotebook, DataStoreFixture)
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

FIXTURE_TEST_CASE(UserModelGetNotesBySearch1, DataStoreFixture)
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

	NoteList notes;
	userModel.GetNotesBySearch(L"software", notes);
	TEST_CHECK_EQUAL(notes.size(), 2);
	TEST_CHECK_EQUAL(notes.at(0).name, L"software use");
	TEST_CHECK_EQUAL(notes.at(1).name, L"useful software");

	notes.clear();
	userModel.GetNotesBySearch(L"use", notes);
	TEST_CHECK_EQUAL(notes.size(), 1);
	TEST_CHECK_EQUAL(notes.at(0).name, L"software use");
}

FIXTURE_TEST_CASE(UserModelGetNotesBySearch2, DataStoreFixture)
{
	Notebook notebook;
	notebook.name = L"notebook";
	userModel.AddNotebook(notebook);

	wstring empty;

	userModel.AddNote
		( MakeNote(L"Ангара", 0)
		, empty
		, L"Восточный"
		, notebook
		);

	NoteList notes;
	userModel.GetNotesBySearch(L"ангара", notes);
	TEST_CHECK_EQUAL(notes.size(), 1);

	notes.clear();
	userModel.GetNotesBySearch(L"восточный", notes);
	TEST_CHECK_EQUAL(notes.size(), 1);
}

FIXTURE_TEST_CASE(UserModelTags, DataStoreFixture)
{
	TagList tags1;
	tags1.push_back(Tag());
	tags1.back().name = L"tag-c";
	tags1.push_back(Tag());
	tags1.back().name = L"tag-a";
	tags1.push_back(Tag());
	tags1.back().name = L"tag-b";

	foreach (const Tag & tag, tags1)
		userModel.AddTag(tag);

	TagList tags2;
	userModel.GetTags(tags2);
	TEST_CHECK_EQUAL(tags2.size(), 3);
	TEST_CHECK_EQUAL(tags2.at(0).name, L"tag-a");
	TEST_CHECK_EQUAL(tags2.at(1).name, L"tag-b");
	TEST_CHECK_EQUAL(tags2.at(2).name, L"tag-c");
}

FIXTURE_TEST_CASE(UserModelThumbnail, DataStoreFixture)
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

	userModel.DeleteNoteThumbnail(guid);

	out.Data.clear();
	userModel.GetNoteThumbnail(guid, out);
	TEST_CHECK(out.Data.empty());
}

FIXTURE_TEST_CASE(UserModelUnload, DataStoreFixture)
{
	TEST_CHECK_EQUAL(::DeleteFile(storeFile), FALSE);
	userModel.Unload();
	TEST_CHECK_EQUAL(::DeleteFile(storeFile), TRUE);
}

FIXTURE_TEST_CASE(UserModelUpdateNote, DataStoreFixture)
{
	Notebook notebook;
	userModel.GetDefaultNotebook(notebook);

	Guid guid1;
	Note note;
	note.guid               = guid1;
	note.usn                = 1;
	note.name               = L"note 1";
	note.isDirty            = true;
	note.creationDate       = Timestamp(1);
	note.modificationDate   = Timestamp(2);
	note.subjectDate        = Timestamp(3);
	note.Location.IsValid   = true;
	note.Location.Altitude  = 1;
	note.Location.Latitude  = 2;
	note.Location.Longitude = 3;
	note.Author             = L"Motoko";
	note.Source             = L"Sea of informaiton";
	note.SourceApplication  = L"Project 2501";
	note.SourceUrl          = L"http://www.peoplesnote.org";


	userModel.AddNote(note, L"", L"", notebook);

	NoteList notes;
	userModel.GetNotesByNotebook(notebook, notes);
	TEST_CHECK_EQUAL(notes.size(), 1);
	TestNoteEquality(notes.at(0), note);

	Guid guid2;
	note.guid               = guid2;
	note.usn                = 2;
	note.name               = L"note 2";
	note.isDirty            = false;
	note.creationDate       = Timestamp(4);
	note.modificationDate   = Timestamp(5);
	note.subjectDate        = Timestamp(6);
	note.Location.IsValid   = false;
	note.Location.Altitude  = 7;
	note.Location.Latitude  = 8;
	note.Location.Longitude = 9;
	note.Author             = L"Kusanagi";
	note.Source             = L"Section 9";
	note.SourceApplication  = L"Public Security";
	note.SourceUrl          = L"http://peoplesnote.org";

	userModel.UpdateNote(guid1, note);

	note.Location.Altitude  = 0;
	note.Location.Latitude  = 0;
	note.Location.Longitude = 0;

	notes.clear();
	userModel.GetNotesByNotebook(notebook, notes);
	TEST_CHECK_EQUAL(notes.size(), 1);
	TestNoteEquality(notes.at(0), note);
}

FIXTURE_TEST_CASE(UserModelUpdateNotebook, DataStoreFixture)
{
	Notebook notebook;
	userModel.GetDefaultNotebook(notebook);

	Note note;
	note.name = L"note-0";
	userModel.AddNote(note, L"", L"", notebook);

	Notebook replacementNotebook;
	replacementNotebook.guid    = Guid("{1}");
	replacementNotebook.isDirty = false;
	replacementNotebook.name    = L"notebook-1";
	replacementNotebook.usn     = 1;

	userModel.UpdateNotebook(notebook.guid, replacementNotebook);

	NotebookList notebooks;
	userModel.GetNotebooks(notebooks);

	TEST_CHECK_EQUAL(notebooks.size(), 1);
	TEST_CHECK_EQUAL(notebooks.at(0).guid,    replacementNotebook.guid);
	TEST_CHECK_EQUAL(notebooks.at(0).isDirty, replacementNotebook.isDirty);
	TEST_CHECK_EQUAL(notebooks.at(0).name,    replacementNotebook.name);
	TEST_CHECK_EQUAL(notebooks.at(0).usn,     replacementNotebook.usn);

	Notebook defaultNotebook;
	userModel.GetDefaultNotebook(defaultNotebook);
	TEST_CHECK_EQUAL(defaultNotebook.guid, replacementNotebook.guid);

	Notebook lastUsedNotebook;
	userModel.GetLastUsedNotebook(lastUsedNotebook);
	TEST_CHECK_EQUAL(lastUsedNotebook.guid, replacementNotebook.guid);

	NoteList notes;
	userModel.GetNotesByNotebook(replacementNotebook, notes);

	TEST_CHECK_EQUAL(notes.size(), 1);
	TEST_CHECK_EQUAL(notes.at(0).name, L"note-0");
}

FIXTURE_TEST_CASE(UserModelUpdateTag, DataStoreFixture)
{
	Tag tag;
	tag.guid    = Guid("{0}");
	tag.isDirty = true;
	tag.name    = L"tag-0";
	tag.usn     = 0;

	userModel.AddTag(tag);

	Tag replacementTag;
	replacementTag.guid    = Guid("{1}");
	replacementTag.isDirty = false;
	replacementTag.name    = L"tag-1";
	replacementTag.usn     = 1;

	userModel.UpdateTag(tag.guid, replacementTag);

	TagList tags;
	userModel.GetTags(tags);

	TEST_CHECK_EQUAL(tags.size(), 1);
	TEST_CHECK_EQUAL(tags.at(0).guid,    replacementTag.guid);
	TEST_CHECK_EQUAL(tags.at(0).isDirty, replacementTag.isDirty);
	TEST_CHECK_EQUAL(tags.at(0).name,    replacementTag.name);
	TEST_CHECK_EQUAL(tags.at(0).usn,     replacementTag.usn);
}

FIXTURE_TEST_CASE(UserModelSyncVersion, DataStoreFixture)
{
	TEST_CHECK_EQUAL(userModel.GetSyncVersion(), 0);
	userModel.SetSyncVersion(17);
	TEST_CHECK_EQUAL(userModel.GetSyncVersion(), 17);
}
