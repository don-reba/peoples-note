#pragma once

#include "ISqlStatement.h"

struct sqlite3_stmt;
struct sqlite3;

class SqlStatement : public ISqlStatement
{
private:

	sqlite3_stmt * statement;
	sqlite3      * db;

public:

	SqlStatement(sqlite3 * db, const char * sql);

	~SqlStatement();

// ISqlStatement implementation

public:

	virtual bool Execute();

	virtual void Bind(int index, __int32      n);
	virtual void Bind(int index, __int64      n);
	virtual void Bind(int index, std::string  text);
	virtual void Bind(int index, std::wstring text);

	virtual void Get(int index, __int32      & n);
	virtual void Get(int index, __int64      & n);
	virtual void Get(int index, std::string  & text);
	virtual void Get(int index, std::wstring & text);
};
