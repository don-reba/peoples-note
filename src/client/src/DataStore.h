#pragma once

#include "IDataStore.h"
#include "Notebook.h"
#include "SQLite/sqlite3.h"

class DataStore : public IDataStore
{
private:

	sqlite3 * db;

	std::wstring folder;
	std::wstring path;

	boost::shared_ptr<INotebook> defaultNotebook;

	boost::ptr_vector<INotebook> notebooks;

// interface

public:

	DataStore(std::wstring folder);

	~DataStore();

	int GetVersion();

	INotebook & GetDefaultNotebook();

	std::wstring GetUser();

	void MakeNotebookLastUsed(INotebook & notebook);

// IDataStore implementation

	virtual void LoadOrCreate(std::wstring name);

	virtual void AddNotebook(INotebook & notebook);

	virtual void MakeNotebookDefault(INotebook & notebook);

	virtual INotebook & GetLastUsedNotebook();

	virtual boost::ptr_vector<INotebook> & GetNotebooks();

	virtual int GetNotebookCount();

// utility functions

private:

	void Connect();

	void Disconnect();

	std::wstring CreatePathFromName(std::wstring name);

	void Initialize(std::wstring name);

	std::wstring GetProperty(std::wstring name);

// SQLite wrappers

private:

	sqlite3_stmt * PrepareStatement(const char * sql);

	void BindText
		( sqlite3_stmt * statement
		, std::wstring   text
		, int            index
		);

	bool ExecuteStatement(sqlite3_stmt * statement);

	int GetColumnInt(sqlite3_stmt * statement, int index);

	std::wstring GetColumnText(sqlite3_stmt * statement, int index);

	void CloseStatement(sqlite3_stmt * statement);
};
