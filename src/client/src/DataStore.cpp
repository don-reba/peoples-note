#include "stdafx.h"
#include "DataStore.h"

#include "Tools.h"

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

void DataStore::LoadOrCreate(wstring name)
{
	path = CreatePathFromName(name);
	Connect();
	Initialize(); // TODO: change
}

void DataStore::AddNotebook(INotebook & notebook)
{
	// TODO: implement
}

void DataStore::MakeNotebookDefault(INotebook & notebook)
{
	// TODO: implement
}

int DataStore::GetNotebookCount()
{
	// TODO: implement
	return 0;
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

void DataStore::Initialize()
{
	char * errorMessage = NULL;
	int result = sqlite3_exec
		( db
		, "CREATE TABLE Properties(key PRIMARY KEY, value NOT NULL)"
		, NULL
		, this
		, &errorMessage
		);
	if (SQLITE_OK != result)
		throw exception("Nyu!");
}
