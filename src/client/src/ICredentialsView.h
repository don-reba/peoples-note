#pragma once

#include "ISignalProvider.h"

class ICredentialsView : public ISignalProvider
{
public:

	virtual ~ICredentialsView() {}

	virtual void Close() = 0;

	virtual void ConnectCancel(slot_type OnCancel) = 0;

	virtual void ConnectCreated(slot_type OnCreated) = 0;

	virtual void ConnectOk(slot_type OnOk) = 0;

	virtual std::wstring GetPassword() const = 0;

	virtual std::wstring GetUsername() const = 0;

	virtual void Open() = 0;

	virtual void SetMessage(const std::wstring & message) = 0;

	virtual void SetPassword(const std::wstring & password) = 0;

	virtual void SetUsername(const std::wstring & username) = 0;
};
