#pragma once

#include "CredentialsModel.h"
#include "ILastUserModel.h"
#include "IRegistryKey.h"

class LastUserModel : public ILastUserModel
{
private:

	IRegistryKey & registryKey;
	CredentialsModel credentialsModel;

public:

	LastUserModel(IRegistryKey & registryKey);

	virtual std::wstring GetUsername();

	virtual void SetUsername(const std::wstring & username);
};
