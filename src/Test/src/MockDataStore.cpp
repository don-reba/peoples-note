#include "stdafx.h"
#include "MockDataStore.h"

using namespace std;

void MockDataStore::AddNotebook(const INotebook & notebook)
{
	notebooks.push_back(new MockNotebook(notebook));
}

INotebook & MockDataStore::GetLastUsedNotebook()
{
	return lastUsedNotebook;
}

int MockDataStore::GetNotebookCount()
{
	return notebooks.size();
}

boost::ptr_vector<INotebook> & MockDataStore::GetNotebooks()
{
	return notebooks;
}

boost::ptr_vector<INote> & MockDataStore::GetNotesByNotebook
	( const INotebook & notebook
	)
{
	getNotesByNotebookNotebook = notebook;
	return notesByNotebook;
}

boost::ptr_vector<INote> & MockDataStore::GetNotesBySearch
	( std::wstring search
	)
{
	getNotesBySearchSearch = search;
	return notesBySearch;
}

void MockDataStore::LoadOrCreate(wstring name)
{
	this->name = name;
}

void MockDataStore::MakeNotebookDefault(const INotebook & notebook)
{
	defaultNotebook = notebook;
}

void MockDataStore::MakeNotebookLastUsed(const INotebook & notebook)
{
	lastUsedNotebook = notebook;
}