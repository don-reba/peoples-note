#pragma once

#include "ISignalProvider.h"

class ICredentialsModel : public ISignalProvider
{
	MacroIEvent(Commit)
	MacroIEvent(Set)
	MacroIEvent(Update)

public:

	virtual void Commit() = 0;

	virtual std::wstring GetShard() const = 0;

	virtual std::wstring GetToken() const = 0;
	
	virtual std::wstring GetUsername() const = 0;

	virtual std::wstring GetStatus() const = 0;

	virtual void Set
		( const std::wstring & username
		, const std::wstring & token
		, const std::wstring & shard
		) = 0;

	virtual void Update(const std::wstring & status) = 0;
};
