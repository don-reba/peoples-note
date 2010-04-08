#pragma once

#include "ICredentialsModel.h"

class CredentialsModel : public ICredentialsModel
{
private:

	std::wstring username;
	std::wstring password;

	signal SignalUpdating;

public:

	virtual void ConnectUpdating(slot_type OnUpdating);
	
	virtual std::wstring GetUsername() const;

	virtual std::wstring GetPassword() const;

	virtual void SetCredentials
		( std::wstring username
		, std::wstring password
		);
};
