#pragma once
#include "ILastUserModel.h"

class IRegistryKey;

class LastUserModel : public ILastUserModel
{
private:

	IRegistryKey & registryKey;

public:

	LastUserModel(IRegistryKey & registryKey);

	virtual std::wstring GetShard() const;

	virtual std::wstring GetToken() const;

	virtual std::wstring GetUsername() const;

	virtual void SetCredentials
		( const std::wstring & username
		, const std::wstring & token
		, const std::wstring & shard
		);
};
