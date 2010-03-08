#include "stdafx.h"
#include "SqlStatement.h"

#include "Tools.h"

#include "SQLite/sqlite3.h"

using namespace std;
using namespace Tools;

SqlStatement::SqlStatement(sqlite3 * db, const char * sql)
	: db        (db)
	, statement (NULL)
{
	int result = sqlite3_prepare_v2(db, sql, -1, &statement, NULL);
	if (result != SQLITE_OK)
		throw std::exception(sqlite3_errmsg(db));
}

SqlStatement::~SqlStatement()
{
	int result = sqlite3_finalize(statement);
}

bool SqlStatement::Execute()
{
	int result = sqlite3_step(statement);
	if (result == SQLITE_ERROR || result == SQLITE_MISUSE)
		throw std::exception(sqlite3_errmsg(db));
	return result == SQLITE_DONE;
}

void SqlStatement::Bind(int index, __int32 n)
{
	int result = sqlite3_bind_int(statement, index, n);
	if (result != SQLITE_OK)
		throw std::exception(sqlite3_errmsg(db));
}

void SqlStatement::Bind(int index, __int64 n)
{
	int result = sqlite3_bind_int64(statement, index, n);
	if (result != SQLITE_OK)
		throw std::exception(sqlite3_errmsg(db));
}

void SqlStatement::Bind(int index, string text)
{
	int result = sqlite3_bind_text
		( statement
		, index
		, text.c_str()
		, text.size()
		, SQLITE_TRANSIENT
		);
	if (result != SQLITE_OK)
		throw std::exception(sqlite3_errmsg(db));
}

void SqlStatement::Bind(int index, wstring text)
{
	vector<unsigned char> utf8Text = ConvertToUtf8(text);
	const char * textPointer
		= utf8Text.empty()
		? ""
		: reinterpret_cast<const char*>(&utf8Text[0])
		;
	int result = sqlite3_bind_text
		( statement
		, index
		, textPointer
		, utf8Text.size()
		, SQLITE_TRANSIENT
		);
	if (result != SQLITE_OK)
		throw std::exception(sqlite3_errmsg(db));
}

void SqlStatement::Get(int index, __int32 & n)
{
	n = sqlite3_column_int(statement, index);
}

void SqlStatement::Get(int index, __int64 & n)
{
	n = sqlite3_column_int64(statement, index);
}

void SqlStatement::Get(int index, string & text)
{
	text = reinterpret_cast<const char *>(sqlite3_column_text(statement, index));
}

void SqlStatement::Get(int index, wstring & text)
{
	text = ConvertToUnicode(sqlite3_column_text(statement, index));
}
