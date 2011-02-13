#pragma once

#include "ISignalProvider.h"

class ICredentialsModel : public ISignalProvider
{
	MacroIEvent(Updated)
	MacroIEvent(Updating)

public:
	
	virtual std::wstring GetUsername() const = 0;

	virtual std::wstring GetPassword() const = 0;

	virtual void SetCredentials
		( std::wstring username
		, std::wstring password
		) = 0;

	virtual void Update() = 0;
};
