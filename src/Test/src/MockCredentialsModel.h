#pragma once

#include "ICredentialsModel.h"

class MockCredentialsModel : public ICredentialsModel
{
public:
	
	std::wstring username;
	std::wstring password;

	signal SignalNeedsSet;

public:

	MockCredentialsModel();

	MockCredentialsModel
		( std::wstring username
		, std::wstring password
		);

public:

	virtual void ConnectNeedsSet(slot_type OnNeedsSet);
	
	virtual std::wstring GetUsername() const;

	virtual std::wstring GetPassword() const;

	virtual void SetCredentials(std::wstring username, std::wstring password);
};
