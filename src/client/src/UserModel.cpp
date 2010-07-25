#include "stdafx.h"
#include "UserModel.h"

#include "IDataStore.h"
#include "ISqlBlob.h"
#include "ISqlStatement.h"
#include "Notebook.h"
#include "Transaction.h"

#include "SQLite/sqlite3.h"

#include <algorithm>
#include <iterator>
#include <sstream>

using namespace boost;
using namespace std;

//----------
// interface
//----------

UserModel::UserModel
	( IDataStore & dataStore
	, wstring      folder
	)
	: dataStore (dataStore)
	, folder    (folder)
{
}

int UserModel::GetNoteCount()
{
	IDataStore::Statement statement = dataStore.MakeStatement
		( "SELECT COUNT(*)"
		"  FROM Notes"
		);
	statement->Execute();
	int count(0);
	statement->Get(0, count);
	return count;
}

int UserModel::GetNotebookCount()
{
	IDataStore::Statement statement = dataStore.MakeStatement
		( "SELECT COUNT(*)"
		"  FROM Notebooks"
		);
	statement->Execute();
	int count(0);
	statement->Get(0, count);
	return count;
}

int UserModel::GetResourceCount()
{
	IDataStore::Statement statement = dataStore.MakeStatement
		( "SELECT COUNT(*)"
		"  FROM Resources"
		);
	statement->Execute();
	int count(0);
	statement->Get(0, count);
	return count;
}

int UserModel::GetVersion()
{
	int version(0);
	GetProperty(L"version", version);
	return version;
}

//--------------------------
// IUuserModel implementaion
//--------------------------

void UserModel::AddNote
	( const Note     & note
	, const wstring  & body
	, const wstring  & bodyText
	, const Notebook & notebook
	)
{
	Transaction transaction(*this);

	IDataStore::Statement existenceCheck = dataStore.MakeStatement
		( "SELECT rowid from Notes WHERE guid = ?"
		);
	existenceCheck->Bind(1, note.guid);
	if (existenceCheck->Execute())
	{
		IDataStore::Statement insertNote = dataStore.MakeStatement
			( "INSERT INTO Notes(guid, usn, creationDate, title, body, isDirty, notebook)"
			"  VALUES (?, ?, ?, ?, ?, ?, ?)"
			);
		insertNote->Bind(1, note.guid);
		insertNote->Bind(2, note.usn);
		insertNote->Bind(3, note.creationDate.GetTime());
		insertNote->Bind(4, note.name);
		insertNote->Bind(5, body);
		insertNote->Bind(6, note.isDirty);
		insertNote->Bind(7, notebook.guid);
		insertNote->Execute();
		insertNote->Finalize();

		__int64 rowid(dataStore.GetLastInsertRowid());

		IDataStore::Statement insertText = dataStore.MakeStatement
			( "INSERT INTO NoteText(rowid, title, body)"
			"  VALUES (?, ?, ?)"
			);
		insertText->Bind(1, rowid);
		insertText->Bind(2, note.name);
		insertText->Bind(3, bodyText);
		insertText->Execute();
		insertText->Finalize();
	}
	else
	{
		__int64 rowid(0);
		existenceCheck->Get(0, rowid);

		IDataStore::Statement updateText = dataStore.MakeStatement
			( "UPDATE NoteText"
			"  SET    title = ?, body = ?"
			"  WHERE  rowid = ?"
			);
		updateText->Bind(1, note.name);
		updateText->Bind(2, bodyText);
		updateText->Bind(3, rowid);
		updateText->Execute();
		updateText->Finalize();

		IDataStore::Statement updateNote = dataStore.MakeStatement
			( "UPDATE Notes"
			"  SET    guid = ?, usn = ?, creationDate = ?, title = ?,"
			"         body = ?, isDirty = ?, notebook = ?"
			"  WHERE  rowid = ?"
			);
		updateNote->Bind(1, note.guid);
		updateNote->Bind(2, note.usn);
		updateNote->Bind(3, note.creationDate.GetTime());
		updateNote->Bind(4, note.name);
		updateNote->Bind(5, body);
		updateNote->Bind(6, note.isDirty);
		updateNote->Bind(7, notebook.guid);
		updateNote->Bind(8, rowid);
		updateNote->Execute();
		updateNote->Finalize();
	}
}

