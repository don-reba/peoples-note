#pragma once
#include "IDataStore.h"

#include "SQLite/sqlite3.h"

class DataStore : public IDataStore
{
private:

	sqlite3 * db;

	DbLocation   location;
	std::wstring path;

// interface

public:

	DataStore();

	~DataStore();

// IDataStore implementation

public:

	virtual void Close();

	virtual bool Open(const wchar_t * path, DbLocation location, int flags);

	virtual __int64 GetLastInsertRowid();

	virtual DbLocation GetLocation();

	virtual std::wstring GetPath();

	virtual Blob MakeBlob
		( const char * table
		, const char * column
		, __int64      row
		);

	virtual Statement MakeStatement(const char * sql);

private:

	static int HandleBusy(void * param, int count);
};
