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
		std::wstring        ShardId;
		AuthenticationToken Token;
	};

public:

	virtual AuthenticationResult GetAuthenticationToken
		( std::wstring username
		, std::wstring password
		) = 0;
};
