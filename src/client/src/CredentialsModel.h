#pragma once

#include "ICredentialsModel.h"

class CredentialsModel : public ICredentialsModel
{
private:

	std::wstring username;
	std::wstring password;

public:

	virtual ICredentialsModel & operator =
		( const ICredentialsModel & credentials
		);
	
	virtual std::wstring GetUsername() const;

	virtual std::wstring GetPassword() const;

	virtual void SetCredentials
		( std::wstring username
		, std::wstring password
		);
};
