#pragma once

#include "ISignalProvider.h"

class ICredentialsView : public ISignalProvider
{
	MacroIEvent(Cancel)
	MacroIEvent(Created)
	MacroIEvent(Ok)

public:

	virtual ~ICredentialsView() {}

	virtual void Close() = 0;

	virtual std::wstring GetPassword() const = 0;

	virtual std::wstring GetUsername() const = 0;

	virtual void Open() = 0;

	virtual void SetFocusToPassword() = 0;

	virtual void SetFocusToUsername() = 0;

	virtual void SetMessage(const std::wstring & message) = 0;

	virtual void SetPassword(const std::wstring & password) = 0;

	virtual void SetUsername(const std::wstring & username) = 0;
};
