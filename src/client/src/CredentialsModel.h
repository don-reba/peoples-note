#pragma once

#include "ICredentialsModel.h"

class CredentialsModel : public ICredentialsModel
{
	MacroEvent(Updated)
	MacroEvent(Updating)

private:

	std::wstring username;
	std::wstring password;

public:
	
	virtual std::wstring GetUsername() const;

	virtual std::wstring GetPassword() const;

	virtual void SetCredentials
		( std::wstring username
		, std::wstring password
		);

	virtual void Update();
};
