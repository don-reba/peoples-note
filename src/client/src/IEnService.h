#pragma once

#include "Note.h"
#include "Notebook.h"

class IUserModel;

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

	virtual void Sync(IUserModel & userModel) = 0;
};
