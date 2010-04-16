#pragma once

#include "ILastUserModel.h"
#include "IRegistryKey.h"

class LastUserModel : public ILastUserModel
{
private:

	IRegistryKey & registryKey;

public:

	LastUserModel(IRegistryKey & registryKey);

	virtual std::wstring GetUsername();

	virtual void SetUsername(const std::wstring & username);
};
