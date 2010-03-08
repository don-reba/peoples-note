#pragma once
#include "IDataStore.h"

#include "SQLite/sqlite3.h"

class DataStore : public IDataStore
{
private:

	sqlite3 * db;

// interface

public:

	DataStore();

	~DataStore();

// IDataStore implementation

public:

	virtual bool Create(std::wstring path, int flags);

	virtual __int64 GetLastInsertRowid();

	virtual Statement MakeStatement(const char * sql);
};
