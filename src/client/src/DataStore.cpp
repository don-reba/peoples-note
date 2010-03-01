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

void DataStore::MakeNotebookLastUsed(INotebook & notebook)
{
	// remove old
	{
		sqlite3_stmt * statement = PrepareStatement("UPDATE Notebooks SET isLastUsed = 0 WHERE isLastUsed = 1");
		ExecuteStatement(statement);
		CloseStatement(statement);
	}
	// set new
	{
		sqlite3_stmt * statement = PrepareStatement("UPDATE Notebooks SET isLastUsed = 1 WHERE name = ?");
		BindText(statement, notebook.GetName(), 1);
		ExecuteStatement(statement);
		CloseStatement(statement);
	}
}

INotebook & DataStore::GetDefaultNotebook()
{
	sqlite3_stmt * statement = PrepareStatement("SELECT name FROM Notebooks WHERE isDefault = 1 LIMIT 1");

	if (ExecuteStatement(statement))
		throw std::exception("No default notebook.");
	wstring name(GetColumnText(statement, 0));

	CloseStatement(statement);

	defaultNotebook = make_shared<Notebook>(name);
	return *defaultNotebook;
}

//--------------------------
// IDataStore implementation
//--------------------------

void DataStore::LoadOrCreate(wstring name)
{
	path = CreatePathFromName(name);
	Connect();
	Initialize(name); // TODO: change
}

void DataStore::AddNotebook(INotebook & notebook)
{
	sqlite3_stmt * statement = PrepareStatement("INSERT INTO Notebooks(name, isDefault, isLastUsed) VALUES (?, 0, 0)");
	BindText(statement, notebook.GetName(), 1);
	ExecuteStatement(statement);
	CloseStatement(statement); // TODO: handle exception
}

void DataStore::MakeNotebookDefault(INotebook & notebook)
{
	// remove old
	{
		sqlite3_stmt * statement = PrepareStatement("UPDATE Notebooks SET isDefault = 0 WHERE isDefault = 1");
		ExecuteStatement(statement);
		CloseStatement(statement);
	}
	// set new
	{
		sqlite3_stmt * statement = PrepareStatement("UPDATE Notebooks SET isDefault = 1 WHERE name = ?");
		BindText(statement, notebook.GetName(), 1);
		ExecuteStatement(statement);
		CloseStatement(statement);
	}
}

INotebook & DataStore::GetLastUsedNotebook()
{
	sqlite3_stmt * statement = PrepareStatement("SELECT name FROM Notebooks WHERE isLastUsed = 1 LIMIT 1");

	if (ExecuteStatement(statement))
		throw std::exception("No last used notebook.");
	wstring name(GetColumnText(statement, 0));

	CloseStatement(statement);

	defaultNotebook = make_shared<Notebook>(name);
	return *defaultNotebook;
}

ptr_vector<INotebook> & DataStore::GetNotebooks()
{
	sqlite3_stmt * statement = PrepareStatement("SELECT name FROM Notebooks ORDER BY name");
	notebooks.clear();
	while (!ExecuteStatement(statement))
		notebooks.push_back(new Notebook(GetColumnText(statement, 0)));

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

ptr_vector<INote> & DataStore::GetNotesByNotebook(INotebook & notebook)
{
	// TODO: implement
	return notes;
}

ptr_vector<INote> & DataStore::GetNotesBySearch(wstring search)
{
	// TODO: implement
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

void DataStore::Disconnect()
{
	if (db == NULL)
		return;
	sqlite3 * copy = db;
	db = NULL;
	if (SQLITE_OK != sqlite3_close(copy))
		throw std::exception(sqlite3_errmsg(db));
}

wstring DataStore::CreatePathFromName(wstring name)
{
	wstringstream stream;
	stream << folder << L'\\' << name << L".db";
	return stream.str();
}

void DataStore::Initialize(wstring name)
{
	int result;
	const char * sql;
	char * message = NULL;

	// properties table
	sql = "CREATE TABLE Properties(key PRIMARY KEY, value NOT NULL)";
	result = sqlite3_exec(db, sql, NULL, NULL, &message);
	if (SQLITE_OK != result)
		throw std::exception(message); // HACK: memory leak

	// properties
	typedef pair<string, string> Property;
	vector<Property> properties;
	properties.push_back(make_pair("version", "0"));
	properties.push_back(make_pair("user",    ConvertToAnsi(name)));
	foreach (const Property & p, properties)
	{
		stringstream stream;
		stream << "INSERT INTO Properties VALUES ('" << p.first << "', '" << p.second << "')";
		result = sqlite3_exec(db, stream.str().c_str(), NULL, NULL, &message);
		if (SQLITE_OK != result)
			throw std::exception(message); // HACK: memory leak
	}

	// notebooks table
	sql = "CREATE TABLE Notebooks(name PRIMARY KEY, isDefault, isLastUsed)";
	result = sqlite3_exec(db, sql, NULL, NULL, &message);
	if (SQLITE_OK != result)
		throw std::exception(message); // HACK: memory leak
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
	return sqlite3_column_int(statement, 0);
}

wstring DataStore::GetColumnText(sqlite3_stmt * statement, int index)
{
	return ConvertToUnicode(sqlite3_column_text(statement, 0));
}

void DataStore::CloseStatement(sqlite3_stmt * statement)
{
	int result = sqlite3_finalize(statement);
	if (result != SQLITE_OK)
		throw std::exception(sqlite3_errmsg(db));
}
