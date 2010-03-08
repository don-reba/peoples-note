#pragma once

#include "Note.h"
#include "Notebook.h"

class IDataStore
{
public:

	virtual ~IDataStore() {}

	virtual void AddNote
		( const Note     & note
		, const Notebook & notebook
		) = 0;

	virtual void AddNotebook(const Notebook & notebook) = 0;

	virtual Notebook & GetLastUsedNotebook() = 0;

	virtual int GetNotebookCount() = 0;

	virtual const NotebookList & GetNotebooks() = 0;

	virtual const NoteList & GetNotesByNotebook
		( const Notebook & notebook
		) = 0;

	virtual const NoteList & GetNotesBySearch
		( std::wstring search
		) = 0;

	virtual void LoadOrCreate(std::wstring name) = 0;

	virtual void MakeNotebookDefault(const Notebook & notebook) = 0;

	virtual void MakeNotebookLastUsed(const Notebook & notebook) = 0;
};
