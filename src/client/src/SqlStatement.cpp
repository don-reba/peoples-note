#include "stdafx.h"
#include "SqlStatement.h"

#include "IDataStore.h"
#include "SqlStatementInfo.h"
#include "Timer.h"
#include "Tools.h"

#include "SQLite/sqlite3.h"

using namespace std;
using namespace Tools;

#ifdef PROFILE_SQL

SqlStatement::SqlStatement(sqlite3 * db, const char * sql, SqlStatementInfo & info)
	: bindIndex (1)
	, db        (db)
	, getIndex  (0)
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

#else PROFILE_SQL

SqlStatement::SqlStatement(sqlite3 * db, const char * sql)
	: bindIndex (1)
	, db        (db)
	, getIndex  (0)
	, statement (NULL)
{
	int result = sqlite3_prepare_v2(db, sql, -1, &statement, NULL);
	if (result != SQLITE_OK)
		HandleError(sqlite3_errmsg(db));
}

#endif // PROFILE_SQL

SqlStatement::~SqlStatement()
{
	if (statement != NULL)
		Finalize();
}

#ifdef PROFILE_SQL

bool SqlStatement::Execute()
{
	getIndex = 0;

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

#else // PROFILE_SQL

bool SqlStatement::Execute()
{
	getIndex = 0;
	switch (sqlite3_step(statement))
	{
	case SQLITE_OK:
	case SQLITE_DONE:
		return true;
	case SQLITE_ROW:
		return false;
	default:
		HandleError(sqlite3_errmsg(db));
		return false;
	}
}

#endif // PROFILE_SQL

#ifdef PROFILE_SQL

void SqlStatement::Finalize()
{
	Timer timer;
	int result = sqlite3_finalize(statement);
	timer.Stop();

	if (result == SQLITE_OK)
		info.FinalizationTime += timer.GetElapsedSeconds();
	else
		::NKDbgPrintfW(L"%s\n", sqlite3_errmsg(db));
}

#else // PROFILE_SQL

void SqlStatement::Finalize()
{
	int result = sqlite3_finalize(statement);
	if (result != SQLITE_OK)
		::NKDbgPrintfW(L"%s\n", sqlite3_errmsg(db));
}

#endif // PROFILE_SQL

void SqlStatement::Bind(__int32 n)
{
	int result(sqlite3_bind_int(statement, bindIndex++, n));
	if (result != SQLITE_OK)
		HandleError(sqlite3_errmsg(db));
}

void SqlStatement::Bind(__int64 n)
{
	int result(sqlite3_bind_int64(statement, bindIndex++, n));
	if (result != SQLITE_OK)
		HandleError(sqlite3_errmsg(db));
}

void SqlStatement::Bind(double n)
{
	int result(sqlite3_bind_double(statement, bindIndex++, n));
	if (result != SQLITE_OK)
		HandleError(sqlite3_errmsg(db));
}

void SqlStatement::Bind(const string & text)
{
	int result = sqlite3_bind_text
		( statement
		, bindIndex++
		, text.c_str()
		, text.size()
		, SQLITE_TRANSIENT
		);
	if (result != SQLITE_OK)
		HandleError(sqlite3_errmsg(db));
}

void SqlStatement::Bind(const wstring & text)
{
	vector<unsigned char> textUtf8Chars;
	const char * textUtf8 = reinterpret_cast<const char *>(ConvertToUtf8(text, textUtf8Chars));
	int result = sqlite3_bind_text
		( statement
		, bindIndex++
		, textUtf8
		, textUtf8Chars.size()
		, SQLITE_TRANSIENT
		);
	if (result != SQLITE_OK)
		HandleError(sqlite3_errmsg(db));
}

void SqlStatement::Bind(const Blob & blob)
{
	if (blob.empty())
	{
		++bindIndex;
		return;
	}
	int result = sqlite3_bind_blob
		( statement
		, bindIndex++
		, &blob[0]
		, blob.size()
		, SQLITE_STATIC
		);
	if (result != SQLITE_OK)
		HandleError(sqlite3_errmsg(db));
}

void SqlStatement::Bind(const Guid & guid)
{
	const string & text(guid);
	int result = sqlite3_bind_text
		( statement
		, bindIndex++
		, text.c_str()
		, text.size()
		, SQLITE_TRANSIENT
		);
	if (result != SQLITE_OK)
		HandleError(sqlite3_errmsg(db));
}

void SqlStatement::BindNull()
{
	int result = sqlite3_bind_null(statement, bindIndex++);
	if (result != SQLITE_OK)
		HandleError(sqlite3_errmsg(db));
}

void SqlStatement::Get(bool & n)
{
	n = (sqlite3_column_int(statement, getIndex++) != 0);
}

void SqlStatement::Get(__int16 & n)
{
	n = sqlite3_column_int(statement, getIndex++);
}

void SqlStatement::Get(__int32 & n)
{
	n = sqlite3_column_int(statement, getIndex++);
}

void SqlStatement::Get(__int64 & n)
{
	n = sqlite3_column_int64(statement, getIndex++);
}

void SqlStatement::Get(double & n)
{
	n = sqlite3_column_double(statement, getIndex++);
}

void SqlStatement::Get(string & text)
{
	const unsigned char * temp(sqlite3_column_text(statement, getIndex++));
	if (temp)
		text = reinterpret_cast<const char *>(temp);
}

void SqlStatement::Get(wstring & text)
{
	ConvertToUnicode(sqlite3_column_text(statement, getIndex++), text);
}

void SqlStatement::Get(Guid & guid)
{
	string temp;
	Get(temp);
	guid = Guid(temp);
}

void SqlStatement::Get(Timestamp & time)
{
	__int64 temp;
	Get(temp);
	time = static_cast<time_t>(temp);
}

void SqlStatement::HandleError(const std::string msg)
{
	::NKDbgPrintfW(L"%s\n", Tools::ConvertToUnicode(msg).c_str());
	throw std::exception(msg.c_str());
}
