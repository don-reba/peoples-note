#pragma once

#include "IEnService.h"

class MockEnService : public IEnService
{
public:

	CredentialsValidity credentialsValidity;

	std::wstring username;
	std::wstring password;

public:

	MockEnService();

public:

	virtual CredentialsValidity CheckCredentials
		( const std::wstring & username
		, const std::wstring & password
		);
};
