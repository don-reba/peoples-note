#pragma once

#include "AuthenticationToken.h"

class IUserStore
{
public:

	class AuthenticationResult
	{
	public:
		bool                IsGood;
		std::wstring        Message;
		AuthenticationToken Token;
	};

public:

	virtual AuthenticationResult GetAuthenticationToken
		( std::wstring username
		, std::wstring password
		) = 0;
};
