#include "stdafx.h"
#include "SqlBlob.h"

#include "SQLite/sqlite3.h"

SqlBlob::SqlBlob
	( sqlite3    * db
	, const char * table
	, const char * column
	, __int64      row
	)
	: blob (NULL)
	, db   (db)
{
	const char * dbName = "main";
	int          flags  = 0;
	int result = sqlite3_blob_open(db, dbName, table, column, row, flags, &blob);
	if (result != SQLITE_OK)
		throw std::exception(sqlite3_errmsg(db));
}

void SqlBlob::Read(Blob & blob)
{
	blob.resize(sqlite3_blob_bytes(this->blob));
	int result = sqlite3_blob_read(this->blob, &blob[0], blob.size(), 0);
	if (result != SQLITE_OK)
		throw std::exception(sqlite3_errmsg(db));
}

SqlBlob::~SqlBlob()
{
	if (NULL != blob)
		sqlite3_blob_close(blob);
}