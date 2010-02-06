#pragma once

#include "ICredentialsModel.h"
#include "INotebook.h"
#include "ISignalProvider.h"

class IUserModel : public ISignalProvider
{
public:

	virtual void ConnectLoaded(slot_type OnLoaded) = 0;

	virtual void CreateDefaultUser() = 0;
	
	virtual void SetCredentials(const ICredentialsModel & credentials) = 0;

	virtual INotebook & GetLastUsedNotebook() = 0;

	virtual void Load() = 0;
};
