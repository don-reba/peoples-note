#include "stdafx.h"
#include "DataStore.h"

#include "INotebook.h"
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

INotebook & DataStore::GetDefaultNotebook()
{
	int result;

	// prepare statement
	sqlite3_stmt * statement = NULL;
	const char * sql = "SELECT name FROM Notebooks WHERE isDefault = 1 LIMIT 1";
	result = sqlite3_prepare_v2(db, sql, -1, &statement, NULL);
	if (result != SQLITE_OK)
		throw std::exception("Query could not be constructed.");

	// retrieve value
	result = sqlite3_step(statement);
	if (result == SQLITE_ERROR || result == SQLITE_MISUSE)
		throw std::exception("Query could not be executed.");
	if (result == SQLITE_DONE)
		throw std::exception("No default notebook.");
	wstring name(ConvertToUnicode(sqlite3_column_text(statement, 0)));

	// close statement
	result = sqlite3_finalize(statement);
	if (result != SQLITE_OK)
		throw std::exception("Query could not be finalized.");

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
	int result;

	// prepare statement
	sqlite3_stmt * statement = NULL;
	const char * sql = "INSERT INTO Notebooks(name, isDefault, isLastUsed) VALUES (?, 0, 0)";
	result = sqlite3_prepare_v2(db, sql, -1, &statement, NULL);
	if (result != SQLITE_OK)
		throw std::exception(sqlite3_errmsg(db));

	// bind name
	vector<unsigned char> utf8Name(ConvertToUtf8(notebook.GetName()));
	result = sqlite3_bind_text
		( statement
		, 1
		, reinterpret_cast<const char*>(&utf8Name[0])
		, -1
		, SQLITE_STATIC
		);
	if (result != SQLITE_OK)
		throw std::exception(sqlite3_errmsg(db));

	// execute statement
	result = sqlite3_step(statement);
	if (result == SQLITE_ERROR || result == SQLITE_MISUSE)
		throw std::exception(sqlite3_errmsg(db));

	// close statement
	// TODO: handle this better
	result = sqlite3_finalize(statement);
	if (result != SQLITE_OK)
		throw std::exception(sqlite3_errmsg(db));
}

void DataStore::MakeNotebookDefault(INotebook & notebook)
{
	int result;

	// remove old default
	{
		// prepare statement
		sqlite3_stmt * statement = NULL;
		const char * sql = "UPDATE Notebooks SET isDefault = 0 WHERE isDefault = 1";
		result = sqlite3_prepare_v2(db, sql, -1, &statement, NULL);
		if (result != SQLITE_OK)
			throw std::exception(sqlite3_errmsg(db));

		// execute statement
		result = sqlite3_step(statement);
		if (result == SQLITE_ERROR || result == SQLITE_MISUSE)
			throw std::exception(sqlite3_errmsg(db));

		// close statement
		result = sqlite3_finalize(statement);
		if (result != SQLITE_OK)
			throw std::exception(sqlite3_errmsg(db));
	}

	// set new default
	{
		// prepare statement
		sqlite3_stmt * statement = NULL;
		const char * sql = "UPDATE Notebooks SET isDefault = 1 WHERE name = ?";
		result = sqlite3_prepare_v2(db, sql, -1, &statement, NULL);
		if (result != SQLITE_OK)
			throw std::exception(sqlite3_errmsg(db));

		// bind name
		vector<unsigned char> utf8Name(ConvertToUtf8(notebook.GetName()));
		result = sqlite3_bind_text
			( statement
			, 1
			, reinterpret_cast<const char*>(&utf8Name[0])
			, -1
			, SQLITE_STATIC
			);
		if (result != SQLITE_OK)
			throw std::exception(sqlite3_errmsg(db));

		// execute statement
		result = sqlite3_step(statement);
		if (result == SQLITE_ERROR || result == SQLITE_MISUSE)
			throw std::exception(sqlite3_errmsg(db));

		// close statement
		result = sqlite3_finalize(statement);
		if (result != SQLITE_OK)
			throw std::exception(sqlite3_errmsg(db));
	}
}

