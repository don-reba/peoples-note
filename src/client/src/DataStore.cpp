#include "stdafx.h"
#include "DataStore.h"

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

bool DataStore::Create(std::wstring path, int flags)
{
	assert(db == NULL);

	vector<unsigned char> utf8Path = ConvertToUtf8(path);
	assert(!utf8Path.empty());
	const char * cPath = reinterpret_cast<const char *>(&utf8Path[0]);

	int result = sqlite3_open_v2(cPath, &db, flags, NULL);
	if (SQLITE_OK != result)
	{
		sqlite3_close(db);
		db = NULL;
		return false;
	}
	return true;
}

__int64 DataStore::GetLastInsertRowid()
{
	return sqlite3_last_insert_rowid(db);
}

boost::shared_ptr<ISqlStatement> DataStore::MakeStatement(const char * sql)
{
	return make_shared<SqlStatement>(db, sql);
}
