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

	virtual void Close();

	virtual bool Create(std::wstring path, int flags);

	virtual __int64 GetLastInsertRowid();

	virtual Blob MakeBlob
		( const char * table
		, const char * column
		, __int64      row
		);

	virtual Statement MakeStatement(const char * sql);

private:

	static int HandleBusy(void * param, int count);
};