void UserModel::AddNotebook(const Notebook & notebook)
{
	IDataStore::Statement statement = dataStore.MakeStatement
		( "INSERT OR REPLACE INTO Notebooks(guid, usn, name, isDirty, isDefault, isLastUsed)"
		"  VALUES (?, ?, ?, ?, 0, 0)"
		);
	statement->Bind(1, notebook.guid);
	statement->Bind(2, notebook.usn);
	statement->Bind(3, notebook.name);
	statement->Bind(4, notebook.isDirty);
	statement->Execute();
	statement->Finalize();
}

void UserModel::AddResource(const Resource & resource)
{
	IDataStore::Statement statement = dataStore.MakeStatement
		( "INSERT OR REPLACE INTO Resources(guid, hash, data, note)"
		"  VALUES (?, ?, ?, ?)"
		);
	statement->Bind(1, resource.Guid);
	statement->Bind(2, resource.Hash);
	statement->Bind(3, resource.Data);
	statement->Bind(4, resource.Note);
	statement->Execute();
	statement->Finalize();
}

void UserModel::AddTag(const Tag & tag)
{
	IDataStore::Statement statement = dataStore.MakeStatement
		( "INSERT OR REPLACE INTO Tags(guid, usn, name, isDirty)"
		"  VALUES (?, ?, ?, ?)"
		);
	statement->Bind(1, tag.guid);
	statement->Bind(2, tag.usn);
	statement->Bind(3, tag.name);
	statement->Bind(4, tag.isDirty);
	statement->Execute();
	statement->Finalize();
}

void UserModel::AddTagToNote
	( const wstring & tagName
	, const Note    & note
	)
{
	IDataStore::Statement statement = dataStore.MakeStatement
		( "INSERT OR REPLACE INTO NoteTags(note, tag)"
			" SELECT ?, guid"
			" FROM   tags"
			" WHERE  upper(name) = upper(?)"
		);
	statement->Bind(1, note.guid);
	statement->Bind(2, tagName);
	statement->Execute();
	statement->Finalize();
}

void UserModel::BeginTransaction()
{
	IDataStore::Statement statement = dataStore.MakeStatement
		( "BEGIN TRANSACTION"
		);
	statement->Execute();
	statement->Finalize();
}

void UserModel::ConnectLoaded(slot_type OnLoaded)
{
	SignalLoaded.connect(OnLoaded);
}

void UserModel::DeleteNote(const Guid & note)
{
	Transaction transaction(*this);
	__int64 rowid(0L);
	{
		IDataStore::Statement statement = dataStore.MakeStatement
			( "SELECT rowid FROM Notes Where guid = ?"
			);
		statement->Bind(1, note);
		if (statement->Execute())
			throw std::exception("Could not find the note to delete.");
		statement->Get(0, rowid);
	}
	{
		IDataStore::Statement statement = dataStore.MakeStatement
			( "DELETE FROM Notes WHERE rowid = ?"
			);
		statement->Bind(1, rowid);
		statement->Execute();
	}
	{
		IDataStore::Statement statement = dataStore.MakeStatement
			( "DELETE FROM NoteText WHERE rowid = ?"
			);
		statement->Bind(1, rowid);
		statement->Execute();
	}
}

void UserModel::DeleteNotebook(const Guid & notebook)
{
	Transaction transaction(*this);

	bool isDefault, isLastUsed;
	{
		IDataStore::Statement statement = dataStore.MakeStatement
			( "SELECT isDefault, isLastUsed FROM Notebooks WHERE guid = ? LIMIT 1"
			);
		statement->Bind(1, notebook);
		if (statement->Execute())
			return;
		statement->Get(0, isDefault);
		statement->Get(1, isLastUsed);
	}

	if (isDefault)
		throw std::exception("Cannot delete the default notebook.");

	{
		IDataStore::Statement statement = dataStore.MakeStatement
			( "DELETE FROM Notebooks WHERE guid = ?"
			);
		statement->Bind(1, notebook);
		statement->Execute();
		statement->Finalize();
	}

	if (isLastUsed)
	{
		Notebook notebook;
		GetDefaultNotebook(notebook);
		MakeNotebookLastUsed(notebook);
	}
}

