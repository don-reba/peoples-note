#pragma once

#include "Note.h"
#include "Notebook.h"

class  ISqlBlob;
class  ISqlStatement;
struct sqlite3;

class IDataStore
{
public:

	typedef boost::shared_ptr<ISqlStatement> Statement;
	typedef boost::shared_ptr<ISqlBlob>      Blob;

public:

	virtual ~IDataStore() {}

	virtual bool Create(std::wstring path, int flags) = 0;

	virtual __int64 GetLastInsertRowid() = 0;

	virtual Blob MakeBlob
		( const char * table
		, const char * column
		, __int64      row
		) = 0;

	virtual Statement MakeStatement(const char * sql) = 0;
};
