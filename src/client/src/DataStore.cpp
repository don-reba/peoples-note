#include "stdafx.h"
#include "DataStore.h"

#include "SqlBlob.h"
#include "SqlStatement.h"
#include "Tools.h"

#include <sstream>

using namespace boost;
using namespace std;
using namespace Tools;

//----------
// interface
//----------

DataStore::DataStore()
	: db       (NULL)
	, location (DbLocationNone)
{
}

DataStore::~DataStore()
{
	sqlite3_close(db);

#ifdef PROFILE_SQL
	foreach (StatementCache::value_type & info, statements)
	{
		wstring sql(ConvertToUnicode(info.first));
		if (sql.size() > 80)
			sql.resize(80);

		wstringstream preparationTime;  preparationTime  << info.second.PreparationTime;
		wstringstream executionTime;    executionTime    << info.second.ExecutionTime;
		wstringstream finalizationTime; finalizationTime << info.second.FinalizationTime;

		::NKDbgPrintfW(L"%s\n",                 sql.c_str());
		::NKDbgPrintfW(L"Count:        %d\n",   info.second.UseCount);
		::NKDbgPrintfW(L"Preparation:  %s s\n", preparationTime.str().c_str());
		::NKDbgPrintfW(L"Execution:    %s s\n", executionTime.str().c_str());
		::NKDbgPrintfW(L"Finalization: %s s\n", finalizationTime.str().c_str());
	}
#endif // PROFILE_SQL
}

//--------------------------
// IDataStore implementation
//--------------------------

void DataStore::Close()
{
	if (NULL != db)
	{
		int result(sqlite3_close(db));
		db = NULL;
		path.clear();
		location = DbLocationNone;
	}
}

bool DataStore::Open(const wchar_t * path, DbLocation location, int flags)
{
	assert(db == NULL);

	vector<unsigned char> utf8PathChars;
	const char * utf8Path
		= reinterpret_cast<const char *>
		( ConvertToUtf8(path, utf8PathChars)
		);

	int result = sqlite3_open_v2(utf8Path, &db, flags, NULL);
	if (SQLITE_OK != result)
	{
		sqlite3_close(db);
		db = NULL;
		return false;
	}

	sqlite3_busy_handler(db, &HandleBusy, this);

	sqlite3_extended_result_codes(db, true);

	this->location = location;
	this->path     = path;

	return true;
}

__int64 DataStore::GetLastInsertRowid()
{
	return sqlite3_last_insert_rowid(db);
}

DbLocation DataStore::GetLocation()
{
	return location;
}

wstring DataStore::GetPath()
{
	return path;
}

IDataStore::Blob DataStore::MakeBlob
	( const char * table
	, const char * column
	, __int64      row
	)
{
	return make_shared<SqlBlob>(db, table, column, row);
}

IDataStore::Statement DataStore::MakeStatement(const char * sql)
{
#ifdef PROFILE_SQL
	return make_shared<SqlStatement>(db, sql, ref(statements[sql]));
#else // PROFILE_SQL
	return make_shared<SqlStatement>(db, sql);
#endif // PROFILE_SQL
}

int DataStore::HandleBusy(void * param, int count)
{
	if (count < 0 || count > 13)
		return 0;
	::Sleep(1 << count);
	return 1;
}
