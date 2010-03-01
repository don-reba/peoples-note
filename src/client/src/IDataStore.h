#pragma once

class INotebook;

class IDataStore
{
public:

	virtual void LoadOrCreate(std::wstring name) = 0;

	virtual void AddNotebook(INotebook & notebook) = 0;

	virtual void MakeNotebookDefault(INotebook & notebook) = 0;

	virtual INotebook & GetLastUsedNotebook() = 0;

	virtual boost::ptr_vector<INotebook> & GetNotebooks() = 0;

	virtual int GetNotebookCount() = 0;
};
