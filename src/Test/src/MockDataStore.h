#pragma once

#include "IDataStore.h"
#include "Note.h"
#include "Notebook.h"

class MockDataStore : public IDataStore
{
public:

	std::wstring name;
	Notebook     defaultNotebook;
	Notebook     lastUsedNotebook;

	NotebookList notebooks;
	NoteList     notesByNotebook;
	NoteList     notesBySearch;

	Notebook     getNotesByNotebookNotebook;
	std::wstring getNotesBySearchSearch;

	typedef std::pair<std::wstring, std::wstring> AddedNote;
	std::vector<AddedNote> addedNotes;

public:

	MockDataStore();

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
};
