#pragma once

#include "IUserModel.h"
#include "MockCredentialsModel.h"
#include "MockNotebook.h"

class MockUserModel : public IUserModel
{
private:
	
	signal SignalLoaded;

public:

	MockCredentialsModel credentialsModel;
	MockNotebook         lastUsedNotebook;
	bool                 isDefault;
	bool                 isLoaded;

public:

	MockUserModel();

	void Loaded();

	virtual void ConnectLoaded(slot_type OnLoaded);

	virtual void CreateDefaultUser();
	
	virtual void SetCredentials(const ICredentialsModel & credentials);

	virtual INotebook & GetLastUsedNotebook();

	virtual void Load();
};
