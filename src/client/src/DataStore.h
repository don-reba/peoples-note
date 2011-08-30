#pragma once
#include "IDataStore.h"

#include "SQLite/sqlite3.h"
#include "SqlStatementInfo.h"

#include <map>
#include <string>

class DataStore : public IDataStore
{
private:

	struct StatementComparator
	{
		bool operator () (const char * a, const char * b) const
		{
			return std::strcmp(a, b) < 0;
		}
	};

	typedef std::map<const char *, SqlStatementInfo, StatementComparator> StatementCache;

private:

	sqlite3 * db;

	DbLocation   location;
	std::wstring path;

#ifdef PROFILE_SQL
	StatementCache statements;
#endif // PROFILE_SQL

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
