#include "stdafx.h"
#include "SqlStatement.h"

#include "IDataStore.h"
#include "SqlStatementInfo.h"
#include "Timer.h"
#include "Tools.h"

#include "SQLite/sqlite3.h"

using namespace std;
using namespace Tools;

SqlStatement::SqlStatement(sqlite3 * db, const char * sql, SqlStatementInfo & info)
	: db        (db)
	, info      (info)
	, statement (NULL)
{
	++info.UseCount;

	Timer timer;
	int result = sqlite3_prepare_v2(db, sql, -1, &statement, NULL);
	timer.Stop();

	if (result == SQLITE_OK)
		info.PreparationTime += timer.GetElapsedSeconds();
	else
		HandleError(sqlite3_errmsg(db));
}

SqlStatement::~SqlStatement()
{
	if (statement != NULL)
		Finalize();
}

bool SqlStatement::Execute()
{
	Timer timer;
	int result(sqlite3_step(statement));
	timer.Stop();

	switch (result)
	{
	case SQLITE_OK:
	case SQLITE_DONE:
		info.ExecutionTime += timer.GetElapsedSeconds();
		return true;
	case SQLITE_ROW:
		info.ExecutionTime += timer.GetElapsedSeconds();
		return false;
	default:
		HandleError(sqlite3_errmsg(db));
		return false;
	}
}

void SqlStatement::Finalize()
{
	Timer timer;
	int result = sqlite3_finalize(statement);
	timer.Stop();

	if (result == SQLITE_OK)
		info.FinalizationTime += timer.GetElapsedSeconds();
	else
		DEBUGMSG(true, (L"%s\n", sqlite3_errmsg(db)));
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

void SqlStatement::Bind(int index, double n)
{
	int result = sqlite3_bind_double(statement, index, n);
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

void SqlStatement::BindNull(int index)
{
	int result = sqlite3_bind_null(statement, index);
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

void SqlStatement::Get(int index, double & n)
{
	n = sqlite3_column_double(statement, index);
}

void SqlStatement::Get(int index, string & text)
{
	const unsigned char * temp(sqlite3_column_text(statement, index));
	if (temp)
		text = reinterpret_cast<const char *>(temp);
}

void SqlStatement::Get(int index, wstring & text)
{
	ConvertToUnicode(sqlite3_column_text(statement, index), text);
}

void SqlStatement::Get(int index, Guid & guid)
{
	string temp;
	Get(index, temp);
	guid = temp;
}

void SqlStatement::Get(int index, Timestamp & time)
{
	__int64 temp;
	Get(index, temp);
	time = static_cast<time_t>(temp);
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
