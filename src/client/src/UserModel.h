#pragma once
#include "IUserModel.h"

#include "CredentialsModel.h"

class IDataStore;

class UserModel : public IUserModel
{
private:

	IDataStore & dataStore;

	CredentialsModel credentialsModel;

	signal SignalLoaded;

public:

	UserModel(IDataStore & dataStore);

	virtual void AddNote(const Note & note, const Notebook & notebook);

	virtual void ConnectLoaded(slot_type OnLoaded);

	virtual void CreateDefaultUser();
	
	virtual void SetCredentials(const ICredentialsModel & credentials);

	virtual Notebook & GetLastUsedNotebook();

	virtual const NotebookList & GetNotebooks();

	virtual const NoteList & GetNotesByNotebook(const Notebook & notebook);

	virtual const NoteList & GetNotesBySearch(std::wstring search);

	virtual void Load();
};
