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

	int bindIndex;
	int getIndex;

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

	virtual void Bind(__int32              n);
	virtual void Bind(__int64              n);
	virtual void Bind(double               n);
	virtual void Bind(const std::string  & text);
	virtual void Bind(const std::wstring & text);
	virtual void Bind(const Blob         & blob);
	virtual void Bind(const Guid         & guid);

	virtual void BindNull();

	virtual void Get(bool         & n);
	virtual void Get(__int16      & n);
	virtual void Get(__int32      & n);
	virtual void Get(__int64      & n);
	virtual void Get(double       & n);
	virtual void Get(std::string  & text);
	virtual void Get(std::wstring & text);
	virtual void Get(Guid         & guid);
	virtual void Get(Timestamp    & time);

private:

	void HandleError(const std::string msg);

	virtual void Finalize();
};