void UserModel::DeleteTag(const Guid & tag)
{
	IDataStore::Statement statement = dataStore.MakeStatement
		( "DELETE FROM Tags WHERE guid = ?"
		);
	statement->Bind(1, tag);
	statement->Execute();
	statement->Finalize();
}

void UserModel::EndTransaction()
{
	IDataStore::Statement statement = dataStore.MakeStatement
		( "END TRANSACTION"
		);
	statement->Execute();
	statement->Finalize();
}

bool UserModel::Exists(const wstring & username)
{
	wstring path(CreatePathFromName(username));
	return INVALID_FILE_ATTRIBUTES != ::GetFileAttributes(path.c_str());
}


void UserModel::GetCredentials(Credentials & credentials)
{
	wstring password;
	wstring username;
	GetProperty(L"password", password);
	GetProperty(L"username", username);
	credentials.SetPassword(password);
	credentials.SetUsername(username);
}

void UserModel::GetDefaultNotebook(Notebook & notebook)
{
	IDataStore::Statement statement = dataStore.MakeStatement
		( "SELECT guid, usn, name, isDirty"
		"  FROM Notebooks"
		"  WHERE isDefault = 1"
		"  LIMIT 1"
		);
	if (statement->Execute())
		throw std::exception("No default notebook.");
	wstring guidString;
	statement->Get(0, guidString);
	statement->Get(1, notebook.usn);
	statement->Get(2, notebook.name);
	statement->Get(3, notebook.isDirty);
	notebook.guid = guidString;
}

int UserModel::GetDirtyNoteCount(const Notebook & notebook)
{
	IDataStore::Statement statement = dataStore.MakeStatement
		( "SELECT Count(*)"
		"  FROM Notes"
		"  WHERE isDirty = 1 AND notebook = ?"
		);
	statement->Bind(1, notebook.guid);
	if (statement->Execute())
		throw std::exception("Could not count dirty notes.");
	int count;
	statement->Get(0, count);
	return count;
}

wstring UserModel::GetFolder() const
{
	return folder;
}

__int64 UserModel::GetLastSyncEnTime()
{
	__int64 enTime(0);
	GetProperty(L"lastSyncTime", enTime);
	return enTime;
}

void UserModel::GetLastUsedNotebook(Notebook & notebook)
{
	IDataStore::Statement statement = dataStore.MakeStatement
		( "SELECT guid, usn, name, isDirty"
		"  FROM Notebooks"
		"  WHERE isLastUsed = 1"
		"  LIMIT 1"
		);
	if (statement->Execute())
		throw std::exception("No last used notebook.");
	wstring guidString;
	statement->Get(0, guidString);
	statement->Get(1, notebook.usn);
	statement->Get(2, notebook.name);
	statement->Get(3, notebook.isDirty);
	notebook.guid = guidString;
}

Note UserModel::GetNote(Guid guid)
{
	IDataStore::Statement statement = dataStore.MakeStatement
		( "SELECT title, usn, creationDate, isDirty"
		"  FROM Notes"
		"  WHERE guid = ?"
		"  LIMIT 1"
		);
	statement->Bind(1, guid);
	if (statement->Execute())
		throw std::exception("Note not found.");
	wstring title;
	int     usn;
	int     creationDate;
	bool    isDirty;
	statement->Get(0, title);
	statement->Get(1, usn);
	statement->Get(2, creationDate);
	statement->Get(3, isDirty);
	Note note;
	note.guid         = guid;
	note.name         = title;
	note.usn          = usn;
	note.creationDate = creationDate;
	note.isDirty      = isDirty;
	return note;
}

