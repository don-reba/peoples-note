#include "stdafx.h"
#include "SqlStatement.h"

#include "IDataStore.h"
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
		HandleError(sqlite3_errmsg(db));
}

SqlStatement::~SqlStatement()
{
	if (statement != NULL)
		sqlite3_finalize(statement);
}

bool SqlStatement::Execute()
{
	int result = sqlite3_step(statement);
	if (result == SQLITE_ERROR || result == SQLITE_MISUSE)
		HandleError(sqlite3_errmsg(db));
	return result == SQLITE_DONE;
}

void SqlStatement::Finalize()
{
	int result(sqlite3_finalize(statement));
	statement = NULL;
	if (result != SQLITE_OK)
		HandleError(sqlite3_errmsg(db));
}

void SqlStatement::Bind(int index, __int32 n)
{
	int result = sqlite3_bind_int(statement, index, n);
	if (result != SQLITE_OK)
		HandleError(sqlite3_errmsg(db));
}

void SqlStatement::Bind(int index, __int64 n)
{
	int result = sqlite3_bind_int64(statement, index, n);
	if (result != SQLITE_OK)
		HandleError(sqlite3_errmsg(db));
}

void SqlStatement::Bind(int index, const string & text)
{
	int result = sqlite3_bind_text
		( statement
		, index
		, text.c_str()
		, text.size()
		, SQLITE_TRANSIENT
		);
	if (result != SQLITE_OK)
		HandleError(sqlite3_errmsg(db));
}

void SqlStatement::Bind(int index, const wstring & text)
{
	vector<unsigned char> textUtf8Chars;
	const char * textUtf8 = reinterpret_cast<const char *>(ConvertToUtf8(text, textUtf8Chars));
	int result = sqlite3_bind_text
		( statement
		, index
		, textUtf8
		, textUtf8Chars.size()
		, SQLITE_TRANSIENT
		);
	if (result != SQLITE_OK)
		HandleError(sqlite3_errmsg(db));
}

void SqlStatement::Bind(int index, const Blob & blob)
{
	if (blob.empty())
		return;
	int result = sqlite3_bind_blob
		( statement
		, index
		, &blob[0]
		, blob.size()
		, SQLITE_STATIC
		);
	if (result != SQLITE_OK)
		HandleError(sqlite3_errmsg(db));
}

void SqlStatement::Get(int index, bool & n)
{
	n = (sqlite3_column_int(statement, index) != 0);
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
	ConvertToUnicode(sqlite3_column_text(statement, index), text);
}

bool SqlStatement::IsNull(int index)
{
	return SQLITE_NULL == sqlite3_column_type(statement, index);
}

void SqlStatement::HandleError(const std::string msg)
{
	DEBUGMSG(true, (L"%s\n", Tools::ConvertToUnicode(msg).c_str()));
	throw std::exception(msg.c_str());
}
