#pragma once

#include "IUserModel.h"
#include "MockCredentialsModel.h"
#include "Note.h"
#include "Notebook.h"

class MockUserModel : public IUserModel
{
// data

public:

	MockCredentialsModel credentialsModel;

	Notebook defaultNotebook;
	Notebook lastUsedNotebook;

	NotebookList notebooks;
	NoteList     notes;

	typedef std::pair<std::wstring, std::wstring> AddedNote;
	std::vector<AddedNote> addedNotes;

	bool isDefault;
	bool isLoaded;

	mutable std::wstring notebookSelection;
	mutable std::wstring searchSelection;
	
	signal SignalLoaded;

// interface

public:

	MockUserModel();

// IUserModel implementation

public:

	virtual void AddNote(const Note & note, const Notebook & notebook);

	virtual void AddNotebook(const Notebook & notebook);

	virtual void ConnectLoaded(slot_type OnLoaded);

	virtual void CreateDefaultUser();

	virtual Notebook GetDefaultNotebook();

	virtual Notebook GetLastUsedNotebook();

	virtual const NotebookList & GetNotebooks();

	virtual const NoteList & GetNotesByNotebook(const Notebook & notebook);

	virtual const NoteList & GetNotesBySearch(std::wstring search);

	virtual void Load();

	virtual void MakeNotebookDefault(const Notebook & notebook);

	virtual void MakeNotebookLastUsed(const Notebook & notebook);
	
	virtual void SetCredentials(const ICredentialsModel & credentials);
};