void UserModel::GetNoteBody
	( const Guid & guid
	, wstring    & body
	)
{
	IDataStore::Statement statement = dataStore.MakeStatement
		( "SELECT body"
		"  FROM Notes"
		"  WHERE guid = ?"
		"  LIMIT 1"
		);
	statement->Bind(1, guid);
	if (statement->Execute())
		throw std::exception("Note not found.");
	statement->Get(0, body);
}

void UserModel::GetNoteResources
	( const Guid   & note
	, vector<Guid> & resources
	)
{
	IDataStore::Statement statement = dataStore.MakeStatement
		( "SELECT Resources.guid"
		"  FROM Resources"
		"  WHERE note = ?"
		);
	statement->Bind(1, note);
	while (!statement->Execute())
	{
		string guid;
		statement->Get(0, guid);
		resources.push_back(guid);
	}
}

void UserModel::GetNoteTags
	( const Note & note
	, TagList    & tags
	)
{
	IDataStore::Statement statement = dataStore.MakeStatement
		( "SELECT guid, usn, name, isDirty"
		"  FROM Tags, NoteTags"
		"  WHERE note = ? AND tag = guid"
		"  ORDER BY name"
		);
	statement->Bind(1, note.guid);
	while (!statement->Execute())
	{
		string  guid;
		tags.push_back(Tag());
		statement->Get(0, guid);
		statement->Get(1, tags.back().usn);
		statement->Get(2, tags.back().name);
		statement->Get(3, tags.back().isDirty);
		tags.back().guid = guid;
	}
}

void UserModel::GetNotebook
	( const Guid & guid
	, Notebook   & notebook
	)
{
	IDataStore::Statement statement = dataStore.MakeStatement
		( "SELECT guid, usn, name, isDirty"
		"  FROM   Notebooks"
		"  WHERE  guid = ?"
		);
	statement->Bind(1, guid);
	if (statement->Execute())
		throw std::exception("Notebook not found.");
	wstring guidString;
	statement->Get(0, guidString);
	statement->Get(1, notebook.usn);
	statement->Get(2, notebook.name);
	statement->Get(3, notebook.isDirty);
	notebook.guid = guidString;
}

int UserModel::GetNotebookUpdateCount(const Guid & notebook)
{
	IDataStore::Statement statement = dataStore.MakeStatement
		( "SELECT updateCount"
		"  FROM   notebooks"
		"  WHERE  guid = ?"
		);
	statement->Bind(1, notebook);
	if (statement->Execute())
		throw std::exception("Notebook not found.");
	int updateCount(0);
	statement->Get(0, updateCount);
	return updateCount;
}

void UserModel::GetNoteThumbnail(const Guid & guid, Thumbnail & thumbnail)
{
	IDataStore::Statement statement = dataStore.MakeStatement
		( "SELECT rowid, thumbnailWidth, thumbnailHeight"
		"  FROM Notes"
		"  WHERE guid = ?"
		"  LIMIT 1"
		);
	statement->Bind(1, guid);
	if (statement->Execute())
		throw std::exception("Note not found.");

	__int64 row(0);
	statement->Get(0, row);
	statement->Get(1, thumbnail.Width);
	statement->Get(2, thumbnail.Height);

	if (thumbnail.Width > 0 && thumbnail.Height > 0)
	{
		IDataStore::Blob sqlBlob = dataStore.MakeBlob
			( "Notes"
			, "thumbnail"
			, row
			);
		sqlBlob->Read(thumbnail.Data);
	}
	else
	{
		thumbnail.Data.clear();
	}
}

void UserModel::GetNotebooks(NotebookList & notebooks)
{
	IDataStore::Statement statement = dataStore.MakeStatement
		( "SELECT guid, usn, name, isDirty"
		"  FROM Notebooks"
		"  ORDER BY name"
		);
	while (!statement->Execute())
	{
		notebooks.push_back(Notebook());
		wstring guidString;
		statement->Get(0, guidString);
		statement->Get(1, notebooks.back().usn);
		statement->Get(2, notebooks.back().name);
		statement->Get(3, notebooks.back().isDirty);
		notebooks.back().guid = guidString;
	}
}

