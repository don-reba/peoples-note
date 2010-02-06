#pragma once

#include "IUserModel.h"
#include "MockCredentialsModel.h"
#include "MockNotebook.h"

class MockUserModel : public IUserModel
{
private:
	
	signal SignalLoaded;

public:

	MockCredentialsModel    credentialsModel;
	MockNotebook            lastUsedNotebook;
	std::vector<INotebook*> notebooks;
	bool                    isDefault;
	bool                    isLoaded;

public:

	MockUserModel();

	void Loaded();

	virtual void ConnectLoaded(slot_type OnLoaded);

	virtual void CreateDefaultUser();
	
	virtual void SetCredentials(const ICredentialsModel & credentials);

	virtual INotebook & GetLastUsedNotebook();

	virtual std::vector<INotebook*> GetNotebooks() const;

	virtual void Load();
};
