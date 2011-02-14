#pragma once

#include "IUserStore.h"

class ILogger;

class UserStore : public IUserStore
{
private:

	ILogger & logger;

public:

	UserStore(ILogger & logger);

	virtual AuthenticationResult GetAuthenticationToken
		( std::wstring username
		, std::wstring password
		);
};
