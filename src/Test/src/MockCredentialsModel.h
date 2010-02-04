#pragma once

#include "ICredentialsModel.h"

class MockCredentialsModel : public ICredentialsModel
{
private:
	
	std::wstring username;
	std::wstring password;

public:
	
	virtual std::wstring GetUsername() const;

	virtual std::wstring GetPassword() const;

	virtual void SetCredentials(std::wstring username, std::wstring password);
};
