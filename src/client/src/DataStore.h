#pragma once
#include "IDataStore.h"

#include "SQLite/sqlite3.h"

class DataStore : public IDataStore
{
private:

	sqlite3 * db;

	std::wstring folder;
	std::wstring path;

	boost::shared_ptr<Notebook> defaultNotebook;

	NotebookList notebooks;
	NoteList     notes;

// interface

public:

	DataStore(std::wstring folder);

	~DataStore();

	Notebook & GetDefaultNotebook();

	std::wstring GetUser();

	int GetVersion();

// IDataStore implementation

public:

	virtual void AddNote
		( const Note     & note
		, const Notebook & notebook
		);

	virtual void AddNotebook(const Notebook & notebook);

	virtual Notebook & GetLastUsedNotebook();

	virtual int GetNotebookCount();

	virtual const NotebookList & GetNotebooks();

	virtual const NoteList & GetNotesByNotebook
		( const Notebook & notebook
		);

	virtual const NoteList & GetNotesBySearch
		( std::wstring search
		);

	virtual void LoadOrCreate(std::wstring name);

	virtual void MakeNotebookDefault(const Notebook & notebook);

	virtual void MakeNotebookLastUsed(const Notebook & notebook);

// utility functions

private:

	void AddProperty(std::wstring key, std::wstring value);

	void Create();

	std::wstring CreatePathFromName(std::wstring name);

	void CreateTable(const char * sql);

	void Disconnect();

	std::wstring GetProperty(std::wstring name);

	void Initialize(std::wstring name);

	void SetPragma(const char * sql);

	bool TryLoad();

// SQLite wrappers

private:

	__int64 GetLastInsertRowid();
};