INotebook & DataStore::GetLastUsedNotebook()
{
	int result;

	// prepare statement
	sqlite3_stmt * statement = NULL;
	const char * sql = "SELECT name FROM Notebooks WHERE isLastUsed = 1 LIMIT 1";
	result = sqlite3_prepare_v2(db, sql, -1, &statement, NULL);
	if (result != SQLITE_OK)
		throw std::exception("Query could not be constructed.");

	// retrieve value
	result = sqlite3_step(statement);
	if (result == SQLITE_ERROR || result == SQLITE_MISUSE)
		throw std::exception("Query could not be executed.");
	if (result == SQLITE_DONE)
		throw std::exception("No default notebook.");
	wstring name(ConvertToUnicode(sqlite3_column_text(statement, 0)));

	// close statement
	result = sqlite3_finalize(statement);
	if (result != SQLITE_OK)
		throw std::exception("Query could not be finalized.");

	defaultNotebook = make_shared<Notebook>(name);
	return *defaultNotebook;
}

ptr_vector<INotebook> & DataStore::GetNotebooks()
{
	int result;

	// prepare statement
	sqlite3_stmt * statement = NULL;
	const char * sql = "SELECT name FROM Notebooks ORDER BY name";
	result = sqlite3_prepare_v2(db, sql, -1, &statement, NULL);
	if (result != SQLITE_OK)
		throw std::exception("Query could not be constructed.");

	// retrieve values
	notebooks.clear();
	for (;;)
	{
		result = sqlite3_step(statement);
		if (result == SQLITE_ERROR || result == SQLITE_MISUSE)
			throw std::exception("Query could not be executed.");
		if (result == SQLITE_DONE)
		{
			break;
		}
		else
		{
			const unsigned char * name = sqlite3_column_text(statement, 0);
			notebooks.push_back(new Notebook(ConvertToUnicode(name)));
		}
	}

	// close statement
	result = sqlite3_finalize(statement);
	if (result != SQLITE_OK)
		throw std::exception("Query could not be finalized.");

	return notebooks;
}

int DataStore::GetNotebookCount()
{
	int result;

	// prepare statement
	sqlite3_stmt * statement = NULL;
	const char * sql = "SELECT COUNT(*) FROM Notebooks";
	result = sqlite3_prepare_v2(db, sql, -1, &statement, NULL);
	if (result != SQLITE_OK)
		throw std::exception(sqlite3_errmsg(db));

	// retrieve value
	result = sqlite3_step(statement);
	if (result == SQLITE_ERROR || result == SQLITE_MISUSE)
		throw std::exception(sqlite3_errmsg(db));
	int count = sqlite3_column_int(statement, 0);

	// close statement
	result = sqlite3_finalize(statement);
	if (result != SQLITE_OK)
		throw std::exception(sqlite3_errmsg(db));

	return count;
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

	// default notebook
	sql = "INSERT into Notebooks(name, isDefault, isLastUsed) VALUES ('Notes', 1, 1)";
	result = sqlite3_exec(db, sql, NULL, NULL, &message);
	if (SQLITE_OK != result)
		throw std::exception(message); // HACK: memory leak
}

std::wstring DataStore::GetProperty(std::wstring key)
{
	int result;

	// prepare statement
	sqlite3_stmt * statement = NULL;
	const char * sql = "SELECT value FROM Properties WHERE key = ? LIMIT 1";
	result = sqlite3_prepare_v2(db, sql, -1, &statement, NULL);
	if (result != SQLITE_OK)
		throw std::exception("Query could not be constructed.");

	// bind key
	vector<unsigned char> utf8Key = ConvertToUtf8(key);
	result = sqlite3_bind_text
		( statement
		, 1
		, reinterpret_cast<const char *>(&utf8Key[0])
		, -1
		, SQLITE_STATIC
		);
	if (result != SQLITE_OK)
		throw std::exception("Query could not be parameterized.");

	// retrieve value
	result = sqlite3_step(statement);
	if (result == SQLITE_ERROR || result == SQLITE_MISUSE)
		throw std::exception("Query could not be executed.");
	if (result == SQLITE_DONE)
		throw std::exception("Property not found.");
	wstring value(ConvertToUnicode(sqlite3_column_text(statement, 0)));

	// close statement
	result = sqlite3_finalize(statement);
	if (result != SQLITE_OK)
		throw std::exception("Query could not be finalized.");

	return value;
}
