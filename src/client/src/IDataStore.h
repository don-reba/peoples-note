#pragma once

#include "Note.h"
#include "Notebook.h"

class  ISqlStatement;
struct sqlite3;

class IDataStore
{
public:

	typedef boost::shared_ptr<ISqlStatement> Statement;

public:

	virtual ~IDataStore() {}

	virtual bool Create(std::wstring path, int flags) = 0;

	virtual __int64 GetLastInsertRowid() = 0;

	virtual Statement MakeStatement(const char * sql) = 0;
};
