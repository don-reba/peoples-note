#include "stdafx.h"
#include "UserModel.h"

#include "IDataStore.h"
#include "ISqlBlob.h"
#include "ISqlStatement.h"
#include "Notebook.h"

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

wstring UserModel::GetUser()
{
	return GetProperty(L"user");
}

int UserModel::GetVersion()
{
	return _wtoi(GetProperty(L"version").c_str());
}

//--------------------------
// IUuserModel implementaion
//--------------------------

void UserModel::AddImageResource
	( std::string  hash
	, const Blob & data
	, Guid         note
	)
{
	IDataStore::Statement statement = dataStore.MakeStatement
		( "INSERT INTO ImageResources(hash, data, note) VALUES (?, ?, ?)"
		);
	statement->Bind(1, hash);
	statement->Bind(2, data);
	statement->Bind(3, note);
	statement->Execute();
	statement->Finalize();
}

void UserModel::AddNote
	( const Note     & note
	, const wstring  & body
	, const wstring  & bodyText
	, const Notebook & notebook
	)
{
	IDataStore::Statement insertContents = dataStore.MakeStatement
		( "INSERT INTO NoteContents(titleText, bodyText) VALUES (?, ?)"
		);
	insertContents->Bind(1, note.GetTitle());
	insertContents->Bind(2, bodyText);
	insertContents->Execute();
	insertContents->Finalize();

	IDataStore::Statement insertInfo = dataStore.MakeStatement
		( "INSERT INTO Notes(guid, creationDate, title, body, search, notebook)"
		"  VALUES (?, ?, ?, ?, ?, ?)"
		);
	insertInfo->Bind(1, note.GetGuid());
	insertInfo->Bind(2, note.GetCreationDate().GetTime());
	insertInfo->Bind(3, note.GetTitle());
	insertInfo->Bind(4, body);
	insertInfo->Bind(5, dataStore.GetLastInsertRowid());
	insertInfo->Bind(6, notebook.GetGuid());
	insertInfo->Execute();
	insertInfo->Finalize();
}

void UserModel::AddNotebook(const Notebook & notebook)
{
	IDataStore::Statement statement = dataStore.MakeStatement
		( "INSERT INTO Notebooks(guid, name, isDefault, isLastUsed)"
		"  VALUES (?, ?, 0, 0)"
		);
	statement->Bind(1, notebook.GetGuid());
	statement->Bind(2, notebook.GetName());
	statement->Execute();
	statement->Finalize();
}

void UserModel::ConnectLoaded(slot_type OnLoaded)
{
	SignalLoaded.connect(OnLoaded);
}

void UserModel::CreateDefaultUser()
{
	credentialsModel.SetCredentials(L"", L"");
}

ICredentialsModel & UserModel::GetCredentials()
{
	return credentialsModel;
}

Notebook UserModel::GetDefaultNotebook()
{
	IDataStore::Statement statement = dataStore.MakeStatement
		( "SELECT guid, name"
		"  FROM Notebooks"
		"  WHERE isDefault = 1"
		"  LIMIT 1"
		);
	if (statement->Execute())
		throw std::exception("No default notebook.");
	wstring guid;
	wstring name;
	statement->Get(0, guid);
	statement->Get(1, name);
	return Notebook(guid, name);
}

void UserModel::GetImageResource(string hash, Blob & blob)
{
	IDataStore::Statement statement = dataStore.MakeStatement
		( "SELECT rowid"
		"  FROM   ImageResources"
		"  WHERE  hash = ?"
		"  LIMIT  1"
		);
	statement->Bind(1, hash);
	if (statement->Execute())
		throw std::exception("Image resource not found.");
	__int64 row(0);
	statement->Get(0, row);

	IDataStore::Blob sqlBlob = dataStore.MakeBlob
		( "ImageResources"
		, "data"
		, row
		);
	sqlBlob->Read(blob);
}

Notebook UserModel::GetLastUsedNotebook()
{
	IDataStore::Statement statement = dataStore.MakeStatement
		( "SELECT guid, name"
		"  FROM Notebooks"
		"  WHERE isLastUsed = 1"
		"  LIMIT 1"
		);
	if (statement->Execute())
		throw std::exception("No last used notebook.");
	wstring guid;
	wstring name;
	statement->Get(0, guid);
	statement->Get(1, name);
	return Notebook(guid, name);
}

