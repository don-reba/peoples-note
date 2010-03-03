#pragma once

#include "IDataStore.h"
#include "INotebook.h"
#include "INote.h"
#include "SQLite/sqlite3.h"

class DataStore : public IDataStore
{
private:

	sqlite3 * db;

	std::wstring folder;
	std::wstring path;

	boost::shared_ptr<INotebook> defaultNotebook;

	boost::ptr_vector<INotebook> notebooks;

	boost::ptr_vector<INote> notes;

// interface

public:

	DataStore(std::wstring folder);

	~DataStore();

	void AddNote(const INote & note, const INotebook & notebook);

	INotebook & GetDefaultNotebook();

	std::wstring GetUser();

	int GetVersion();

	void MakeNotebookLastUsed(const INotebook & notebook);

// IDataStore implementation

	virtual void AddNotebook(const INotebook & notebook);

	virtual INotebook & GetLastUsedNotebook();

	virtual int GetNotebookCount();

	virtual boost::ptr_vector<INotebook> & GetNotebooks();

	virtual boost::ptr_vector<INote> & GetNotesByNotebook(const INotebook & notebook);

	virtual boost::ptr_vector<INote> & GetNotesBySearch(std::wstring search);

	virtual void LoadOrCreate(std::wstring name);

	virtual void MakeNotebookDefault(const INotebook & notebook);

// utility functions

private:

	void AddProperty(std::wstring key, std::wstring value);

	void Connect();

	std::wstring CreatePathFromName(std::wstring name);

	void CreateTable(const char * sql);

	void Disconnect();

	std::wstring GetProperty(std::wstring name);

	void Initialize(std::wstring name);

// SQLite wrappers

private:

	sqlite3_stmt * PrepareStatement(const char * sql);

	void BindInt(sqlite3_stmt * statement, __int64 n, int index);

	void BindText
		( sqlite3_stmt * statement
		, std::string    text
		, int            index
		);

	void BindText
		( sqlite3_stmt * statement
		, std::wstring   text
		, int            index
		);

	bool ExecuteStatement(sqlite3_stmt * statement);

	int GetColumnInt(sqlite3_stmt * statement, int index);

	std::wstring GetColumnText(sqlite3_stmt * statement, int index);

	void CloseStatement(sqlite3_stmt * statement);

	__int64 GetLastInsertRowid();

	void SetPragma(const char * sql);
};