void UserModel::GetNotesByNotebook
	( const Notebook & notebook
	, NoteList       & notes
	)
{
	IDataStore::Statement statement = dataStore.MakeStatement
		( "SELECT guid, usn, title, creationDate, isDirty"
		"  FROM Notes"
		"  WHERE notebook = ?"
		"  ORDER BY creationDate"
		);
	statement->Bind(1, notebook.guid);
	while (!statement->Execute())
	{
		string  guid;
		int     usn;
		wstring title;
		int     creationDate;
		bool    isDirty;
		statement->Get(0, guid);
		statement->Get(1, usn);
		statement->Get(2, title);
		statement->Get(3, creationDate);
		statement->Get(4, isDirty);
		notes.push_back(Note());
		notes.back().guid         = guid;
		notes.back().usn          = usn;
		notes.back().name         = title;
		notes.back().creationDate = creationDate;
		notes.back().isDirty      = isDirty;
	}
}

void UserModel::GetNotesBySearch
	( const std::wstring & search
	, NoteList           & notes
	)
{
	IDataStore::Statement statement = dataStore.MakeStatement
		( "SELECT n.guid, n.usn, n.title, n.creationDate, n.isDirty"
		"  FROM   Notes as n, NoteText"
		"  WHERE  n.rowid = NoteText.rowid AND NoteText MATCH ?"
		"  ORDER  BY n.creationDate"
		);
	statement->Bind(1, search);
	while (!statement->Execute())
	{
		string  guid;
		int     usn;
		wstring title;
		int     creationDate;
		bool    isDirty;
		statement->Get(0, guid);
		statement->Get(1, usn);
		statement->Get(2, title);
		statement->Get(3, creationDate);
		statement->Get(4, isDirty);
		notes.push_back(Note());
		notes.back().guid         = guid;
		notes.back().usn          = usn;
		notes.back().name         = title;
		notes.back().creationDate = creationDate;
		notes.back().isDirty      = isDirty;
	}
}

void UserModel::GetResource
	( const string & hash
	, Blob         & blob
	)
{
	IDataStore::Statement statement = dataStore.MakeStatement
		( "SELECT rowid, data"
		"  FROM   Resources"
		"  WHERE  hash = ?"
		"  LIMIT  1"
		);
	statement->Bind(1, hash);
	if (statement->Execute())
		throw std::exception("Image resource not found.");
	__int64 row(0);
	statement->Get(0, row);

	if (statement->IsNull(1))
	{
		blob.clear();
	}
	else
	{
		IDataStore::Blob sqlBlob = dataStore.MakeBlob
			( "Resources"
			, "data"
			, row
			);
		sqlBlob->Read(blob);
	}
}

void UserModel::GetResource
	( const Guid & guid
	, Resource   & resource
	)
{
	IDataStore::Statement statement = dataStore.MakeStatement
		( "SELECT rowid, hash, note, data"
		"  FROM   Resources"
		"  WHERE  guid = ?"
		);
	statement->Bind(1, guid);
	if (statement->Execute())
		throw std::exception("Resource not found.");

	__int64 row(0);
	wstring noteGuid;
	statement->Get(0, row);
	statement->Get(1, resource.Hash);
	statement->Get(2, noteGuid);

	resource.Note = noteGuid;

	if (statement->IsNull(3))
	{
		resource.Data.clear();
	}
	else
	{
		IDataStore::Blob sqlBlob = dataStore.MakeBlob
			( "Resources"
			, "data"
			, row
			);
		sqlBlob->Read(resource.Data);
	}

	resource.Guid = guid;
}

void UserModel::GetTags(TagList & tags)
{
	IDataStore::Statement statement = dataStore.MakeStatement
		( "SELECT guid, usn, name, isDirty"
		"  FROM Tags"
		"  ORDER BY name"
		);
	while (!statement->Execute())
	{
		wstring guid;
		wstring name;
		int     usn;
		bool    isDirty;
		statement->Get(0, guid);
		statement->Get(1, usn);
		statement->Get(2, name);
		statement->Get(3, isDirty);
		tags.push_back(Tag());
		tags.back().guid    = guid;
		tags.back().usn     = usn;
		tags.back().name    = name;
		tags.back().isDirty = isDirty;
	}
}

