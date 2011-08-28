#pragma once
#include "ILastUserModel.h"

class IRegistryKey;

class LastUserModel : public ILastUserModel
{
private:

	IRegistryKey & registryKey;

public:

	LastUserModel(IRegistryKey & registryKey);

	virtual std::wstring GetPassword() const;

	virtual std::wstring GetUsername() const;

	virtual void SetCredentials
		( const std::wstring & username
		, const std::wstring & password
		);
};
