#include "stdafx.h"
#include "MockDataStore.h"

using namespace std;

void MockDataStore::LoadOrCreate(wstring name)
{
	this->name = name;
}

void MockDataStore::AddNotebook(INotebook & notebook)
{
	notebooks.push_back(notebook);
}

void MockDataStore::MakeNotebookDefault(INotebook & notebook)
{
	defaultNotebook = notebook;
}

int MockDataStore::GetNotebookCount()
{
	return notebooks.size();
}