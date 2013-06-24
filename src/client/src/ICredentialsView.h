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

	virtual std::wstring GetShard() const = 0;

	virtual std::wstring GetToken() const = 0;

	virtual std::wstring GetUsername() const = 0;

	virtual void Open() = 0;

	virtual void SetFocusToUsername() = 0;

	virtual void SetMessage(const std::wstring & message) = 0;

	virtual void SetShard(const std::wstring & shard) = 0;

	virtual void SetToken(const std::wstring & token) = 0;

	virtual void SetUsername(const std::wstring & username) = 0;
};
