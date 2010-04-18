#include "stdafx.h"
#include "DataStore.h"

#include "SqlBlob.h"
#include "SqlStatement.h"
#include "Tools.h"

using namespace boost;
using namespace std;
using namespace Tools;

//----------
// interface
//----------

DataStore::DataStore()
	: db (NULL)
{
}

DataStore::~DataStore()
{
	sqlite3_close(db);
}

//--------------------------
// IDataStore implementation
//--------------------------

void DataStore::Close()
{
	if (NULL != db)
	{
		sqlite3_close(db);
		db = NULL;
	}
}

bool DataStore::Create(std::wstring path, int flags)
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

	return true;
}

__int64 DataStore::GetLastInsertRowid()
{
	return sqlite3_last_insert_rowid(db);
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
	return make_shared<SqlStatement>(db, sql);
}

int DataStore::HandleBusy(void * param, int count)
{
	if (count < 0 || count > 13)
		return 0;
	::Sleep(1 << count);
	return 1;
}
