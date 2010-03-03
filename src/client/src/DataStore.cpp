#include "stdafx.h"
#include "DataStore.h"

#include "Note.h"
#include "Notebook.h"
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

int DataStore::GetVersion()
{
	return _wtoi(GetProperty(L"version").c_str());
}

wstring DataStore::GetUser()
{
	return GetProperty(L"user");
}

void DataStore::MakeNotebookLastUsed(const INotebook & notebook)
{
	// remove old
	{
		sqlite3_stmt * statement = PrepareStatement("UPDATE Notebooks SET isLastUsed = 0 WHERE isLastUsed = 1");
		ExecuteStatement(statement);
		CloseStatement(statement);
	}
	// set new
	{
		sqlite3_stmt * statement = PrepareStatement("UPDATE Notebooks SET isLastUsed = 1 WHERE guid = ?");
		BindText(statement, notebook.GetGuid(), 1);
		ExecuteStatement(statement);
		CloseStatement(statement);
	}
}

INotebook & DataStore::GetDefaultNotebook()
{
	sqlite3_stmt * statement = PrepareStatement("SELECT guid, name FROM Notebooks WHERE isDefault = 1 LIMIT 1");

	if (ExecuteStatement(statement))
		throw std::exception("No default notebook.");
	wstring guid(GetColumnText(statement, 0));
	wstring name(GetColumnText(statement, 1));

	CloseStatement(statement);

	defaultNotebook = make_shared<Notebook>(guid, name);
	return *defaultNotebook;
}

void DataStore::AddNote(const INote & note, const INotebook & notebook)
{
	{
		sqlite3_stmt * statement = PrepareStatement("INSERT INTO NoteContents(title) VALUES (?)");
		BindText(statement, note.GetTitle(), 1);
		ExecuteStatement(statement);
		CloseStatement(statement);
	}
	{
		sqlite3_stmt * statement = PrepareStatement("INSERT INTO Notes(guid, content, notebook) VALUES (?, ?, ?)");
		BindText (statement, note.GetGuid(),       1);
		BindInt  (statement, GetLastInsertRowid(), 2);
		BindText (statement, notebook.GetGuid(),   3);
		ExecuteStatement(statement);
		CloseStatement(statement);
	}
}

//--------------------------
// IDataStore implementation
//--------------------------

void DataStore::LoadOrCreate(wstring name)
{
	path = CreatePathFromName(name);
	Connect();
	SetPragma("PRAGMA foreign_keys = ON");
	Initialize(name); // TODO: change
}

void DataStore::AddNotebook(const INotebook & notebook)
{
	sqlite3_stmt * statement = PrepareStatement("INSERT INTO Notebooks(guid, name, isDefault, isLastUsed) VALUES (?, ?, 0, 0)");
	BindText(statement, notebook.GetGuid(), 1);
	BindText(statement, notebook.GetName(), 2);
	ExecuteStatement(statement);
	CloseStatement(statement); // TODO: handle exception
}

void DataStore::MakeNotebookDefault(const INotebook & notebook)
{
	// remove old
	{
		sqlite3_stmt * statement = PrepareStatement("UPDATE Notebooks SET isDefault = 0 WHERE isDefault = 1");
		ExecuteStatement(statement);
		CloseStatement(statement);
	}
	// set new
	{
		sqlite3_stmt * statement = PrepareStatement("UPDATE Notebooks SET isDefault = 1 WHERE guid = ?");
		BindText(statement, notebook.GetGuid(), 1);
		ExecuteStatement(statement);
		CloseStatement(statement);
	}
}

INotebook & DataStore::GetLastUsedNotebook()
{
	sqlite3_stmt * statement = PrepareStatement("SELECT guid, name FROM Notebooks WHERE isLastUsed = 1 LIMIT 1");

	if (ExecuteStatement(statement))
		throw std::exception("No last used notebook.");
	wstring guid(GetColumnText(statement, 0));
	wstring name(GetColumnText(statement, 1));

	CloseStatement(statement);

	defaultNotebook = make_shared<Notebook>(guid, name);
	return *defaultNotebook;
}

ptr_vector<INotebook> & DataStore::GetNotebooks()
{
	sqlite3_stmt * statement = PrepareStatement("SELECT guid, name FROM Notebooks ORDER BY name");
	notebooks.clear();
	while (!ExecuteStatement(statement))
	{
		wstring guid(GetColumnText(statement, 0));
		wstring name(GetColumnText(statement, 1));
		notebooks.push_back(new Notebook(guid, name));
	}

	CloseStatement(statement);

	return notebooks;
}

int DataStore::GetNotebookCount()
{
	sqlite3_stmt * statement = PrepareStatement("SELECT COUNT(*) FROM Notebooks");
	int count = GetColumnInt(statement, 0);
	CloseStatement(statement);
	return count;
}

ptr_vector<INote> & DataStore::GetNotesByNotebook(const INotebook & notebook)
{
	sqlite3_stmt * statement = PrepareStatement
		( "SELECT guid, title"
			" FROM Notes, NoteContents"
			" WHERE content = docid AND notebook = ?"
			" ORDER BY title"
		);
	BindText(statement, notebook.GetGuid(), 1);
	notes.clear();
	while (!ExecuteStatement(statement))
	{
		Guid    guid  (GetColumnText(statement, 0));
		wstring title (GetColumnText(statement, 1));
		notes.push_back(new Note(guid, title));
	}
	CloseStatement(statement);
	return notes;
}

