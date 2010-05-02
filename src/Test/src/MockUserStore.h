#pragma once
#include "IUserStore.h"

class MockUserStore : public IUserStore
{
public:

	AuthenticationResult authenticationResult;

public:

	MockUserStore();

	virtual AuthenticationResult GetAuthenticationToken
		( std::wstring username
		, std::wstring password
		);
};
