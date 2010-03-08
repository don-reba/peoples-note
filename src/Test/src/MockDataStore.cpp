#include "stdafx.h"
#include "MockDataStore.h"

using namespace std;

MockDataStore::MockDataStore()
	: defaultNotebook            (Guid(), L"default-notebook")
	, getNotesByNotebookNotebook (Guid(), L"notes-notebook")
	, lastUsedNotebook           (Guid(), L"last-used-notebook")
{
}

void MockDataStore::AddNote
	( const Note     & note
	, const Notebook & notebook
	)
{
	addedNotes.push_back(make_pair(note.GetTitle(), notebook.GetName()));
}

void MockDataStore::AddNotebook(const Notebook & notebook)
{
	notebooks.push_back(Notebook(notebook));
}

Notebook & MockDataStore::GetLastUsedNotebook()
{
	return lastUsedNotebook;
}

int MockDataStore::GetNotebookCount()
{
	return notebooks.size();
}

const NotebookList & MockDataStore::GetNotebooks()
{
	return notebooks;
}

const NoteList & MockDataStore::GetNotesByNotebook
	( const Notebook & notebook
	)
{
	getNotesByNotebookNotebook = notebook;
	return notesByNotebook;
}

const NoteList & MockDataStore::GetNotesBySearch
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

void MockDataStore::MakeNotebookDefault(const Notebook & notebook)
{
	defaultNotebook = notebook;
}

void MockDataStore::MakeNotebookLastUsed(const Notebook & notebook)
{
	lastUsedNotebook = notebook;
}