int UserModel::GetUpdateCount()
{
	int updateCount(0);
	GetProperty(L"updateCount", updateCount);
	return updateCount;
}

void UserModel::Load(const wstring & username)
{
	wstring path = CreatePathFromName(username);
	if (!TryLoad(path))
		throw std::exception("Database could not be loaded.");
	Update();
	SignalLoaded();
}

void UserModel::LoadAs
	( const std::wstring & oldUsername
	, const std::wstring & newUsername
	)
{
	wstring oldPath = CreatePathFromName(oldUsername);
	wstring newPath = CreatePathFromName(newUsername);
	if (!::MoveFile(oldPath.c_str(), newPath.c_str()))
		throw std::exception("Database could not be renamed.");
	if (!TryLoad(newPath))
		throw std::exception("Database could not be loaded.");
	Update();
	SetProperty(L"username", newUsername);
	SignalLoaded();
}

void UserModel::LoadOrCreate(const wstring & username)
{
	wstring path = CreatePathFromName(username);
	if (TryLoad(path))
	{
		if (GetVersion() != 0)
			throw std::exception("Incorrect database version.");
	}
	else
	{
		Create(path);
		Initialize(username);
	}
	Update();
	SignalLoaded();
}

void UserModel::MakeNotebookDefault(const Notebook & notebook)
{
	IDataStore::Statement removeOld = dataStore.MakeStatement
		( "UPDATE Notebooks"
		"  SET isDefault = 0"
		"  WHERE isDefault = 1"
		);
	removeOld->Execute();

	IDataStore::Statement setNew = dataStore.MakeStatement
		( "UPDATE Notebooks"
		"  SET isDefault = 1"
		"  WHERE guid = ?"
		);
	setNew->Bind(1, notebook.guid);
	setNew->Execute();
}

void UserModel::MakeNotebookLastUsed(const Notebook & notebook)
{
	IDataStore::Statement removeOld = dataStore.MakeStatement
		( "UPDATE Notebooks"
		"  SET isLastUsed = 0"
		"  WHERE isLastUsed = 1"
		);
	removeOld->Execute();

	IDataStore::Statement setNew = dataStore.MakeStatement
		( "UPDATE Notebooks"
		"  SET isLastUsed = 1"
		"  WHERE guid = ?"
		);
	setNew->Bind(1, notebook.guid);
	setNew->Execute();
}

void UserModel::SetCredentials
	( const wstring & username
	, const wstring & password
	)
{
	SetProperty(L"username", username);
	SetProperty(L"password", password);
}

void UserModel::SetLastSyncEnTime(__int64 enTime)
{
	SetProperty(L"lastSyncTime", enTime);
}

void UserModel::SetNotebookUpdateCount
	( const Guid & notebook
	, int          updateCount
	)
{
	IDataStore::Statement statement = dataStore.MakeStatement
		( "UPDATE Notebooks"
		"  SET    updateCount = ?"
		"  WHERE  guid = ?"
		);
	statement->Bind(1, updateCount);
	statement->Bind(2, notebook);
	statement->Execute();
}

void UserModel::SetNoteThumbnail(const Guid & guid, const Thumbnail & thumbnail)
{
	IDataStore::Statement statement = dataStore.MakeStatement
		( "UPDATE Notes"
		"  SET    thumbnail = ?, thumbnailWidth = ?, thumbnailHeight = ?"
		"  WHERE  guid = ?"
		);
	statement->Bind(1, thumbnail.Data);
	statement->Bind(2, thumbnail.Width);
	statement->Bind(3, thumbnail.Height);
	statement->Bind(4, guid);
	statement->Execute();
}

void UserModel::SetUpdateCount(int updateCount)
{
	SetProperty(L"updateCount", updateCount);
}

void UserModel::Unload()
{
	dataStore.Close();
}

