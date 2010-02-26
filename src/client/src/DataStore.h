#pragma once

#include "IDataStore.h"

class DataStore : public IDataStore
{
public:

	virtual void LoadOrCreate(std::wstring name);

	virtual void AddNotebook(INotebook & notebook);

	virtual void MakeNotebookDefault(INotebook & notebook);

	virtual int GetNotebookCount();
};
