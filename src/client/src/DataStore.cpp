#include "stdafx.h"
#include "DataStore.h"

#include "Note.h"
#include "Notebook.h"
#include "SqlStatement.h"
#include "Tools.h"

#include <map>
#include <sstream>

using namespace boost;
using namespace std;
using namespace Tools;

//----------
// interface
//----------

DataStore::DataStore(wstring folder)
	: db     (NULL)
	, folder (folder)
{
}

DataStore::~DataStore()
{
	Disconnect();
}

Notebook & DataStore::GetDefaultNotebook()
{
	SqlStatement statement
		( db
		, "SELECT guid, name"
		"  FROM Notebooks"
		"  WHERE isDefault = 1 LIMIT 1"
		);

	if (statement.Execute())
		throw std::exception("No default notebook.");
	wstring guid;
	wstring name;
	statement.Get(0, guid);
	statement.Get(1, name);

	defaultNotebook = make_shared<Notebook>(guid, name);
	return *defaultNotebook;
}

wstring DataStore::GetUser()
{
	return GetProperty(L"user");
}

int DataStore::GetVersion()
{
	return _wtoi(GetProperty(L"version").c_str());
}

//--------------------------
// IDataStore implementation
//--------------------------

void DataStore::AddNote(const Note & note, const Notebook & notebook)
{
	SqlStatement insertContents
		( db
		, "INSERT INTO NoteContents(title) VALUES (?)"
		);
	insertContents.Bind(1, note.GetTitle());
	insertContents.Execute();

	SqlStatement insertInfo
		( db
		, "INSERT INTO Notes(guid, creationDate, content, notebook)"
		"  VALUES (?, ?, ?, ?)"
		);
	insertInfo.Bind(1, note.GetGuid());
	insertInfo.Bind(2, note.GetCreationDate().GetTime());
	insertInfo.Bind(3, GetLastInsertRowid());
	insertInfo.Bind(4, notebook.GetGuid());
	insertInfo.Execute();
}

void DataStore::AddNotebook(const Notebook & notebook)
{
	SqlStatement statement
		( db
		, "INSERT INTO Notebooks(guid, name, isDefault, isLastUsed)"
		"  VALUES (?, ?, 0, 0)"
		);
	statement.Bind(1, notebook.GetGuid());
	statement.Bind(2, notebook.GetName());
	statement.Execute();
}

Notebook & DataStore::GetLastUsedNotebook()
{
	SqlStatement statement
		( db
		, "SELECT guid, name"
		"  FROM Notebooks"
		"  WHERE isLastUsed = 1"
		"  LIMIT 1"
		);
	if (statement.Execute())
		throw std::exception("No last used notebook.");
	wstring guid;
	wstring name;
	statement.Get(0, guid);
	statement.Get(1, name);
	defaultNotebook = make_shared<Notebook>(guid, name);
	return *defaultNotebook;
}

int DataStore::GetNotebookCount()
{
	SqlStatement statement
		( db
		, "SELECT COUNT(*)"
		"  FROM Notebooks"
		);
	statement.Execute();
	int count(0);
	statement.Get(0, count);
	return count;
}

const NotebookList & DataStore::GetNotebooks()
{
	SqlStatement statement
		( db
		, "SELECT guid, name"
		"  FROM Notebooks"
		"  ORDER BY name"
		);
	notebooks.clear();
	while (!statement.Execute())
	{
		wstring guid;
		wstring name;
		statement.Get(0, guid);
		statement.Get(1, name);
		notebooks.push_back(Notebook(guid, name));
	}
	return notebooks;
}

const NoteList & DataStore::GetNotesByNotebook(const Notebook & notebook)
{
	SqlStatement statement
		( db
		, "SELECT guid, title, creationDate"
		"  FROM Notes, NoteContents"
		"  WHERE content = docid AND notebook = ?"
		"  ORDER BY creationDate"
		);
	statement.Bind(1, notebook.GetGuid());
	while (!statement.Execute())
	{
		string  guid;
		wstring title;
		int     creationDate;
		statement.Get(0, guid);
		statement.Get(1, title);
		statement.Get(2, creationDate);
		notes.push_back(Note(Guid(guid), title, Timestamp(creationDate)));
	}
	return notes;
}

const NoteList & DataStore::GetNotesBySearch(wstring search)
{
	SqlStatement statement
		( db
		, "SELECT guid, title, creationDate"
		"  FROM Notes, NoteContents"
		"  WHERE content = docid AND NoteContents MATCH ?"
		"  ORDER BY creationDate"
		);
	statement.Bind(1, search);
	notes.clear();
	while (!statement.Execute())
	{
		string  guid;
		wstring title;
		int     creationDate;
		statement.Get(0, guid);
		statement.Get(1, title);
		statement.Get(2, creationDate);
		notes.push_back(Note(Guid(guid), title, Timestamp(creationDate)));
	}
	return notes;
}

