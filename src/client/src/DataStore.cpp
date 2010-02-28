#include "stdafx.h"
#include "DataStore.h"

#include "INotebook.h"
#include "Tools.h"

#include <map>
#include <sstream>

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

void DataStore::LoadOrCreate(wstring name)
{
	path = CreatePathFromName(name);
	Connect();
	Initialize(name); // TODO: change
}

void DataStore::AddNotebook(INotebook & notebook)
{
	int result;

	// prepare statement
	sqlite3_stmt * statement = NULL;
	const wchar_t * sql = L"INSERT into Notebooks VALUES (1, ?, 0, 0)";
	result = sqlite3_prepare16_v2(db, sql, -1, &statement, NULL);
	if (result != SQLITE_OK)
		throw exception("Query could not be constructed.");

	// bind name
	result = sqlite3_bind_text16(statement, 1, notebook.GetName().c_str(), -1, SQLITE_STATIC);
	if (result != SQLITE_OK)
		throw exception("Query could not be parameterized.");

	// execute statement
	result = sqlite3_step(statement);
	if (result == SQLITE_ERROR || result == SQLITE_MISUSE)
		throw exception("Query could not be executed.");

	// close statement
	result = sqlite3_finalize(statement);
	if (result != SQLITE_OK)
		throw exception("Notebook could not be added");
}

void DataStore::MakeNotebookDefault(INotebook & notebook)
{
	// TODO: implement
}

int DataStore::GetNotebookCount()
{
	int result;

	// prepare statement
	sqlite3_stmt * statement = NULL;
	const wchar_t * sql = L"SELECT COUNT(*) FROM Notebooks";
	result = sqlite3_prepare16_v2(db, sql, -1, &statement, NULL);
	if (result != SQLITE_OK)
		throw exception("Query could not be constructed.");

	// retrieve value
	result = sqlite3_step(statement);
	if (result == SQLITE_ERROR || result == SQLITE_MISUSE)
		throw exception("Query could not be executed.");
	int count = sqlite3_column_int(statement, 0);

	// close statement
	result = sqlite3_finalize(statement);
	if (result != SQLITE_OK)
		throw exception("Query could not be finalized.");

	return count;
}

//------------------
// utility functions
//------------------

void DataStore::Connect()
{
	assert(db == NULL);
	if (SQLITE_OK != sqlite3_open16(path.c_str(), &db))
	{
		sqlite3_close(db);
		db = NULL;
		throw exception("Database could not be opened.");
	}
}

void DataStore::Disconnect()
{
	if (db == NULL)
		return;
	sqlite3 * copy = db;
	db = NULL;
	if (SQLITE_OK != sqlite3_close(copy))
		throw exception("Database could not be closed.");
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
		throw exception(message); // WARN: memory leak

	// properties
	typedef pair<string, string> Property;
	vector<Property> properties;
	properties.push_back(make_pair("version", "1"));
	properties.push_back(make_pair("user",    ConvertToAnsi(name)));
	foreach (const Property & p, properties)
	{
		stringstream stream;
		stream << "INSERT INTO Properties VALUES ('" << p.first << "', '" << p.second << "')";
		result = sqlite3_exec(db, stream.str().c_str(), NULL, NULL, &message);
		if (SQLITE_OK != result)
			throw exception(message); // WARN: memory leak
	}

	// notebooks table
	sql = "CREATE TABLE Notebooks(guid INTEGER PRIMARY KEY, name UNIQUE, isDefault, isLastUsed)";
	result = sqlite3_exec(db, sql, NULL, NULL, &message);
	if (SQLITE_OK != result)
		throw exception(message); // WARN: memory leak

	// default notebook
	sql = "INSERT into Notebooks VALUES (0, 'Notes', 1, 1)";
	result = sqlite3_exec(db, sql, NULL, NULL, &message);
	if (SQLITE_OK != result)
		throw exception(message); // WARN: memory leak
}

std::wstring DataStore::GetProperty(std::wstring key)
{
	int result;

	// prepare statement
	sqlite3_stmt * statement = NULL;
	const wchar_t * sql = L"SELECT value FROM Properties WHERE key = ? LIMIT 1";
	result = sqlite3_prepare16_v2(db, sql, -1, &statement, NULL);
	if (result != SQLITE_OK)
		throw exception("Query could not be constructed.");

	// bind key
	result = sqlite3_bind_text16(statement, 1, key.c_str(), -1, SQLITE_STATIC);
	if (result != SQLITE_OK)
		throw exception("Query could not be parameterized.");

	// retrieve value
	result = sqlite3_step(statement);
	if (result == SQLITE_ERROR || result == SQLITE_MISUSE)
		throw exception("Query could not be executed.");
	wstring value(static_cast<const wchar_t*>(sqlite3_column_text16(statement, 0)));

	// close statement
	result = sqlite3_finalize(statement);
	if (result != SQLITE_OK)
		throw exception("Query could not be finalized.");

	return value;
}