ptr_vector<INote> & DataStore::GetNotesBySearch(wstring search)
{
	sqlite3_stmt * statement = PrepareStatement
		( "SELECT guid, title"
			" FROM Notes, NoteContents"
			" WHERE content = docid AND NoteContents MATCH ?"
			" ORDER BY title"
			);
	BindText(statement, search, 1);
	notes.clear();
	while (!ExecuteStatement(statement))
	{
		Guid    guid  (GetColumnText(statement, 0));
		wstring title (GetColumnText(statement, 1));
		notes.push_back(new Note(guid, title));
	}
	CloseStatement(statement);
	return notes;
}

//------------------
// utility functions
//------------------

void DataStore::Connect()
{
	assert(db == NULL);
	vector<unsigned char> utf8Path = ConvertToUtf8(path);
	int flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
	int result = sqlite3_open_v2
		( reinterpret_cast<const char *>(&utf8Path[0])
		, &db
		, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE
		, NULL
		);
	if (SQLITE_OK != result)
	{
		sqlite3_close(db);
		db = NULL;
		throw std::exception(sqlite3_errmsg(db));
	}
}

wstring DataStore::CreatePathFromName(wstring name)
{
	wstringstream stream;
	stream << folder << L'\\' << name << L".db";
	return stream.str();
}

void DataStore::Disconnect()
{
	if (db == NULL)
		return;
	sqlite3 * copy = db;
	db = NULL;
	int result = sqlite3_close(copy);
	if (result != SQLITE_OK)
		throw std::exception(sqlite3_errmsg(copy));
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
			", content"
			", notebook REFERENCES Notebooks"
			")"
		);
}

std::wstring DataStore::GetProperty(std::wstring key)
{
	sqlite3_stmt * statement = PrepareStatement
		( "SELECT value FROM Properties WHERE key = ? LIMIT 1"
		);
	BindText(statement, key, 1);

	if (ExecuteStatement(statement))
		throw std::exception("Property not found.");
	wstring value(GetColumnText(statement, 0));

	CloseStatement(statement);

	return value;
}

void DataStore::CreateTable(const char * sql)
{
	sqlite3_stmt * statement = PrepareStatement(sql);
	ExecuteStatement(statement);
	CloseStatement(statement);
}

void DataStore::AddProperty(wstring key, wstring value)
{
	sqlite3_stmt * statement = PrepareStatement("INSERT INTO Properties VALUES (?, ?)");
	BindText(statement, key,   1);
	BindText(statement, value, 2);
	ExecuteStatement(statement);
	CloseStatement(statement);
}
void DataStore::SetPragma(const char * sql)
{
	sqlite3_stmt * statement = PrepareStatement(sql);
	ExecuteStatement(statement);
	CloseStatement(statement);
}

//----------------
// SQLite wrappers
//----------------

sqlite3_stmt * DataStore::PrepareStatement(const char * sql)
{
	sqlite3_stmt * statement = NULL;
	int result = sqlite3_prepare_v2(db, sql, -1, &statement, NULL);
	if (result != SQLITE_OK)
		throw std::exception(sqlite3_errmsg(db));
	return statement;
}

void DataStore::BindInt(sqlite3_stmt * statement, __int64 n, int index)
{
	int result = sqlite3_bind_int64(statement, index, n);
	if (result != SQLITE_OK)
		throw std::exception(sqlite3_errmsg(db));
}

void DataStore::BindText
	( sqlite3_stmt * statement
	, std::string    text
	, int            index
	)
{
	int result = sqlite3_bind_text
		( statement
		, index
		, text.c_str()
		, text.size()
		, SQLITE_TRANSIENT
		);
	if (result != SQLITE_OK)
		throw std::exception(sqlite3_errmsg(db));
}

void DataStore::BindText
	( sqlite3_stmt * statement
	, std::wstring   text
	, int            index
	)
{
	vector<unsigned char> utf8Text = ConvertToUtf8(text);
	int result = sqlite3_bind_text
		( statement
		, index
		, reinterpret_cast<const char*>(&utf8Text[0])
		, utf8Text.size()
		, SQLITE_TRANSIENT
		);
	if (result != SQLITE_OK)
		throw std::exception(sqlite3_errmsg(db));
}

bool DataStore::ExecuteStatement(sqlite3_stmt * statement)
{
	int result = sqlite3_step(statement);
	if (result == SQLITE_ERROR || result == SQLITE_MISUSE)
		throw std::exception(sqlite3_errmsg(db));
	return result == SQLITE_DONE;
}

int DataStore::GetColumnInt(sqlite3_stmt * statement, int index)
{
	int result = sqlite3_step(statement);
	if (result == SQLITE_ERROR || result == SQLITE_MISUSE)
		throw std::exception(sqlite3_errmsg(db));
	return sqlite3_column_int(statement, index);
}

wstring DataStore::GetColumnText(sqlite3_stmt * statement, int index)
{
	return ConvertToUnicode(sqlite3_column_text(statement, index));
}

void DataStore::CloseStatement(sqlite3_stmt * statement)
{
	int result = sqlite3_finalize(statement);
	if (result != SQLITE_OK)
		throw std::exception(sqlite3_errmsg(db));
}

__int64 DataStore::GetLastInsertRowid()
{
	return sqlite3_last_insert_rowid(db);
}
