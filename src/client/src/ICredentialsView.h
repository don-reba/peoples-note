#pragma once

#include "ISignalProvider.h"

class ICredentialsView : public ISignalProvider
{
public:

	virtual void ConnectSignIn(slot_type OnSignIn) = 0;

	virtual std::wstring GetUsername() const = 0;

	virtual void SetUsername(std::wstring username) = 0;

	virtual std::wstring GetPassword() const = 0;

	virtual void SetPassword(std::wstring password) = 0;
};
