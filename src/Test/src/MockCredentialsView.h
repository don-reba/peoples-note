#pragma once

#include "ICredentialsView.h"

class MockCredentialsView : public ICredentialsView
{
private:

	signal SignalSignIn;

public:

	std::wstring username;
	std::wstring password;

public:

	virtual void ConnectSignIn(slot_type OnSignIn);

	virtual std::wstring GetUsername() const;

	virtual void SetUsername(std::wstring username);

	virtual std::wstring GetPassword() const;

	virtual void SetPassword(std::wstring password);

	void SignIn();
};