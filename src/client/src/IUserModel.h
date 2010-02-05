#pragma once

#include "ICredentialsModel.h"

class IUserModel
{
public:

	virtual void CreateDefaultUser() = 0;
	
	virtual void SetCredentials(const ICredentialsModel & credentials) = 0;

	virtual void Load() = 0;
};
