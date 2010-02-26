#pragma once

#include "IDataStore.h"
#include "MockNotebook.h"

class MockDataStore : public IDataStore
{
public:

	std::wstring name;
	std::vector<MockNotebook> notebooks;
	MockNotebook defaultNotebook;

public:

	virtual void LoadOrCreate(std::wstring name);

	virtual void AddNotebook(INotebook & notebook);

	virtual void MakeNotebookDefault(INotebook & notebook);

	virtual int GetNotebookCount();
};
