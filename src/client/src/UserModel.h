#pragma once

#include "IUserModel.h"

#include "CredentialsModel.h"
#include "Note.h"
#include "Notebook.h"

class IDataStore;

class UserModel : public IUserModel
{
private:

	IDataStore & dataStore;

	CredentialsModel credentialsModel;

	signal SignalLoaded;

	Notebook lastUsedNotebook; // temporary

public:

	UserModel(IDataStore & dataStore);

	virtual void ConnectLoaded(slot_type OnLoaded);

	virtual void CreateDefaultUser();
	
	virtual void SetCredentials(const ICredentialsModel & credentials);

	virtual INotebook & GetLastUsedNotebook();

	virtual boost::ptr_vector<INotebook> & GetNotebooks();

	virtual boost::ptr_vector<INote> & GetNotesByNotebook(INotebook & notebook);

	virtual boost::ptr_vector<INote> & GetNotesBySearch(std::wstring search);

	virtual void Load();
};
