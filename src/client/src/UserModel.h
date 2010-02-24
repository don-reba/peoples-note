#pragma once

#include "IUserModel.h"

#include "CredentialsModel.h"
#include "Note.h"
#include "Notebook.h"

class UserModel : public IUserModel
{
private:

	CredentialsModel credentialsModel;

	mutable std::vector<Notebook> notebooks;

	signal SignalLoaded;

public:

	virtual void ConnectLoaded(slot_type OnLoaded);

	virtual void CreateDefaultUser();
	
	virtual void SetCredentials(const ICredentialsModel & credentials);

	virtual INotebook & GetLastUsedNotebook();

	virtual std::vector<INotebook*> GetNotebooks() const;

	virtual std::vector<INote*> GetNotesByNotebook(INotebook & notebook) const;

	virtual std::vector<INote*> GetNotesBySearch(std::wstring search) const;

	virtual void Load();
};
