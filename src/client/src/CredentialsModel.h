#pragma once

#include "ICredentialsModel.h"

class CredentialsModel : public ICredentialsModel
{
	MacroEvent(Commit)
	MacroEvent(Set)
	MacroEvent(Update)

private:

	std::wstring username;
	std::wstring password;
	std::wstring status;

public:

	virtual void Commit();
	
	virtual std::wstring GetUsername() const;

	virtual std::wstring GetPassword() const;

	virtual std::wstring GetStatus() const;

	virtual void Set
		( const std::wstring & username
		, const std::wstring & password
		);

	virtual void Update(const std::wstring & status);
};
