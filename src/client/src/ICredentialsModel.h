#pragma once

#include "ISignalProvider.h"

class ICredentialsModel : public ISignalProvider
{
public:

	virtual void ConnectNeedsSet(slot_type OnNeedsSet) = 0;
	
	virtual std::wstring GetUsername() const = 0;

	virtual std::wstring GetPassword() const = 0;

	virtual void SetCredentials
		( std::wstring username
		, std::wstring password
		) = 0;
};
