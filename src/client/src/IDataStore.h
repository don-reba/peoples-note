#pragma once

class INotebook;

class IDataStore
{
public:

	virtual void LoadOrCreate(std::wstring name) = 0;

	virtual void AddNotebook(INotebook & notebook) = 0;

	virtual void MakeNotebookDefault(INotebook & notebook) = 0;

	virtual int GetNotebookCount() = 0;
};