Note UserModel::GetNote(Guid guid)
{
	IDataStore::Statement statement = dataStore.MakeStatement
		( "SELECT title, creationDate"
		"  FROM Notes"
		"  WHERE guid = ?"
		"  LIMIT 1"
		);
	statement->Bind(1, guid);
	if (statement->Execute())
		throw std::exception("Note not found.");
	wstring title;
	int     creationDate;
	statement->Get(0, title);
	statement->Get(1, creationDate);
	return Note(guid, title, Timestamp(creationDate));
}

void UserModel::GetNoteBody(Guid guid, wstring & body)
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

const NotebookList & UserModel::GetNotebooks()
{
	IDataStore::Statement statement = dataStore.MakeStatement
		( "SELECT guid, name"
		"  FROM Notebooks"
		"  ORDER BY name"
		);
	notebooks.clear();
	while (!statement->Execute())
	{
		wstring guid;
		wstring name;
		statement->Get(0, guid);
		statement->Get(1, name);
		notebooks.push_back(Notebook(guid, name));
	}
	return notebooks;
}

const NoteList & UserModel::GetNotesByNotebook(const Notebook & notebook)
{
	IDataStore::Statement statement = dataStore.MakeStatement
		( "SELECT guid, title, creationDate"
		"  FROM Notes"
		"  WHERE notebook = ?"
		"  ORDER BY creationDate"
		);
	statement->Bind(1, notebook.GetGuid());
	notes.clear();
	while (!statement->Execute())
	{
		string  guid;
		wstring title;
		int     creationDate;
		statement->Get(0, guid);
		statement->Get(1, title);
		statement->Get(2, creationDate);
		notes.push_back(Note(Guid(guid), title, Timestamp(creationDate)));
	}
	return notes;
}

const NoteList & UserModel::GetNotesBySearch(wstring search)
{
	IDataStore::Statement statement = dataStore.MakeStatement
		( "SELECT guid, title, creationDate"
		"  FROM Notes, NoteContents"
		"  WHERE search = docid AND NoteContents MATCH ?"
		"  ORDER BY creationDate"
		);
	statement->Bind(1, search);
	notes.clear();
	while (!statement->Execute())
	{
		string  guid;
		wstring title;
		int     creationDate;
		statement->Get(0, guid);
		statement->Get(1, title);
		statement->Get(2, creationDate);
		notes.push_back(Note(Guid(guid), title, Timestamp(creationDate)));
	}
	return notes;
}

void UserModel::Load()
{
	LoadOrCreate(credentialsModel.GetUsername());
	if (GetNotebookCount() == 0)
	{
		Notebook notebook(Guid(), L"Notes");
		AddNotebook(notebook);
		MakeNotebookDefault(notebook);
		MakeNotebookLastUsed(notebook);
	}
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
	setNew->Bind(1, notebook.GetGuid());
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
	setNew->Bind(1, notebook.GetGuid());
	setNew->Execute();
}

void UserModel::SetCredentials(const ICredentialsModel & credentials)
{
	credentialsModel.SetCredentials
		( credentials.GetUsername()
		, credentials.GetPassword()
		);
}

//------------------
// utility functions
//------------------

void UserModel::AddProperty(wstring key, wstring value)
{
	IDataStore::Statement statement = dataStore.MakeStatement
		( "INSERT INTO Properties VALUES (?, ?)"
		);
	statement->Bind(1, key);
	statement->Bind(2, value);
	statement->Execute();
}

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

std::wstring UserModel::GetProperty(std::wstring key)
{
	IDataStore::Statement statement = dataStore.MakeStatement
		( "SELECT value"
		"  FROM Properties"
		"  WHERE key = ?"
		"  LIMIT 1"
		);
	statement->Bind(1, key);
	if (statement->Execute())
		throw std::exception("Property not found.");
	wstring value;
	statement->Get(0, value);
	return value;
}

void UserModel::Initialize(wstring name)
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
			"( titleText"
			", bodyText"
			")"
		);

	CreateTable
		( "CREATE TABLE Notes"
			"( guid PRIMARY KEY"
			", creationDate"
			", title"
			", body"
			", search"
			", notebook REFERENCES Notebooks"
			")"
		);

	CreateTable
		( "CREATE TABLE ImageResources"
			"( hash PRIMARY KEY"
			", data"
			", note REFERENCES Notes"
			")"
		);
}

void UserModel::LoadOrCreate(wstring name)
{
	wstring path = CreatePathFromName(name);
	if (TryLoad(path))
	{
		if (GetVersion() != 0)
			throw std::exception("Incorrect database version.");
	}
	else
	{
		Create(path);
		Initialize(name);
	}
	SetPragma("PRAGMA foreign_keys = ON");
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
