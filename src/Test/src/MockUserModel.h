#pragma once

#include "IUserModel.h"
#include "MockCredentialsModel.h"
#include "Note.h"
#include "Notebook.h"

class MockUserModel : public IUserModel
{
private:
	
	signal SignalLoaded;

public:

	MockCredentialsModel credentialsModel;
	Notebook             lastUsedNotebook;
	NotebookList         notebooks;
	NoteList             notes;

	typedef std::pair<std::wstring, std::wstring> AddedNote;
	std::vector<AddedNote> addedNotes;

	bool isDefault;
	bool isLoaded;

	mutable std::wstring notebookSelection;
	mutable std::wstring searchSelection;

public:

	MockUserModel();

	void Loaded();

	virtual void AddNote
		( const Note     & note
		, const Notebook & notebook
		);

	virtual void ConnectLoaded(slot_type OnLoaded);

	virtual void CreateDefaultUser();
	
	virtual void SetCredentials
		( const ICredentialsModel & credentials
		);

	virtual Notebook & GetLastUsedNotebook();

	virtual const NotebookList & GetNotebooks();

	virtual const NoteList & GetNotesByNotebook
		( const Notebook & notebook
		);

	virtual const NoteList & GetNotesBySearch
		( std::wstring search
		);

	virtual void Load();
};