void DataStore::LoadOrCreate(wstring name)
{
	path = CreatePathFromName(name);
	if (TryLoad())
	{
		if (GetVersion() != 0)
			throw std::exception("Incorrect database version.");
	}
	else
	{
		Create();
		Initialize(name);
	}
}

void DataStore::MakeNotebookDefault(const Notebook & notebook)
{
	SqlStatement removeOld
		( db
		, "UPDATE Notebooks"
		"  SET isDefault = 0"
		"  WHERE isDefault = 1"
		);
	removeOld.Execute();

	SqlStatement setNew
		( db
		, "UPDATE Notebooks"
		"  SET isDefault = 1"
		"  WHERE guid = ?"
		);
	setNew.Bind(1, notebook.GetGuid());
	setNew.Execute();
}

void DataStore::MakeNotebookLastUsed(const Notebook & notebook)
{
	SqlStatement removeOld
		( db
		, "UPDATE Notebooks"
		"  SET isLastUsed = 0"
		"  WHERE isLastUsed = 1"
		);
	removeOld.Execute();

	SqlStatement setNew
		( db
		, "UPDATE Notebooks"
		"  SET isLastUsed = 1"
		"  WHERE guid = ?"
		);
	setNew.Bind(1, notebook.GetGuid());
	setNew.Execute();
}

//------------------
// utility functions
//------------------

void DataStore::AddProperty(wstring key, wstring value)
{
	SqlStatement statement
		( db
		, "INSERT INTO Properties VALUES (?, ?)"
		);
	statement.Bind(1, key);
	statement.Bind(2, value);
	statement.Execute();
}

void DataStore::Create()
{
	assert(db == NULL);
	vector<unsigned char> utf8Path = ConvertToUtf8(path);
	assert(!utf8Path.empty());
	int flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
	int result = sqlite3_open_v2
		( reinterpret_cast<const char *>(&utf8Path[0])
		, &db
		, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE
		, NULL
		);
	if (SQLITE_OK != result)
	{
		const char * errorMessage = sqlite3_errmsg(db);
		sqlite3_close(db);
		db = NULL;
		throw std::exception(errorMessage);
	}
	SetPragma("PRAGMA foreign_keys = ON");
}

wstring DataStore::CreatePathFromName(wstring name)
{
	wstringstream stream;
	stream << folder << L'\\' << name << L".db";
	return stream.str();
}

void DataStore::CreateTable(const char * sql)
{
	SqlStatement statement(db, sql);
	statement.Execute();
}

void DataStore::Disconnect()
{
	if (db == NULL)
		return;
	sqlite3_close(db);
	db = NULL;
}

std::wstring DataStore::GetProperty(std::wstring key)
{
	SqlStatement statement
		( db
		, "SELECT value"
		"  FROM Properties"
		"  WHERE key = ?"
		"  LIMIT 1"
		);
	statement.Bind(1, key);
	if (statement.Execute())
		throw std::exception("Property not found.");
	wstring value;
	statement.Get(0, value);
	return value;
}

void DataStore::Initialize(wstring name)
{
	CreateTable
		( "CREATE TABLE Properties"
			"( key PRIMARY KEY"
			", value NOT NULL"
			")"
		);
	AddProperty(L"version", L"0");
	AddProperty(L"user",    name);

	CreateTable
		( "CREATE TABLE Notebooks"
			"( guid PRIMARY KEY"
			", name"
			", isDefault"
			", isLastUsed"
			")"
		);

	CreateTable
		( "CREATE VIRTUAL TABLE NoteContents USING fts3"
			"( title"
			", body"
			")"
		);

	CreateTable
		( "CREATE TABLE Notes"
			"( guid PRIMARY KEY"
			", creationDate"
			", content"
			", notebook REFERENCES Notebooks"
			")"
		);
}

void DataStore::SetPragma(const char * sql)
{
	SqlStatement statement(db, sql);
	statement.Execute();
}

bool DataStore::TryLoad()
{
	assert(db == NULL);
	vector<unsigned char> utf8Path = ConvertToUtf8(path);
	assert(!utf8Path.empty());
	int flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
	int result = sqlite3_open_v2
		( reinterpret_cast<const char *>(&utf8Path[0])
		, &db
		, SQLITE_OPEN_READWRITE
		, NULL
		);
	if (SQLITE_OK != result)
	{
		sqlite3_close(db);
		db = NULL;
		return false;
	}
	return true;
}

//----------------
// SQLite wrappers
//----------------

__int64 DataStore::GetLastInsertRowid()
{
	return sqlite3_last_insert_rowid(db);
}
