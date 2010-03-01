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

	virtual boost::ptr_vector<INotebook> & GetNotebooks() const;

	virtual std::vector<INote*> GetNotesByNotebook(INotebook & notebook) const;

	virtual std::vector<INote*> GetNotesBySearch(std::wstring search) const;

	virtual void Load();
};
