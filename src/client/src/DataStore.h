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

// interface

public:

	DataStore(std::wstring folder);

	~DataStore();

	int GetVersion();

	INotebook & GetDefaultNotebook();

	std::wstring GetUser();

// IDataStore implementation

	virtual void LoadOrCreate(std::wstring name);

	virtual void AddNotebook(INotebook & notebook);

	virtual void MakeNotebookDefault(INotebook & notebook);

	virtual int GetNotebookCount();

// utility functions

private:

	void Connect();

	void Disconnect();

	std::wstring CreatePathFromName(std::wstring name);

	void Initialize(std::wstring name);

	std::wstring GetProperty(std::wstring name);
};
