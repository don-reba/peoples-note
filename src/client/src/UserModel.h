#pragma once

#include "ISignalProvider.h"
#include "IUserModel.h"
#include "CredentialsModel.h"
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

	virtual void Load();
};
