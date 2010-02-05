#pragma once

#include "ICredentialsModel.h"

class MockCredentialsModel : public ICredentialsModel
{
public:
	
	std::wstring username;
	std::wstring password;

public:

	virtual ICredentialsModel & operator =
		( const ICredentialsModel & credentials
		);
	
	virtual std::wstring GetUsername() const;

	virtual std::wstring GetPassword() const;

	virtual void SetCredentials(std::wstring username, std::wstring password);
};
