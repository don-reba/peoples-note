#pragma once

#include "IUserModel.h"
#include "MockCredentialsModel.h"

class MockUserModel : public IUserModel
{
public:

	MockCredentialsModel credentialsModel;
	bool                 isDefault;
	bool                 isLoaded;

public:

	MockUserModel();

	virtual void CreateDefaultUser();
	
	virtual void SetCredentials(const ICredentialsModel & credentials);

	virtual void Load();
};
