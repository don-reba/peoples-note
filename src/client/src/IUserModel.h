#pragma once

#include "ISignalProvider.h"
#include "Note.h"
#include "Notebook.h"

class ICredentialsModel;

class IUserModel : public ISignalProvider
{
public:

	virtual ~IUserModel() {}

	virtual void AddNote(const Note & note, const Notebook & notebook) = 0;

	virtual void AddNotebook(const Notebook & notebook) = 0;

	virtual void ConnectLoaded(slot_type OnLoaded) = 0;

	virtual void CreateDefaultUser() = 0;
	
	virtual void SetCredentials(const ICredentialsModel & credentials) = 0;

	virtual Notebook GetDefaultNotebook() = 0;

	virtual Notebook GetLastUsedNotebook() = 0;

	virtual const NotebookList & GetNotebooks() = 0;

	virtual const NoteList & GetNotesByNotebook(const Notebook & notebook) = 0;

	virtual const NoteList & GetNotesBySearch(std::wstring search) = 0;

	virtual void Load() = 0;

	virtual void MakeNotebookDefault(const Notebook & notebook) = 0;

	virtual void MakeNotebookLastUsed(const Notebook & notebook) = 0;
};
