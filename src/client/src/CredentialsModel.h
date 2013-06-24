#pragma once

#include "ICredentialsModel.h"

class CredentialsModel : public ICredentialsModel
{
	MacroEvent(Commit)
	MacroEvent(Set)
	MacroEvent(Update)

private:

	std::wstring username;
	std::wstring token;
	std::wstring shard;
	std::wstring status;

public:

	virtual void Commit();

	virtual std::wstring GetShard() const;

	virtual std::wstring GetToken() const;
	
	virtual std::wstring GetUsername() const;

	virtual std::wstring GetStatus() const;

	virtual void Set
		( const std::wstring & username
		, const std::wstring & token
		, const std::wstring & shard
		);

	virtual void Update(const std::wstring & status);
};
