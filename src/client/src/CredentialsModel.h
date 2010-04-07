#pragma once

#include "ICredentialsModel.h"

class CredentialsModel : public ICredentialsModel
{
private:

	std::wstring username;
	std::wstring password;

	signal SignalNeedsSet;

public:

	virtual void ConnectNeedsSet(slot_type OnNeedsSet);
	
	virtual std::wstring GetUsername() const;

	virtual std::wstring GetPassword() const;

	virtual void SetCredentials
		( std::wstring username
		, std::wstring password
		);
};
