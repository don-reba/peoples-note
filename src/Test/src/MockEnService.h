#pragma once

#include "IEnService.h"

class MockEnService : public IEnService
{
public:

	CredentialsValidity credentialsValidity;

	std::wstring username;
	std::wstring password;

	bool hasSynced;

public:

	MockEnService();

public:

	virtual CredentialsValidity CheckCredentials
		( const std::wstring & username
		, const std::wstring & password
		);

	virtual void Sync(IUserModel & userModel);
};