void UserModel::UpdateNotebook
	( const Notebook & notebook
	, const Notebook & replacement
	)
{
	IDataStore::Statement statement = dataStore.MakeStatement
		( "UPDATE Notebooks"
		"  SET guid = ?, usn = ?, name = ?, isDirty = ?"
		"  WHERE guid = ?"
		);
	statement->Bind(1, replacement.guid);
	statement->Bind(2, replacement.usn);
	statement->Bind(3, replacement.name);
	statement->Bind(4, replacement.isDirty);
	statement->Bind(5, notebook.guid);
	statement->Execute();
}

void UserModel::UpdateTag
	( const Tag & tag
	, const Tag & replacement
	)
{
	IDataStore::Statement statement = dataStore.MakeStatement
		( "UPDATE Tags"
		"  SET guid = ?, usn = ?, name = ?, isDirty = ?"
		"  WHERE guid = ?"
		);
	statement->Bind(1, replacement.guid);
	statement->Bind(2, replacement.usn);
	statement->Bind(3, replacement.name);
	statement->Bind(4, replacement.isDirty);
	statement->Bind(5, tag.guid);
	statement->Execute();
}

//------------------
// utility functions
//------------------

void UserModel::Create(wstring path)
{
	int flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
	if (!dataStore.Create(path, flags))
		throw std::exception("Database could not be created.");
}

wstring UserModel::CreatePathFromName(wstring name)
{
	// TODO: filter name characters
	wstringstream stream;
	stream << folder << L'\\' << name << L".db";
	return stream.str();
}

void UserModel::CreateTable(const char * sql)
{
	dataStore.MakeStatement(sql)->Execute();
}

void UserModel::Initialize(wstring name)
{
	CreateTable
		( "CREATE TABLE Properties"
			"( key PRIMARY KEY"
			", value NOT NULL"
			")"
		);
	SetProperty(L"version",      0);
	SetProperty(L"username",     name);
	SetProperty(L"password",     L"");
	SetProperty(L"lastSyncTime", 0);
	SetProperty(L"updateCount",  0);

	CreateTable
		( "CREATE TABLE Notebooks"
			"( guid PRIMARY KEY"
			", usn"
			", name"
			", updateCount"
			", isDirty"
			", isDefault"
			", isLastUsed"
			")"
		);

	CreateTable
		( "CREATE VIRTUAL TABLE NoteText USING fts3"
			"( title"
			", body"
			")"
		);

	CreateTable
		( "CREATE TABLE Notes"
			"( guid PRIMARY KEY"
			", usn"
			", creationDate"
			", title"
			", body"
			", isDirty"
			", thumbnail"
			", thumbnailWidth  DEFAULT 0"
			", thumbnailHeight DEFAULT 0"
			", notebook REFERENCES Notebooks(guid)  ON DELETE CASCADE ON UPDATE CASCADE"
			")"
		);

	CreateTable
		( "CREATE TABLE Resources"
			"( guid PRIMARY KEY"
			", hash UNIQUE"
			", data"
			", note REFERENCES Notes(guid) ON DELETE CASCADE ON UPDATE CASCADE"
			")"
		);

	CreateTable
		( "CREATE TABLE Tags"
			"( guid PRIMARY KEY"
			", usn"
			", name"
			", isDirty"
			")"
		);

	CreateTable
		( "CREATE TABLE NoteTags"
			"( note REFERENCES Notes(guid)"
			", tag  REFERENCES Tags(guid)"
			")"
		);
}

void UserModel::SetPragma(const char * sql)
{
	dataStore.MakeStatement(sql)->Execute();
}

bool UserModel::TryLoad(wstring path)
{
	int flags = SQLITE_OPEN_READWRITE;
	return dataStore.Create(path, flags);
}

void UserModel::Update()
{
	SetPragma("PRAGMA foreign_keys = ON");
	Transaction transaction(*this);
	if (GetNotebookCount() == 0)
	{
		Notebook notebook;
		notebook.name    = L"Notes";
		notebook.isDirty = true;
		notebook.usn     = GetUpdateCount();
		AddNotebook(notebook);
		MakeNotebookDefault(notebook);
		MakeNotebookLastUsed(notebook);
	}
}