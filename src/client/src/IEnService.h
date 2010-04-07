#pragma once

class IEnService
{
public:

	struct CredentialsValidity
	{
		bool         IsGood;
		std::wstring Message;
	};

public:

	virtual CredentialsValidity CheckCredentials
		( const std::wstring & username
		, const std::wstring & password
		) = 0;
};
