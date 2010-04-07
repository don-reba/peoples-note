#pragma once

#include "ICredentialsView.h"

class MockCredentialsView : public ICredentialsView
{
public:

	std::wstring message;
	std::wstring password;
	std::wstring username;

	signal SignalCancel;
	signal SignalCreated;
	signal SignalOk;

	bool isOpen;

public:

	MockCredentialsView();

public:

	virtual void Close();

	virtual void ConnectCancel(slot_type OnCancel);

	virtual void ConnectCreated(slot_type OnCreated);

	virtual void ConnectOk(slot_type OnOk);

	virtual std::wstring GetPassword() const;

	virtual std::wstring GetUsername() const;

	virtual void Open();

	virtual void SetMessage(const std::wstring & message);

	virtual void SetPassword(const std::wstring & password);

	virtual void SetUsername(const std::wstring & username);
};
