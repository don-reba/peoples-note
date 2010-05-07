#pragma once

#include "IUserStore.h"

class UserStore : public IUserStore
{
public:

	virtual AuthenticationResult GetAuthenticationToken
		( std::wstring username
		, std::wstring password
		);
};
