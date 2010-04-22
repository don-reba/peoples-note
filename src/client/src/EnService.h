#pragma once

#include "IEnService.h"

class EnService : public IEnService
{
public:

	virtual CredentialsValidity CheckCredentials
		( const std::wstring & username
		, const std::wstring & password
		);

	virtual void Sync(IUserModel & userModel);
};
