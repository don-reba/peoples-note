#pragma once

#include "ICredentialsModel.h"

class MockCredentialsModel : public ICredentialsModel
{
public:
	
	std::wstring username;
	std::wstring password;

	signal SignalUpdated;
	signal SignalUpdating;

public:

	MockCredentialsModel();

	MockCredentialsModel
		( std::wstring username
		, std::wstring password
		);

public:

	virtual void ConnectUpdated(slot_type OnUpdated);

	virtual void ConnectUpdating(slot_type OnUpdating);
	
	virtual std::wstring GetUsername() const;

	virtual std::wstring GetPassword() const;

	virtual void SetCredentials(std::wstring username, std::wstring password);

	virtual void Update();
};
