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
	int          flags  = 0; // read-only
	int result = sqlite3_blob_open(db, dbName, table, column, row, flags, &blob);
	if (result != SQLITE_OK)
		throw std::exception(sqlite3_errmsg(db));
}

int SqlBlob::GetSize()
{
	return sqlite3_blob_bytes(this->blob);
}

void SqlBlob::Read(Blob & blob)
{
	blob.resize(GetSize());
	int result = sqlite3_blob_read(this->blob, &blob[0], blob.size(), 0);
	if (result != SQLITE_OK)
		throw std::exception(sqlite3_errmsg(db));
}

void SqlBlob::Read(int offset, int size, Blob & blob)
{
	blob.resize(size);
	int result = sqlite3_blob_read(this->blob, &blob[0], size, offset);
	if (result != SQLITE_OK)
		throw std::exception(sqlite3_errmsg(db));
}

SqlBlob::~SqlBlob()
{
	if (NULL != blob)
		sqlite3_blob_close(blob);
}