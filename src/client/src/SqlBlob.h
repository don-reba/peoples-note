#pragma once

#include "ISqlBlob.h"

struct sqlite3;
struct sqlite3_blob;

class SqlBlob : public ISqlBlob
{
private:

	sqlite3_blob * blob;
	sqlite3      * db;

public:

	SqlBlob
		( sqlite3    * db
		, const char * table
		, const char * column
		, __int64      row
		);

	~SqlBlob();


public:

	virtual int GetSize();

	virtual void Read(Blob & blob);

	virtual void Read(int offset, int size, Blob & data);
};
