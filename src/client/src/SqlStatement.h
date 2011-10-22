#pragma once
#include "ISqlStatement.h"

struct sqlite3_stmt;
struct sqlite3;
class  SqlStatementInfo;

class SqlStatement : public ISqlStatement
{
private:

	sqlite3_stmt * statement;
	sqlite3      * db;

#ifdef PROFILE_SQL
	SqlStatementInfo & info;
#endif // PROFILE_SQL

public:

#ifdef PROFILE_SQL
	SqlStatement(sqlite3 * db, const char * sql, SqlStatementInfo & info);
#else
	SqlStatement(sqlite3 * db, const char * sql);
#endif

	~SqlStatement();

// ISqlStatement implementation

public:

	virtual bool Execute();

	virtual void Bind(int index, __int32              n);
	virtual void Bind(int index, __int64              n);
	virtual void Bind(int index, double               n);
	virtual void Bind(int index, const std::string  & text);
	virtual void Bind(int index, const std::wstring & text);
	virtual void Bind(int index, const Blob         & blob);
	virtual void Bind(int index, const Guid         & guid);

	virtual void BindNull(int index);

	virtual void Get(int index, bool         & n);
	virtual void Get(int index, __int16      & n);
	virtual void Get(int index, __int32      & n);
	virtual void Get(int index, __int64      & n);
	virtual void Get(int index, double       & n);
	virtual void Get(int index, std::string  & text);
	virtual void Get(int index, std::wstring & text);
	virtual void Get(int index, Guid         & guid);
	virtual void Get(int index, Timestamp    & time);

	virtual bool IsNull(int index);

private:

	void HandleError(const std::string msg);

	virtual void Finalize();
};
