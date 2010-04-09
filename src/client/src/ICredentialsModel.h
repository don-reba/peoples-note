#pragma once

#include "ISignalProvider.h"

class ICredentialsModel : public ISignalProvider
{
public:

	virtual void ConnectUpdated(slot_type OnUpdated) = 0;

	virtual void ConnectUpdating(slot_type OnUpdating) = 0;
	
	virtual std::wstring GetUsername() const = 0;

	virtual std::wstring GetPassword() const = 0;

	virtual void SetCredentials
		( std::wstring username
		, std::wstring password
		) = 0;

	virtual void Update() = 0;
};
